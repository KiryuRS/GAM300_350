#include "stdafx.h"
#include "CameraOptions.h"

void CameraOptions::Display(Editor &)
{
	const float x_size = 3.f;
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 label_size = ImGui::CalcTextSize("Left", NULL, true);
	float offset_x = label_size.x + style.FramePadding.x * x_size;

	ImGui::SameLine(offset_x);
	ImGui::Button("Top");

	ImGui::Button("Left");
	ImGui::SameLine(offset_x);
	ImGui::Button("Cen");
	ImGui::SameLine();
	ImGui::Button("Right");
	
	// Dummy Text to allow same line manipulation
	ImGui::TextDisabled("");
	ImGui::SameLine(offset_x);
	ImGui::Button("Btm");
}

void CameraOptions::Update(Editor &)
{
}
