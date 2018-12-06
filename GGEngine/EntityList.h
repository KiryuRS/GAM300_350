#pragma once
#include <memory>
#include <array>
#include "Vector.h"
#include "LevelSettings.h"
#include "Variant.h"

class Entity;
class LuaScript;
class CameraC;
class SerializeElem;
class Serializer;
struct GameSpace;
class Variant;

struct EntityListMessages
{
	GameSpace* sender;
	Variant var;

	EntityListMessages(GameSpace* _sender = nullptr, const Variant& _var = Variant{})
		: sender{ _sender }, var{ _var }
	{ }
};

class EntityList
{
	friend struct GameSpace;
	friend class EntityWrapper;

	std::map<COMPONENTTYPE, std::vector<Component*>> componentMap;
	void UpdateEntities(float);
	unsigned sceneID, entityID;
	std::vector<EntityListMessages> gameSpaceMessages;
	// This should only be used for EDITOR MODE

	// Gamespace message section
	bool SendMessageTo(const Variant& msg, const std::string& gamespaceName, bool allSpaces = false);
	void HandleMessages();
	void GrabMessageFromGS(const Variant& msg, GameSpace* sender);
	void AddNewEntities(Entity&, std::vector<std::unique_ptr<Entity>>&);
	Entity& LoadEntity(const SerializeElem& pElement);
	void SerializeEntity(const Entity&, SerializeElem& pElement) const;
public:
	std::vector<std::unique_ptr<GameSpace>> gamespaces;

	GameSpace* currGamespace = nullptr;
	LevelSettings settings;
	std::unique_ptr<LuaScript> luaScript;
	std::vector<std::unique_ptr<Entity>> entityList;
	Entity* player;

	EntityList();
	void Update(float dt);

	Entity* CreateEntity(const std::string& ="");
	Entity* CreateEntity(const Serializer&);
	bool RemoveEntity(Entity&);
	Entity* GetElementInList(COMPONENTTYPE);
	Entity* PickEntity(const Vector2&, Camera& cam);
	std::vector<Entity*> PickGroupEntity(const Vector2&, const Vector2&, Camera& cam);
	// Uses the Identifier Component class to search for the Entity
	Entity* FindEntity(const std::string& identifierName);
	// Grab the entity list from another gamespace
	EntityList* GrabEntityListFromGameSpace(const std::string& gamespaceName);

	void SetupEntityComponent(Entity&);
	void SetupEntityComponent(Component&, Entity&);
	void Initialize();
	void SaveComponentData(Serializer& pRoot) const;
	bool LoadComponentData(SerializeElem* pElement);
	bool LoadComponentData(const std::string&);
	bool LoadComponentData(Serializer& serial);
	void AddNewUpdateEntities(Entity& ent,
		std::vector<std::unique_ptr<Entity>>& newEntities);

	template<typename T>
	inline std::vector<Component*>& GetComponents()
	{
		return componentMap[T::GetType()];
	}

	void Clear();
	unsigned GetChildIndex(Entity*) const;
	void EditorUpdate(float);
	void EditorSetup();
	void AssignComponents();
	CameraC* GetActiveCamera();
	CameraC* FindACameraComponent();

	std::vector<Entity*> GetPrefabName(const std::string&);
	void SetupLuascripts();
	void Awake();
	void RemoveComponent(Component* comp);
	void RemoveEntityComponents(Entity&);
	void DestroyComponents();
	void BroadcastSettings();
	void UpdatePrefabs(const Entity& prefab);
	void SetupGameplay();
	void PreloadObjects();
	void LoadMultipleGamespaces();
	void UpdateRequiredComponents();
	Entity *FindExternalEntity(const std::string & idName);
	inline void AssignID(unsigned num) { sceneID = num; }
	inline unsigned GetID() const { return sceneID; }
	void Render();
	bool TestScriptCompilation() const;
};