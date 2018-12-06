#pragma once
#include "GuiWindow.h"
#include "ConsoleApp.h"

class ConsoleWindow :public GuiWindow
{
	ConsoleApp console;
public:
	static constexpr char windowName[] = "Console";
	ConsoleWindow(Editor& edit) :GuiWindow(edit, windowName)
	{}
	void Display(Editor&) override;
};