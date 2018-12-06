#pragma once
#include <vector>
#include <memory>
#include <map>
#include <sol.hpp>

class EntityList;
class Component;
enum class COMPONENTTYPE;
class EntityWrapper;

class ComponentWrapper final
{
public:
	Component * comp;
	ComponentWrapper(Component* sc) : comp(sc)
	{

	}
	bool Exists() const
	{
		return comp != nullptr;
	}
};

class Entity final
{
	std::shared_ptr<char> proxy;
public:
	bool disabled = false;
	unsigned eid;
	std::array<char, 30> prefabName;
	std::vector<std::unique_ptr<Entity>> newEntities;
	Entity* parent;
	std::vector<Entity*> children;
	// Determine if the entity should be destroyed
	bool destroyFlag;
	std::vector<std::unique_ptr<Component>> components;
	EntityList* entityList;
	std::vector<EventList*> previousEvents;
	EventList entityEvents;
	// For imgui-gizmos
	bool onGizmos;

	// Functions
	Entity();
	void SetEntityList(EntityList&);
	void InitializeComponents();
	void Awake();
	std::unique_ptr<Entity> Clone();
	void DestroyThis();
	Entity& CreateEntity(std::unique_ptr<Entity>&& ent) { newEntities.push_back(std::move(ent)); return *newEntities.back(); }
	void UpdateComponents(float dt = 1.f / 60.f);
	//Updates optional components as well!
	void UpdateRequiredComponents();
	Entity& CreateEntity();
	bool SetParent(Entity&);
	void RemoveParent();
	void RemoveChildren();
	void UpdateComponentData();
	bool LoadPrefab(const std::string& str);
	bool LoadPrefab(const SerializeElem& elem);
	void ActivateEvents(EventList&);
	void CancelEvents();
	bool EventsPlaying() const;
	void RemoveComponent(COMPONENTTYPE);
	void Render();
  void CollisionEnter(Entity&);
  void CollisionExit(Entity&);
  void CollisionStay(Entity&);
  void DisableChildren();
  void EnableChildren();
  inline std::weak_ptr<char> GetProxy()
  {
	  if (!proxy)
	  {
		  proxy = std::make_shared<char>();
	  }
	  return proxy;
	}

	bool operator==(const Entity&);

	//Template monsters
	template<typename T>
	T* AddComponent()
	{
		return static_cast<T*>(AddComponent(T::GetType()));
	}

	//check if required component exists
	Component* AddComponent(COMPONENTTYPE comp, bool checkRequired = true);
	template<typename T>
	const T* GetComponent() const
	{
		for (auto& item : components)
		{
			// [TEMP]
			if (!item)
				continue;

			if (item->GetType() == T::GetType())
				return static_cast<const T*>(item.get());
		}
		return nullptr;
	}

	template<typename T>
	T* GetComponent()
	{
		for (auto& item : components)
		{
			// [TEMP]
			if (!item)
				continue;

			if (item->GetType() == T::GetType())
				return static_cast<T*>(item.get());
		}
		return nullptr;
	}
	Component* GetComponent(COMPONENTTYPE comp);
	const Component* GetComponent(COMPONENTTYPE comp) const;
	void EditorSetup();
	Serializer SerializeEntity();
	void SerializeEntity(SerializeElem&, Entity&);
};

bool operator==(const Entity&, const Entity&);
bool operator!=(const Entity&, const Entity&);