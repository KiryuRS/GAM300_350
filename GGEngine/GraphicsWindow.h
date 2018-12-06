#pragma once
#include "GuiWindow.h"

class GraphicsWindow : public GuiWindow
{
public:
	static constexpr char windowName[] = "Graphics_Window";
	GraphicsWindow(Editor& edit):GuiWindow(edit, windowName)
	{

	}
	void Display(Editor&) override;
};