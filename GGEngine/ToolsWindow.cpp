#include "stdafx.h"
#include "ToolsWindow.h"

void ToolsWindow::Initialize(Editor &edit)
{
	auto& all_gui_windows = edit.GetAllGuiWindows();
	auto iter = std::find_if(all_gui_windows.begin(), all_gui_windows.end(),
		[](const auto& elem)
	{
		return elem->displayName == "Window_Modifiable";
	});
	if (iter != all_gui_windows.end())
		width_trs = iter->get()->position.x;
}

void ToolsWindow::Display(Editor & edit)
{
	// Name editing
	std::string playStr{ "PLAY" };
	std::string pauseStr{ "PAUSE" };
	const std::string skipStr{ "NEXT " + std::to_string(edit.frame_skips) + (edit.frame_skips == 1 ? " FRAME" : " FRAMES") };
	if (edit.playing) playStr = "STOP";
	if (edit.paused) pauseStr = "UNPAUSE";

	// ImGuizmos (Only draw if there's a selected entity)
	if (edit.selectedEntities.size())
	{
		float alignment_x = width_trs * COREENGINE_S.GetWindow().GetScreenWidth();
		ImGui::SameLine(alignment_x);
		if (ImGui::RadioButton("Translate", edit.current_operation == ImGuizmo::TRANSLATE) || edit.editor_input.CheckCombination(KEY_LALT, KEY_T))
			edit.current_operation = ImGuizmo::TRANSLATE;
		ImGui::SameLine(0, 10.f);
		if (ImGui::RadioButton("Rotate", edit.current_operation == ImGuizmo::ROTATE) || edit.editor_input.CheckCombination(KEY_LALT, KEY_R))
			edit.current_operation = ImGuizmo::ROTATE;
		ImGui::SameLine(0, 10.f);
		if (ImGui::RadioButton("Scale", edit.current_operation == ImGuizmo::SCALE) || edit.editor_input.CheckCombination(KEY_LALT, KEY_E))
			edit.current_operation = ImGuizmo::SCALE;
	}

	// Displaying the buttons for playing / stopping / skipping frames
	const float y_size = 0.55f;
	const float x_size = 3.f;
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.45f);
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 label_size = ImGui::CalcTextSize(playStr.c_str(), NULL, true);
	float offset_x = label_size.x + style.FramePadding.x * x_size;
	if (ImGui::Button(playStr.c_str(), ImVec2{ offset_x , ImGui::GetWindowSize().y * y_size }))
	{
		BROADCAST_MESSAGE(EditorMessage{EDITORMSG::PLAYBTN});
	}
	if (edit.playing)
	{
		ImGui::SameLine(0, 10.f);
		label_size = ImGui::CalcTextSize(pauseStr.c_str(), NULL, true);
		offset_x = label_size.x + style.FramePadding.x * x_size;
		if (ImGui::Button(pauseStr.c_str(), ImVec2{ offset_x, ImGui::GetWindowSize().y * y_size }))
		{
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::PAUSEBTN });
		}
		// Display the skip frame button only if its on paused state
		if (edit.paused)
		{
			ImGui::SameLine(0, 10.f);
			label_size = ImGui::CalcTextSize(skipStr.c_str(), NULL, true);
			offset_x = label_size.x + style.FramePadding.x * x_size;
			if (ImGui::Button(skipStr.c_str(), ImVec2{ offset_x, ImGui::GetWindowSize().y * y_size }))
			{
				BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SKIPFRAME });
			}
		}
	}
}

void ToolsWindow::Update(Editor &edit)
{
	if (INPUT_S.GetReleaseKey(KEY_F5))
		BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::PLAYBTN });
	if (edit.playing && INPUT_S.GetReleaseKey(KEY_F6))
		BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::PAUSEBTN });
}
