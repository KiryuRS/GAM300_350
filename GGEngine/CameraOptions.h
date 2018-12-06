#pragma once
#include "GuiWindow.h"

class CameraOptions : public GuiWindow
{

public:
	static constexpr char windowName[] = "CameraOptions";
	CameraOptions(Editor& edit)
		: GuiWindow(edit, windowName)
	{ }
	void Display(Editor&) override;
	void Update(Editor&) override;
};