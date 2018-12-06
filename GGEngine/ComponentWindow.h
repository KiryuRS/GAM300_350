#pragma once
#include "GuiWindow.h"
#include <map>

class Entity;

class ComponentWindow : public GuiWindow
{
	std::map<std::string, Entity> prefabs;
	std::unique_ptr<Component> currentComponent, copiedComponent;
	bool CheckRemoveComponent(COMPONENTTYPE type, const Entity& ent) const;
	void SavePrefab(Entity&);
	void PasteCopiedComponent(Entity&);
	void GizmoUpdate(Editor& edit);
	bool GizmosMove(Editor& edit, std::vector<Entity*>::iterator iter);
public:
	static constexpr char windowName[] = "Components";
	ComponentWindow(Editor& edit);
	void Display(Editor&) override;
	//generate the list of options to add new components
	void ComponentList(std::vector<Entity*>&);
	void Update(Editor&) override;
	//sets the dirty bit for all variables that are from prefabs
	void CheckEntityListForPrefabs(EntityList&);
};