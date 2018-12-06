#include "stdafx.h"				// must be the first thing to include
#include "EntityList.h"
#include "LuaScript.h"
#include "GameSpaces.h"

EntityList::EntityList() :
	player(nullptr), entityID(0),
	luaScript(std::make_unique<LuaScript>())
{
	luaScript->Setup();
}

void EntityList::Initialize()
{
	for (auto& ent : entityList)
	{
		ent->InitializeComponents();
	}
	for (auto& item : gamespaces)
	{
		item->entity_list.Initialize();
	}
}

void EntityList::SerializeEntity(const Entity& ent, SerializeElem& oldElem) const
{
	auto& pElement = oldElem.InsertChild("Entity");
	if (ent.prefabName[0])
		pElement.SetText(&ent.prefabName[0]);

	for (auto& item : ent.components)
	{
		auto& cElement = pElement.InsertChild(
			ComponentDirectory::names[unsigned(item->GetType())].data());
		item->Serialize(cElement);
	}
	if (ent.children.size())
	{
		auto& childrenElem = pElement.InsertChild("Children");
		for (auto& item : ent.children)
			SerializeEntity(*item, childrenElem);
	}
}

void EntityList::SaveComponentData(Serializer & pRoot) const
{
	settings.Serialize(pRoot.NewElement("Settings"));
	auto& entElem = pRoot.NewElement("Entities");
	for (unsigned i = 0; i < entityList.size(); ++i)
	{
		if (entityList[i]->parent == nullptr)
			SerializeEntity(*entityList[i], entElem);
	}
}

void EntityList::Clear()
{
	for(auto& item: entityList)
		for(auto& comp : item->components)
			RemoveComponent(comp.get());
	entityList.clear();
}

unsigned EntityList::GetChildIndex(Entity* ent) const
{
	unsigned i = 0;
	for (auto& item : entityList)
	{
		if (item.get() == ent)
			return i;
		++i;
	}
	return 0;
}

void EntityList::EditorUpdate(float dt)
{
	for (auto& item : entityList)
		for (auto& item2 : item->components)
			item2->EditorUpdate(dt);
}

void EntityList::EditorSetup()
{
	for (auto& item : entityList)
		item->EditorSetup();
}

void EntityList::AssignComponents()
{
	luaScript->Setup();
	DWORD et = static_cast<DWORD>(GetTickCount64());
	for (auto& ent : entityList)
	{
		SetupEntityComponent(*ent);
	}
	for (auto& ent : entityList)
	{
		ent->UpdateComponentData();
	}
	for (auto& ent : entityList)
	{
		ent->UpdateRequiredComponents();
	}
	CONSOLE_SYSTEM("Setup Components elapsed: ", GetTickCount64() - et);
}

CameraC* EntityList::GetActiveCamera()
{
	auto& vecCam = componentMap[COMPONENTTYPE::CAMERA];
	for (auto item : vecCam)
	{
		if (!item->disabled)
			return static_cast<CameraC*>(item);
	}
	return nullptr;
}

std::vector<Entity*> EntityList::GetPrefabName(const std::string & name)
{
	std::vector<Entity*> ents;
	for (auto& item : entityList)
	{
		std::string pfName = item->prefabName.data();
		if (name == pfName)
			ents.emplace_back(item.get());
	}
	return ents;
}

void EntityList::SetupLuascripts()
{
	for (auto& item : entityList)
	{
		auto comp = item->GetComponent<ScriptC>();
		if (comp)
			comp->UpdateLuaVariables();
	}
	for (auto& item : gamespaces)
	{
		item->entity_list.SetupLuascripts();
	}
}

void EntityList::Awake()
{
	PreloadObjects();
	for (auto& item : entityList)
		item->Awake();
	for (auto& item : gamespaces)
		item->entity_list.Awake();
}

void EntityList::RemoveComponent(Component * comp)
{
	comp->DestroyUpdate();
	auto& vec = componentMap[comp->GetType()];
	auto iter = std::find(vec.begin(), vec.end(), comp);
	if (iter != vec.end())
		vec.erase(iter);
}

void EntityList::Update(float dt)
{
	// @TONOTE: Need to be careful (it was hardcoded as 1.f/60.f) as it might cause some updates to go haywire here
	/*static float dt_holder = 0;
	dt_holder += dt;
	while (dt_holder > oneOverSixty)
	{*/
		//dt_holder -= oneOverSixty;
		UpdateEntities(dt);
		for (auto& item : gamespaces)
			item->entity_list.Update(dt);
	//}
}

