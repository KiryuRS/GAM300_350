#include "stdafx.h"
#include "GuiWindow.h"

GuiWindow::GuiWindow(Editor& edit, const std::string& name) :
	toggleDisplay(true), windowName(name), displayName(name)
{
	std::stringstream str;
	str << name << "_Display";
	TOOLS::SearchAndReplace(displayName, "_", " ");
	edit.AddSerializable(windowName, toggleDisplay);
}
