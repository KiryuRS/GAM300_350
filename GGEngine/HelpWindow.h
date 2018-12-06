#pragma once
#include "GuiWindow.h"

class HelpWindow : public GuiWindow
{
public:
	static constexpr char windowName[] = "Help";
	HelpWindow(Editor& edit) : GuiWindow(edit, windowName)
	{}
	void Display(Editor&) override;
};