void EntityList::AddNewEntities(Entity& ent, 
	std::vector<std::unique_ptr<Entity>>& newEntities)
{
	for (auto& item : ent.newEntities)
	{
		item->eid = entityID++;
		if (auto comp = item->GetComponent<ScriptC>())
			comp->UpdateLuaVariables();
		item->UpdateRequiredComponents();
		SetupEntityComponent(*item);
		item->Awake();
		item->InitializeComponents();
		AddNewEntities(*item, newEntities);
	}
	newEntities.insert(newEntities.end(), 
		std::make_move_iterator(ent.newEntities.begin()), 
		std::make_move_iterator(ent.newEntities.end()));
	ent.newEntities.clear();

}

void EntityList::AddNewUpdateEntities(Entity& ent,
	std::vector<std::unique_ptr<Entity>>& newEntities)
{
	for (auto& item : ent.newEntities)
	{
		item->eid = entityID++;
		SetupEntityComponent(*item);
		item->InitializeComponents();
		AddNewUpdateEntities(*item, newEntities);
	}
	newEntities.insert(newEntities.end(),
		std::make_move_iterator(ent.newEntities.begin()),
		std::make_move_iterator(ent.newEntities.end()));
	ent.newEntities.clear();

}

void EntityList::UpdateEntities(float dt)
{
	auto iter = entityList.begin();
	auto lastItem = entityList.end();
	unsigned deleteCount = 0;
	std::vector<std::unique_ptr<Entity>> newEntities;
	while (iter != lastItem)
	{
		auto& ent = *iter;
		// Update the entity
		ent->UpdateComponents(dt);
		AddNewUpdateEntities(*ent, newEntities);

		//handle entity events
		if (ent->entityEvents.size())
		{
			ent->entityEvents[0]->Update(dt);
			if (ent->entityEvents[0]->EventEnded())
			{
				ent->entityEvents.erase(ent->entityEvents.begin());
				if (ent->entityEvents.size())
					ent->entityEvents[0]->Activate();
			}
		}

		// If the destroy flag of the entity is on, check that there's no link to the entity
		if (ent->destroyFlag)
		{
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::ENTITYREMOVED, "", 0, "", ent.get() });
			// Check if camera is focused on the deleted entity
			auto transComp = ent->GetComponent<TransformC>();
			if (transComp)
				transComp->localPosition.x = 999999999999999999999999.f;
			--lastItem;
			// Move the entity to the back of list
			std::swap(ent, *lastItem);
			// Increment the delete count
			++deleteCount;
		}
		else
			++iter;
	}
	// Destroy the entities based on the number of destroy flags captured
	while (deleteCount--)
	{
		entityList.back()->RemoveChildren();
		entityList.back()->RemoveParent();
		RemoveEntityComponents(*entityList.back());
		entityList.pop_back();
	}
	// Add all new entities
	entityList.insert(entityList.end(), std::make_move_iterator(newEntities.begin()), std::make_move_iterator(newEntities.end()));
	Render();
}

bool EntityList::RemoveEntity(Entity& ent)
{
	auto iter = std::find_if(entityList.begin(), entityList.end(),
		[&ent](const std::unique_ptr<Entity> &item)
	{
		return ent == *item;
	});

	if (iter == entityList.end())
		return false;

	entityList.erase(iter);
	return false;
}

Entity* EntityList::CreateEntity(const std::string& str)
{
	entityList.emplace_back(std::make_unique<Entity>());
	entityList.back()->SetEntityList(*this);
	entityList.back()->eid = entityID++;
  auto& ent = *entityList.back();
	std::vector<std::unique_ptr<Entity>> newEntities;
	if (str.size())
	{
		entityList.back()->LoadPrefab(str);
		entityList.back()->UpdateComponentData();
		entityList.back()->UpdateRequiredComponents();
		SetupEntityComponent(*entityList.back());
	}
	AddNewEntities(*entityList.back(), newEntities);
	entityList.insert(entityList.end(), std::make_move_iterator(newEntities.begin()), std::make_move_iterator(newEntities.end()));
	return &ent;
}


