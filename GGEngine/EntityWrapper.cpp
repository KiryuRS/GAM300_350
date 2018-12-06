#include "stdafx.h"

void EntityWrapper::CheckNull() const
{
	if (!heldEntity)
		throw std::runtime_error("Trying to access an invalid Entity!");
	if (!entityProxy.lock())
		throw std::runtime_error("The referenced entity has been destroyed!");
}

ComponentWrapper EntityWrapper::GetComponent(std::string str)
{
	CheckNull();
	for (unsigned i = 0; i < ComponentDirectory::names.size(); ++i)
	{
		if (ComponentDirectory::names[i] == str)
		{
			return heldEntity->GetComponent(static_cast<COMPONENTTYPE>(i));
		}
	}
	throw std::runtime_error{ "Could not find component " + str + "!\n" };
}

EntityWrapper EntityWrapper::GetChild(int index)
{
	CheckNull();
	if (heldEntity->children.size() <= index)
	{
		std::stringstream ss;
		ss << "Could not find child with index " << index << "!";
		throw std::runtime_error(ss.str());
	}

	return heldEntity->children[index];
}

int EntityWrapper::GetChildrenCount()
{
	CheckNull();
	return (int)heldEntity->children.size();
}

void EntityWrapper::AddChild(Entity * child)
{
	CheckNull();
	heldEntity->children.push_back(child);
	child->parent = heldEntity;
}

void EntityWrapper::ClearChildren()
{
	CheckNull();
	heldEntity->RemoveChildren();
}

EntityWrapper EntityWrapper::CreateEntity()
{
	CheckNull();
	return EntityWrapper(&heldEntity->CreateEntity());
}

EntityWrapper EntityWrapper::FindEntity(std::string name)
{
	CheckNull();
	auto foundEntity = heldEntity->entityList->FindEntity(name);
	if (!foundEntity)
	{
		std::stringstream ss;
		ss << "Could not find entity with Identifier " << name << "!";
		throw std::runtime_error(ss.str());
	}
	return EntityWrapper(foundEntity);
}

bool EntityWrapper::EntityExists(std::string name)
{
  CheckNull();
  auto foundEntity = heldEntity->entityList->FindEntity(name);
  if (!foundEntity)
    return false;
  return true;
}

ComponentWrapper EntityWrapper::AddComponent(std::string str)
{
	CheckNull();
	for (unsigned i = 0; i < ComponentDirectory::names.size(); ++i)
	{
		if (ComponentDirectory::names[i] == str)
		{
			return heldEntity->AddComponent(static_cast<COMPONENTTYPE>(i));
		}
	}
	std::stringstream ss;
	ss << "Could not find component type " << str << "!";
	throw std::runtime_error(ss.str());
	return nullptr;
}

void EntityWrapper::Destroy()
{
	CheckNull();
	heldEntity->DestroyThis();
	heldEntity = nullptr;
}

void EntityWrapper::SetParent(EntityWrapper EW)
{
	CheckNull();
	heldEntity->SetParent(*EW.heldEntity);
}

void EntityWrapper::RemoveParent()
{
	CheckNull();
	heldEntity->RemoveParent();
}

std::string EntityWrapper::GetName()
{
	CheckNull();
	return heldEntity->prefabName.data();
}

void EntityWrapper::SetName(std::string str)
{
	CheckNull();
	strcpy_s(heldEntity->prefabName.data(), heldEntity->prefabName.size(), str.c_str());
}

bool EntityWrapper::LoadPrefab(std::string str)
{
	CheckNull();
	std::string newStr = PREFAB_DIRECTORY + str + ".pfb";
	auto loaded = heldEntity->LoadPrefab(newStr);
	if (loaded)
	{
		heldEntity->Awake();
		heldEntity->InitializeComponents();
	}
	else
	{
		std::stringstream ss;
		ss << "Could not find prefab " << str << "!";
		throw std::runtime_error(ss.str());
	}
	return loaded;
}

void EntityWrapper::Disable()
{
	CheckNull();
	heldEntity->disabled = true;
	heldEntity->DisableChildren();
}

