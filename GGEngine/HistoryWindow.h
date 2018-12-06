#pragma once
#include "GuiWindow.h"

class HistoryWindow :public GuiWindow
{
	static constexpr unsigned historyNum = 20;
public:
	static constexpr char windowName[] = "History";
	HistoryWindow(Editor& edit): GuiWindow(edit, windowName)
	{}
	void Display(Editor&) override;
	void Update(Editor&) override;
};