Entity* EntityList::CreateEntity(const Serializer& serial)
{
	auto root = serial.FirstChild();
	if (root == nullptr)
	{
		TOOLS::CreateErrorMessageBox("Invalid xml found.");
		CONSOLE_ERROR("Invalid xml found.");
		return nullptr;
	}
	auto elem = root->FirstChildElement("Prefab");
	if (!elem)
		return nullptr;
	entityList.emplace_back(std::make_unique<Entity>());
	entityList.back()->SetEntityList(*this);
	entityList.back()->eid = entityID++;
	entityList.back()->LoadPrefab(*elem);
	std::vector<std::unique_ptr<Entity>> newEntities;
	entityList.back()->UpdateComponentData();
	entityList.back()->UpdateRequiredComponents();
	SetupEntityComponent(*entityList.back());
	AddNewEntities(*entityList.back(), newEntities);
	entityList.insert(entityList.end(), std::make_move_iterator(newEntities.begin()), std::make_move_iterator(newEntities.end()));
	return entityList.back().get();
}

Entity* EntityList::GetElementInList(COMPONENTTYPE type)
{
	auto iter = entityList.begin();
	while (iter != entityList.end())
	{
		auto* comp = (*iter)->GetComponent(type);
		if (comp)
			return (*iter).get();
		++iter;
	}
	return nullptr;
}

namespace
{
	Vector2 GetPickingMults()
	{
		float xMult = 1.f;
		static auto oldWinRatio = 800.f / 1422.f;
		Vector2 newWindowSize = WINDOW_S.GetScreenSize();
		xMult = newWindowSize.x / newWindowSize.y * oldWinRatio;
		static auto  yMult = oldWinRatio;
		return Vector2(xMult, yMult);
	}
}


Entity* EntityList::PickEntity(const Vector2& clickPos, Camera& cam)
{
  cam.CheckRecalculate();
  float xMult = GetPickingMults().x;
  float yMult = GetPickingMults().y;
  Vector3 front = cam.GetFront();
  Vector3 right = cam.GetRight();
  Vector3 up = right.Cross(front);
  auto clickRay = front + clickPos.x * right * xMult + clickPos.y * up * yMult;
  return PHYSICS_S.PhysicsRayCastModel(cam.GetPosition(), clickRay);
}

std::vector<Entity*> EntityList::PickGroupEntity(const Vector2& clickPos, const Vector2& endPos, Camera& cam)
{
  cam.CheckRecalculate();
  float xMult = GetPickingMults().x;
  float yMult = GetPickingMults().y;
  Vector3 front = cam.GetFront();
  Vector3 right = cam.GetRight();
  Vector3 up = right.Cross(front);
  Vector3 pos = cam.GetPosition();
  auto leftFront = front + clickPos.x * right * xMult + clickPos.y * up * yMult;
  auto rightBack = front + endPos.x * right * xMult + endPos.y * up * yMult;

  auto rightFront = front + endPos.x * right * xMult + clickPos.y * up * yMult;
  auto leftBack = front + clickPos.x * right * xMult + endPos.y * up * yMult;

  auto rightFrus = ((rightBack + rightFront)/2.0f).Normalize();
  auto leftFrus = ((leftBack + leftFront) / 2.0f).Normalize();
  auto frontFrus = ((leftFront + rightFront) / 2.0f).Normalize();
  auto backFrus = ((rightBack + leftBack) / 2.0f).Normalize();


  if (right * rightFrus < right * leftFrus)
    right = -right;
  if (up * frontFrus < up * backFrus)
    up = -up;

  /*
  PHYSICS_S.debugTestDraw[0] = rightFront * cam.GetNearDist() + cam.GetPosition();
  PHYSICS_S.debugTestDraw[1] = leftFront * cam.GetNearDist() + cam.GetPosition();
  PHYSICS_S.debugTestDraw[2] = leftBack * cam.GetNearDist() + cam.GetPosition();
  PHYSICS_S.debugTestDraw[3] = rightBack * cam.GetNearDist() + cam.GetPosition();
  */
  
  float tmpZPlaneFix{ cam.GetNearDist() + 1.0f };
  DebugShape& tmp = DEBUGDRAW_S.DrawQuad(rightFront * tmpZPlaneFix + cam.GetPosition(), leftFront* tmpZPlaneFix + cam.GetPosition(), leftBack* tmpZPlaneFix + cam.GetPosition(), rightBack* tmpZPlaneFix + cam.GetPosition());
  tmp.color.SetX(0.8f);
  tmp.color.SetY(0.8f);
  tmp.color.SetZ(0.8f);

  
  front = front.Normalize();
  std::vector<Vector4> planes;
  planes.emplace_back(Vector4{ front.x, front.y, front.z, (pos * front)});

  auto normal = (frontFrus.Cross(-up).Cross(frontFrus)).Normalize();
  planes.emplace_back(Vector4{ normal.x , normal.y, normal.z, (pos * normal) });
  normal = (backFrus.Cross(up).Cross(backFrus)).Normalize();
  planes.emplace_back(Vector4{ normal.x , normal.y, normal.z, (pos * normal) });
  normal = (rightFrus.Cross(-right).Cross(rightFrus)).Normalize();
  planes.emplace_back(Vector4{ normal.x , normal.y, normal.z, (pos * normal) });
  normal = (leftFrus.Cross(right).Cross(leftFrus)).Normalize();
  planes.emplace_back(Vector4{ normal.x , normal.y, normal.z, (pos * normal) });
  

  return PHYSICS_S.PhysicsGroupModel(planes);

}

