#pragma once
#include "GuiWindow.h"

class ToolsWindow : public GuiWindow
{
	float width_trs;
public:

	static constexpr char windowName[] = "Tools";
	ToolsWindow(Editor& edit)
		: GuiWindow(edit, windowName), width_trs{ 0 }
	{ }
	void Initialize(Editor&) override;
	void Display(Editor&) override;
	void Update(Editor&) override;
};