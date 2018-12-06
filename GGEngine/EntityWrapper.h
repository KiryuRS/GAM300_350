#pragma once
#include "Entity.h"
#include "Component.h"

class EntityWrapper final
{
	std::weak_ptr<char> entityProxy;
public:
	
	Entity * heldEntity;
	EntityWrapper() :heldEntity(nullptr) {}
	EntityWrapper(Entity* ent) :heldEntity(ent) 
	{
		if (ent == nullptr)
		{
			throw std::runtime_error("Trying to create an invalid Entity!");
		}
		entityProxy = ent->GetProxy();
	}

	void CheckNull() const;
	ComponentWrapper GetComponent(std::string str);
	EntityWrapper GetChild(int index);
	int GetChildrenCount();
	void AddChild(Entity* child);
	Entity* GetEntity()				{ return heldEntity; }
	void ClearChildren();
	EntityWrapper CreateEntity();
	EntityWrapper FindEntity(std::string name);
  bool EntityExists(std::string name);
	ComponentWrapper AddComponent(std::string str);
	void Destroy();
	EntityWrapper GetParent()		{ return heldEntity->parent; }
	void SetParent(EntityWrapper EW);
	void RemoveParent();
	std::string GetName();
	void SetName(std::string str);
	bool LoadPrefab(std::string str);
	void Disable();
	void Enable();
	bool isEnabled();
	bool IsDisabled()				{ CheckNull(); return heldEntity->disabled; }
	bool CheckNullBool()			{ return heldEntity; }
	EntityWrapper FindExternalEntity(std::string entName);
	bool DoesExternalEntityExist(std::string entName);
	void SetSkybox(const std::string&direction, const std::string& texture);
	void SetFog(bool enabled);
	void SetFogColor(const Vector4& color);
	EntityWrapper SearchPrefabName(const std::string& prefabName);
};