Entity * EntityList::FindEntity(const std::string & identifierName)
{
	// Grab the vector out
	const auto& vec = GetComponents<IdentifierC>();
	// Compare the identifier's name
	auto func = [&identifierName](const Component* comp)
	{
		std::string lhs = identifierName;
		std::string rhs = static_cast<const IdentifierC*>(comp)->name;
		std::transform(lhs.begin(), lhs.end(), lhs.begin(), [](char C)
		{
			return static_cast<char>(std::tolower(C));
		});
		std::transform(rhs.begin(), rhs.end(), rhs.begin(), [](char C)
		{
			return static_cast<char>(std::tolower(C));
		});

		return lhs == rhs;
	};
	// Get the iteration (Should return a component* type)
	auto iter = std::find_if(vec.begin(), vec.end(), func);
	
	// Return the owner if its found, otherwise return nullptr
	return (iter == vec.end()) ? nullptr : (*iter)->owner;
}

Entity* EntityList::FindExternalEntity(const std::string & idName)
{
	//for (auto& item : gamespaces)
	//{
	//	auto& comps = item->entity_list.GetComponents<IdentifierC>();
	//	for (auto id : comps)
	//	{
	//		if (static_cast<IdentifierC*>(id)->name == idName)
	//			return id->owner;
	//	}
	//}
	//return nullptr;

	//GameScene* currScene = currGamespace->attached_scene;
	//const auto& allGS = currScene->GetAllGamespaces();
	//for (auto& elem : allGS)
	//{
	//	// Skip ourself
	//	if (elem.gamespace.get() == currGamespace)
	//		continue;

	//	// Grab all of the IdentifierC
	//	const auto& comps = elem.gamespace->entity_list.GetComponents<IdentifierC>();
	//	for (auto id : comps)
	//		if (static_cast<IdentifierC*>(id)->name == idName)
	//			return id->owner;
	//}
	auto& coregamespaces = COREENGINE_S.gamespaces;
	for (auto& item : coregamespaces)
	{
		auto& comps = item->GetComponents<IdentifierC>();
		for (auto id : comps)
		{
			if (static_cast<IdentifierC*>(id)->name == idName)
				return id->owner;
		}
	}
	return nullptr;
}

void EntityList::Render()
{
	for (auto& item : entityList)
	{
		item->Render();
	}
}

EntityList * EntityList::GrabEntityListFromGameSpace(const std::string & gamespaceName)
{
	// Retrieve the game scene
	if (!currGamespace)
		return nullptr;

#ifdef EDITOR
	Editor& edit = *EDITOR_S;
	// @todo Search through all the gamespaces from the editor and find the corresponding gamespace

	return nullptr;
#else
	GameScene* currScene = currGamespace->attached_scene;
	GameSpace* foundSpace = currScene->GrabSpace(gamespaceName);
	if (!foundSpace)
	{
		CONSOLE_ERROR("Wrong Game Space name passed in! Unable to find entity...");
		return nullptr;
	}

	return &foundSpace->entity_list;
#endif
}

void EntityList::SetupEntityComponent(Entity& ent)
{
	for (auto& item : ent.components)
		SetupEntityComponent(*item, ent);
}
void EntityList::SetupEntityComponent(Component& comp, Entity& ent)
{
	auto type = comp.GetType();
	//if (type == COMPONENTTYPE::SCRIPT)
		//static_cast<ScriptC&>(comp).SetLuaState(luaScript->lua);
	comp.entityList = this;
	comp.owner = &ent;
	componentMap[type].emplace_back(&comp);
}

