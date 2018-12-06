#pragma once
#include "Serializable.h"

class Editor;
class EntityList;

class GuiWindow : public Serializable
{
public:
	bool toggleDisplay;
	std::string windowName;
	std::string displayName;
	GuiWindow(Editor&, const std::string& name);
	virtual void Initialize(Editor&) {}
	virtual void Update(Editor&) {}
	virtual void Display(Editor&) {}
};