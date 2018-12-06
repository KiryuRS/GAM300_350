#pragma once
#include "GuiWindow.h"

class Serializer;

class ObjectHierarchyWindow : public GuiWindow
{
	std::vector<bool> selectedObjects;
	std::vector<Serializer> copiedEntities;
public:

	static constexpr char windowName[] = "Object_Hierarchy";
	ObjectHierarchyWindow(Editor& edit):GuiWindow(edit, windowName)
	{}
	void Display(Editor&) override;
	void Update(Editor&) override;
	void MoveChildToParent(int childIndex, int parentIndex, std::vector<std::unique_ptr<Entity>>& entityList);
	void AddChildrens(std::vector<std::string>* names, Entity* parent);
	void DeleteObjects(Editor& edit);
	void CopyPaste(Editor&);
};