void EntityWrapper::Enable()
{
	CheckNull();
	heldEntity->disabled = false;
	heldEntity->EnableChildren();
}

bool EntityWrapper::isEnabled()
{
	CheckNull();
	return !heldEntity->disabled;
}

EntityWrapper EntityWrapper::FindExternalEntity(std::string entName)
{
	CheckNull();
	auto foundEntity = heldEntity->entityList->FindExternalEntity(entName);
	if (foundEntity) return foundEntity;

	throw std::runtime_error{ "Could not find entity " + entName + " inside the gamespaces!" };

#ifdef EDITOR
	/*
		In Editor mode, we would want to "hack" by getting the entityList from EDITOR_S. From there,
		we search through ALL of the gamespaces and find the corresponding entName and return the Entity
	*/

	Editor& editor = *EDITOR_S;
	const auto& allGamespaces = editor.entityList->gamespaces;
	for (const auto& elem : allGamespaces)
	{
		auto& allIdentifiers = elem->entity_list.GetComponents<IdentifierC>();
		for (auto& id : allIdentifiers)
			if (static_cast<IdentifierC*>(id)->name == entName)
				return id->owner;
	}

	// If there isnt any found, we search in the editor's main editorScene
	const GameSpacesInfo& editorGS = editor.editorScene[0];
	auto& allIdentifiers = editorGS.gamespace->entity_list.GetComponents<IdentifierC>();
	for (auto& id : allIdentifiers)
		if (static_cast<IdentifierC*>(id)->name == entName)
			return id->owner;

#else
	/*
		In the actual release, we will search through the gamescene and from the game scene to
		search ALL of the gamespaces to find that idName
	*/

	CheckNull();
	Entity* ent = heldEntity->entityList->FindExternalEntity(entName);
	if (!ent)
		throw std::runtime_error{ "Could not find entity " + entName + " inside the gamespaces!" };

	return ent;

#endif
}

bool EntityWrapper::DoesExternalEntityExist(std::string entName)
{
	CheckNull();
	// Similar concept to FindAllExternalEntity
	try
	{
		FindExternalEntity(entName);
		return true;
	}
	// Catch would be catching the runtime error
	catch (const std::runtime_error& e)
	{
		// Dummy catch
	}

	return false;
}

void EntityWrapper::SetSkybox(const std::string & direction, const std::string & texture)
{
	CheckNull();
	if (direction == "Left")
		heldEntity->entityList->settings.skyboxTextures[0] = texture;
	else if (direction == "Right")
		heldEntity->entityList->settings.skyboxTextures[1] = texture;
	else if (direction == "Down")
		heldEntity->entityList->settings.skyboxTextures[2] = texture;
	else if (direction == "Up")
		heldEntity->entityList->settings.skyboxTextures[3] = texture;
	else if (direction == "Back")
		heldEntity->entityList->settings.skyboxTextures[4] = texture;
	else if (direction == "Front")
		heldEntity->entityList->settings.skyboxTextures[5] = texture;
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATESCENESETTINGS, 
		&heldEntity->entityList->settings, heldEntity->entityList->GetID() });

}
void EntityWrapper::SetFog(bool enabled)
{
	CheckNull();
	heldEntity->entityList->settings.enableFog = enabled;
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATESCENESETTINGS,
		&heldEntity->entityList->settings, heldEntity->entityList->GetID() });

}
void EntityWrapper::SetFogColor(const Vector4& color)
{
	CheckNull();
	heldEntity->entityList->settings.fogColor = color;
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATESCENESETTINGS,
		&heldEntity->entityList->settings, heldEntity->entityList->GetID() });

}

EntityWrapper EntityWrapper::SearchPrefabName(const std::string & prefabName)
{
	CheckNull();
	// Get all entityList
	EntityList* eList = heldEntity->entityList;
	for (auto& elem : eList->entityList)
	{
		if (prefabName == elem->prefabName.data())
			return elem.get();
	}

	throw std::runtime_error("Could not find prefab name with: " + prefabName);
}
