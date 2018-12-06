#include "stdafx.h"				// must be the first thing to include

#define PItoDegree 180.0f/3.1415926535f
#define DegreeToPI 3.1415926535f/180.0f

Entity::Entity()
	: destroyFlag{ false }, entityList{ nullptr },
	prefabName{ {0} }, parent{ nullptr },
	eid{ std::numeric_limits<unsigned>::max() }, onGizmos{ false }
{}

void Entity::UpdateComponentData()
{
	for (auto& item : components)
		item->UpdateComponentData();
}

bool Entity::LoadPrefab(const SerializeElem& elem)
{
	if (entityList)
	{
		for (auto& item : components)
			entityList->RemoveComponent(item.get());
	}
	components.clear();
	std::string pName;
	elem.QueryAttribute("Name", pName);
	auto& ch = elem.GetChildren();
	for (auto& cElement : ch)
	{
		std::string componentName = cElement->Name();
		if (componentName == "Children")
		{
			auto& allChildren = cElement->GetChildren();
			for (const auto& item : allChildren)
			{
				auto& ent = CreateEntity();
				ent.LoadPrefab(*item);
				ent.SetParent(*this);
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
			auto type = static_cast<COMPONENTTYPE>(i);
			components.emplace_back(COMPONENTMAP[type]());
			components.back()->owner = this;
			components.back()->AddSerializeData();
			components.back()->ConvertSerializedData(*cElement, pName);
			components.back()->SetType(type);
		}
	}
	strcpy_s(prefabName.data(), prefabName.size(), pName.c_str());
	
	auto comp = GetComponent<ScriptC>();
	if (comp)
	{
		comp->UpdateLuaVariables();
	}
	if (entityList)
	{
		Awake();
		UpdateRequiredComponents();
	}
	return true;
}


bool Entity::LoadPrefab(const std::string & str)
{
	unsigned stringSize = (unsigned)str.size();
	if (stringSize < 5)
		return false;
	unsigned index = stringSize - 4;
	std::string extension = &str.c_str()[index];
	if (extension != ".pfb")
		return false;

	Serializer serial;
	auto error = serial.LoadFile(str);
	if (error != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading save data: ", (int)error);
		return false;
	}
	auto root = serial.FirstChild();
	if (root == nullptr)
	{
		TOOLS::CreateErrorMessageBox("Invalid xml found.");
		CONSOLE_ERROR("Invalid xml found.");
		return false;
	}
	auto elem = root->FirstChildElement("Prefab");
	if(elem)
		return LoadPrefab(*elem);
	return false;
}

void Entity::ActivateEvents(EventList & events)
{
	bool startEvent = true;
	if (entityEvents.size())
	{
		for (auto& item : previousEvents)
		{
			if (item == &events) return;
		}
		startEvent = false;
	}
	else
		previousEvents.clear();
	for (auto& item : events)
	{
		entityEvents.emplace_back(item->Clone());
		entityEvents.back()->owner = this;
	}
	if (events.size() && startEvent)
		entityEvents[0]->Activate();
	previousEvents.emplace_back(&events);
}

bool Entity::EventsPlaying()const
{
	return entityEvents.size() != 0;
}
void Entity::RemoveComponent(COMPONENTTYPE type)
{
	auto iter = components.begin();
	while (iter != components.end())
	{
		if ((*iter)->c_type == type)
		{
			entityList->RemoveComponent(iter->get());
			components.erase(iter);
			break;
		}
		++iter;
	}
}

void Entity::CollisionEnter(Entity& ent)
{
	if (ent.disabled) return;
	if (disabled) return;
  for (auto& item : components)
  {
    item->CollisionEnter(ent);
  }
}

void Entity::CollisionStay(Entity& ent)
{
	if (ent.disabled) return;
	if (disabled) return;
  for (auto& item : components)
  {
    item->CollisionStay(ent);
  }
}

void Entity::DisableChildren()
{
	for (auto item : children)
	{
		item->disabled = true;
		item->DisableChildren();
	}
}

void Entity::EnableChildren()
{
	for (auto item : children)
	{
		item->disabled = false;
		item->EnableChildren();
	}
}

void Entity::CollisionExit(Entity& ent)
{
	if (ent.disabled) return;
	if (disabled) return;
  for (auto& item : components)
  {
    item->CollisionExit(ent);
  }
}

bool Entity::operator==(const Entity & rhs)
{
	return rhs.eid == this->eid;
}
void Entity::CancelEvents()
{
	entityEvents.clear();
}

void Entity::SetEntityList(EntityList& elist)
{
	entityList = &elist;
}

void Entity::UpdateRequiredComponents()
{
	for (auto& item : components)
		item->UpdateRequiredComponents(components);
}

void Entity::InitializeComponents()
{
	for (auto& item : components)
	{
		item->Initialize();
		if (item->newEntities.size())
		{
			newEntities.insert(newEntities.end(), std::make_move_iterator(item->newEntities.begin()),
				std::make_move_iterator(item->newEntities.end()));
			item->newEntities.clear();
		}
	}
}

void Entity::Awake()
{
	for (auto& item : components)
	{
		if (!item->disabled)
			item->Awake();
		if (item->newEntities.size())
		{
			newEntities.insert(newEntities.end(), std::make_move_iterator(item->newEntities.begin()), std::make_move_iterator(item->newEntities.end()));
			item->newEntities.clear();
		}
	}
}

void Entity::UpdateComponents(float dt)
{
	if (disabled) return;
	for (auto& item : components)
	{
		if (!item->disabled)
			item->Update(dt);
		if (item->newEntities.size())
		{
			newEntities.insert(newEntities.end(), std::make_move_iterator(item->newEntities.begin()), std::make_move_iterator(item->newEntities.end()));
			item->newEntities.clear();
		}
	}
}

Entity& Entity::CreateEntity()
{
	newEntities.emplace_back(std::make_unique<Entity>());
	newEntities.back()->entityList = entityList;
	return *newEntities.back();
}
Component* Entity::AddComponent(COMPONENTTYPE type, bool checkRequired)
{
	auto newComponent = COMPONENTMAP[type]();
	newComponent->owner = this;
	newComponent->AddSerializeData();
	std::vector<COMPONENTTYPE> types;
	for (auto& reqs : newComponent->requiredComponents)
		if (reqs->required)
			types.push_back(reqs->GetType());
	for (auto& item : components)
	{
		auto iter = types.begin();
		while (iter != types.end())
		{
			if (item->GetType() == *iter)
			{
				types.erase(iter);
				break;
			}
			++iter;
		}
		if (item->GetType() == type)
		{
			TOOLS::CreateErrorMessageBox("Component already exists!");
			return nullptr;
		}
	}
	if (checkRequired && !types.empty())
	{
		std::string errorStr = "Required component type: ";
		for (auto& compName : types)
		{
			errorStr += ComponentDirectory::names[unsigned(compName)];
			errorStr += " ";
		}
		TOOLS::CreateErrorMessageBox(errorStr);
		return nullptr;
	}
	components.emplace_back(std::move(newComponent));
	components.back()->SetType(type);
	if(checkRequired)
		components.back()->UpdateRequiredComponents(components);
	components.back()->UpdateSerializables();
	//components.back()->EditorSetup();
	//InitType(*components.back());
	if (checkRequired)
	{
		for (auto& item : components)
			item->UpdateRequiredComponents(components);
	}
	/*MODEL_S.history.AddAction("Added ",
		ComponentDirectory::names[unsigned(type)],
		" to Object ", id);*/
	if(entityList)
		entityList->SetupEntityComponent(*components.back(), *this);
	return &*components.back();
}

bool Entity::SetParent(Entity & newParent)
{
	//first, recursively check if any of the parents' parents are yourself
	//to prevent infinite loops
	auto entParent = newParent.parent;
		//std::remove(parent->children.begin(), parent->children.end(), this,);
	while (entParent)
	{
		if (entParent == this)
			return false;
		entParent = entParent->parent;

	}
	//then we can proceed to set the new parent
  RemoveParent();
	parent = &newParent;
	parent->children.emplace_back(this);
  
  TransformC* selfTrans = GetComponent<TransformC>();
  TransformC* parentTrans = parent->GetComponent<TransformC>();

  if (selfTrans && parentTrans)
  {

	  Matrix4x4 localRotateM = Matrix4x4::Inverse(TransformC::GetParentRotationMatrix(parent->parent, *parentTrans->GetRotationMatrix())) * *selfTrans->GetRotationMatrix();


	  Matrix4x4 localScale = Matrix4x4::Inverse(TransformC::GetParentScaleMatrix(parent->parent, *parentTrans->GetScaleMatrix()));

	  Matrix4x4::GetRotationAngle(localRotateM, selfTrans->localRotation.x, selfTrans->localRotation.y, selfTrans->localRotation.z);


	  selfTrans->localRotation *= PItoDegree;
	  selfTrans->localScale = localScale * selfTrans->localScale;

	  Vector3 parentToChildPos = selfTrans->GetGlobalPosition() - parentTrans->GetGlobalPosition();

	  selfTrans->localPosition = localScale * Matrix4x4::Inverse(TransformC::GetParentRotationMatrix(parent->parent, *parentTrans->GetRotationMatrix()))  * parentToChildPos;

	  //selfTrans->UpdateLocal();
  }
	//parent->children.emplace_back(this);
	return true;
}
void Entity::RemoveParent()
{
	if (parent)
	{
		auto iter = std::remove(parent->children.begin(), parent->children.end(), this);
		if(iter != parent->children.end())
			parent->children.erase(iter);

    TransformC* selfTrans = GetComponent<TransformC>();
    TransformC* parentTrans = parent->GetComponent<TransformC>();

	  Matrix4x4 localRotate = TransformC::GetParentRotationMatrix(parent, *selfTrans->GetRotationMatrix());
	  Matrix4x4 localScale = TransformC::GetParentScaleMatrix(parent->parent, *parentTrans->GetScaleMatrix());

	  Matrix4x4::GetRotationAngle(localRotate, selfTrans->localRotation.x, selfTrans->localRotation.y, selfTrans->localRotation.z);


	  selfTrans->localRotation *= PItoDegree;

	  selfTrans->localScale = localScale * selfTrans->localScale;

    selfTrans->localPosition = selfTrans->GetGlobalPosition();
    
    //selfTrans->UpdateLocal();
	}
	parent = nullptr;
}
void Entity::RemoveChildren()
{
	while (children.size())
		children[0]->RemoveParent();
}
std::unique_ptr<Entity> Entity::Clone()
{
	auto returnEnt = std::make_unique<Entity>();
	for (auto&item : components)
	{
		if (item->GetType() != COMPONENTTYPE::TRANSFORM)
		{
			returnEnt->components.push_back(item->Clone());
			returnEnt->components.back()->owner = returnEnt.get();
		}
	}
	returnEnt->SetEntityList(*entityList);
	return std::move(returnEnt);
}

void Entity::DestroyThis()
{
	destroyFlag = true;
	for (auto& item : children)
		item->DestroyThis();
}

Component* Entity::GetComponent(COMPONENTTYPE comp)
{
	for (auto& item : components)
	{
		if (item->GetType() == comp)
			return item.get();
	}
	return nullptr;
}

const Component * Entity::GetComponent(COMPONENTTYPE comp) const
{
	for (auto& item : components)
	{
		if (item->GetType() == comp)
			return item.get();
	}
	return nullptr;
}

void Entity::EditorSetup()
{
	for (auto& item2 : components)
		item2->EditorSetup();
}

void Entity::SerializeEntity(SerializeElem&elem, Entity& ent)
{
	elem.SetAttribute("Name", &ent.prefabName[0]);
	for (auto& item : ent.components)
	{
		SerializeElem& cElement = elem.InsertChild(
			ComponentDirectory::names[unsigned(item->GetType())].data());
		item->Serialize(cElement);
		for (size_t i = 0; i < item->asterisks.size(); ++i)
			item->asterisks[i] = false;
	}
	auto& childrenElem = elem.InsertChild("Children");
	int childNum = 0;
	for (auto child : ent.children)
	{
		std::stringstream ss;
		ss << "Child" << childNum++;
		auto& childElem = childrenElem.InsertChild(ss.str());
		SerializeEntity(childElem, *child);
	}

}

Serializer Entity::SerializeEntity()
{
	Serializer Doc;
	SerializeElem &pRoot = Doc.NewElement("Root");
	SerializeElem & pElement = pRoot.InsertChild("Prefab");
	SerializeEntity(pElement, *this);
	return std::move(Doc);
}

void Entity::Render()
{
	if (disabled) return;
	for (auto& item : components)
		item->Render();
}

bool operator==(const Entity& e1, const Entity& e2)
{
	return &e1 == &e2;
}

bool operator!=(const Entity& e1, const Entity& e2)
{
	return !(e1 == e2);
}


