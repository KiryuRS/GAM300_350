#include "stdafx.h"
#include "GraphicsWindow.h"

void GraphicsWindow::Display(Editor &)
{
	// Display through an inner child
	ImGui::BeginChild("Graphics_Inner", ImVec2{ 0,0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
	GFX_S.DisplayWindow();
	ImGui::EndChild();
}
