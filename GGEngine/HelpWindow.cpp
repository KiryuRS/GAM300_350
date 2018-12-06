#include "stdafx.h"

void HelpWindow::Display(Editor&)
{
	using TIEDUP = std::pair<std::string, std::string>;
	static const std::vector<TIEDUP> arr
	{
		{ "F1"							, "Toggle Editor Windows" },
		{ "F2"							, "Toggle Lock & Hide Mouse" },
		{ "W A S D Q E"					, "Free camera movement" },
		{ "Mouse Scroll"				, "Zoom in/out" },
		{ "[HOLD] Right Click"			, "Camera Rotation" },
		{ "[HOLD] Middle Mouse"			, "Pan Camera in Y-Axis" },
		{ "F"							, "Pan to selected object's location" },
		{ "CTRL-E"						, "Create new Entity"},
		{ "Del"							, "Delete a selected Object" },
		{ "F5"							, "Play / Stop Scene" },
		{ "F6"							, "Pause / Unpause Scene" },
		{ "CTRL-S"						, "Save Current Scene"},
		{ "CTRL-N"						, "New Scene"}
	};

	ImGui::Columns(2);
	ImGui::Separator();
	ImGui::Text("Controls"); ImGui::NextColumn();
	ImGui::Text("Description"); ImGui::NextColumn();
	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::BeginChild("Help Section", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& texts : arr)
	{
		auto&[controls, description] = texts;
		ImGui::Columns(2);
		ImGui::Separator();
		ImGui::Text(controls.c_str());
		ImGui::NextColumn();
		ImGui::TextWrapped(description.c_str());
		ImGui::Columns(1);
	}
	ImGui::Separator();
	ImGui::EndChild();
}