void EntityList::RemoveEntityComponents(Entity& owner)
{
	for (auto& comp : owner.components)
	{
		auto& componentVec = componentMap[comp->GetType()];
		auto iter = componentVec.begin();
		while (iter != componentVec.end())
		{
			if ((*iter)->owner == &owner)
			{
				//if (editor)
				//	(*iter)->EditorDestroy();
				//else
					(*iter)->DestroyUpdate();
				componentVec.erase(iter);
				break;
			}
			++iter;
		}
	}
}
CameraC * EntityList::FindACameraComponent()
{
	const auto& all_cams = GetComponents<CameraC>();
	// Search for all of the camera and return the first best one
	for (auto& cam : all_cams)
	{
		/*
		Current Algorithm is to find the first camera that is not disabled
		*/
		if (!cam->disabled)
			return cam->owner->GetComponent<CameraC>();
	}

	return nullptr;
}

void EntityList::DestroyComponents()
{
	for (auto& item : entityList)
	{
		for (auto& comp : item->components)
		{
			comp->DestroyUpdate();
		}
	}
	for (auto& item : gamespaces)
	{
		item->entity_list.DestroyComponents();
	}
}

Entity& EntityList::LoadEntity(const SerializeElem& pElement)
{
	auto& ent = *CreateEntity();
	std::string prefabName;
	pElement.QueryText(prefabName);
	std::string name = "New Object";
	if (prefabName.size())
	{
		name = prefabName;
		unsigned i = 0;
		for (; i < name.size(); ++i)
			ent.prefabName[i] = name[i];
		ent.prefabName[i] = 0;
	}
	/*int parentID = 0;
	if (_pElement->QueryAttribute("Parent", parentID) == ELEMRESULT::SUCCESS)
		ent.SetParent(*entityList[parentID]);*/
	auto& cElements = pElement.GetChildren();
	for (auto& item : cElements)
	{
		std::string componentName = item->Name();
		if (componentName == "Children")
		{
			auto& childElements = item->GetChildren();
			for (auto& child : childElements)
			{
				auto& newChild = LoadEntity(*child);
				newChild.SetParent(ent);
			}
			continue;
		}
		unsigned i = 0;
		for (; i < ComponentDirectory::names.size(); ++i)
		{
			if (ComponentDirectory::names[i] == componentName)
				break;
		}
		if (i == ComponentDirectory::names.size())
			CONSOLE_ERROR("Component name(", componentName, ") does not exist in the library!");
		else
		{
			bool alreadyExists = false;
			auto type = static_cast<COMPONENTTYPE>(i);
			for (auto& _item : ent.components)
			{
				if (_item->GetType() == type)
				{
					alreadyExists = true;
					break;
				}
			}
			if (!alreadyExists)
			{
				ent.components.emplace_back(COMPONENTMAP[type]());
				auto& comp = *ent.components.back();
				comp.owner = &ent;
				comp.AddSerializeData();
				comp.ConvertSerializedData(*item, name);
				comp.SetType(type);
			}
		}
	}
	ent.UpdateRequiredComponents();
	return ent;
}



bool EntityList::LoadComponentData(SerializeElem* pElement)
{
	DestroyComponents();
	gamespaces.clear();
	componentMap.clear();
	entityList.clear();
	entityID = 0;
	auto& pChildren = pElement->GetChildren();
	for (auto& _pElement : pChildren)
	{
		LoadEntity(*_pElement);
	}
	AssignComponents();
	BroadcastSettings();
	GFX_S.SetupSceneData(gamespaces);
	std::vector<EntityList*> elists;
	elists.emplace_back(this);
	COREENGINE_S.SetupGamespaces(elists);
	return true;
}

bool EntityList::LoadComponentData(const std::string & componentData)
{
	Serializer serial;
	auto result = serial.LoadFile(componentData);
	if (result != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading file: ", (int)result);
		return false;
	}
	if (!LoadComponentData(serial))
		return false;
	if(componentData != TEMPORARY_DIRECTORY)
		CONSOLE_SYSTEM("Loaded scene: \"", componentData, "\"");
	return true;
}

bool EntityList::LoadComponentData(Serializer & serial)
{
	auto levelDataElem = serial.FirstChildElement("Settings");
	if (levelDataElem)
		settings.ConvertSerializedData(*levelDataElem);
	settings.UpdateSerializables();
	auto entityElem = serial.FirstChildElement("Entities");
	if (entityElem == nullptr)
	{
		CONSOLE_ERROR("Invalid xml found.");
		return false;
	}
	LoadComponentData(entityElem);
	return true;
}


void EntityList::BroadcastSettings()
{
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATESCENESETTINGS, &settings, sceneID });
}

void EntityList::UpdatePrefabs(const Entity & prefab)
{
	std::string prefabName = prefab.prefabName.data();
	for (auto& item : entityList)
	{
		std::string name = item->prefabName.data();
		if (name == prefabName)
		{
			if (item.get() == &prefab) continue;
			for (auto& comp : item->components)
			{
				auto otherComp = prefab.GetComponent(comp->c_type);
				if (otherComp)
				{
					comp->CopyUnmodifiedVariables(*otherComp);
					comp->UpdateSerializables();
				}
			}
		}

	}
}

void RecursiveAddNewEntities(EntityList& elist)
{
	std::vector<std::unique_ptr<Entity>> newEntities;
	for (auto& ent : elist.entityList)
		elist.AddNewUpdateEntities(*ent, newEntities);
	elist.entityList.insert(elist.entityList.end(), std::make_move_iterator(newEntities.begin()), std::make_move_iterator(newEntities.end()));
	for (auto& item : elist.gamespaces)
		RecursiveAddNewEntities(item->entity_list);
}

void EntityList::SetupGameplay()
{
	GFX_S.SetCamera(*GetActiveCamera()->GetCamera());
	SetupLuascripts();
	UpdateRequiredComponents();
	Awake();
	RecursiveAddNewEntities(*this);
	Initialize();
}

void EntityList::UpdateRequiredComponents()
{
	for (auto& item : entityList)
	{
		item->UpdateRequiredComponents();
	}
}

void EntityList::PreloadObjects()
{
	for (auto& item : entityList)
	{
		auto scriptC = item->GetComponent<ScriptC>();
		if (scriptC)
		{
			for (auto& varData : scriptC->variableData)
			{
				if (varData.second.first == LUATYPE::STRING)
				{
					std::string ss(std::get<std::string>(varData.second.second));
					auto ext = TOOLS::GetExtension(ss);
					if (ext == ".dds")
					{
						GFX_S.GetTexture(ss);
					}
					else if (ext == ".mdl")
					{
						GFX_S.GetSkeletalMesh(ss);
						GFX_S.GetStaticMesh(ss);
					}
					else if (ext == ".pfb")
					{
						Entity ent;
						ent.LoadPrefab(ss);
					}
				}
			}
		}
	}
}

void EntityList::LoadMultipleGamespaces()
{
	// Setup for more than one gamespaces in the scene
	gamespaces.resize(settings.gameSpaces.size());
	size_t size = gamespaces.size();

	for (unsigned i = 0; i != size; ++i)
	{
		gamespaces[i] = std::make_unique<GameSpace>(nullptr, settings.gameSpaces[i], i + 1);

		// EntityList setup
		gamespaces[i]->SubGamespaceInit(settings.gameSpaces[i]);
	}

	// Inform Graphics
	GFX_S.SetupSceneData(gamespaces);
	std::vector<EntityList*> elists;
	elists.emplace_back(this);
	for (auto& item : gamespaces)
		elists.emplace_back(&item->entity_list);
	COREENGINE_S.SetupGamespaces(elists);

}

bool EntityList::SendMessageTo(const Variant & msg, const std::string & gamespaceName, bool allSpaces)
{
	// Gamespace not attached, cannot send out game messages
	if (!currGamespace)
		return false;
	
	currGamespace->SendMessageToAnotherGamespace(msg, gamespaceName, allSpaces);
}

void EntityList::HandleMessages()
{
	for (auto& msg : gameSpaceMessages)
	{
		
	}

	// Clear all of the messages within the instance
	gameSpaceMessages.clear();
}

void EntityList::GrabMessageFromGS(const Variant& msg, GameSpace* sender)
{
	// Append to our list of gamespace messages
	gameSpaceMessages.emplace_back(sender, msg);
}


bool EntityList::TestScriptCompilation() const
{
	auto iter = componentMap.find(COMPONENTTYPE::SCRIPT);
	if (iter != componentMap.end())
	{
		for (auto item : iter->second)
		{
			auto& script = static_cast<ScriptC&>(*item);
			if (!script.TestCompilation())
			{
				TOOLS::CreateErrorMessageBox("Script ", script.scripts[0], " failed to compile!");
				return false;
			}
		}
	}
	return true;
}