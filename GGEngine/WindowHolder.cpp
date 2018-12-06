#include "stdafx.h"

void GeneralWindowHolder::Initialize()
{
	UpdateResolution();
}

void GeneralWindowHolder::DisplayWindow()
{
	std::vector<GuiWindow*> displayWindows;
	for (auto& win : windows)
	{
		if (win->toggleDisplay)
			displayWindows.emplace_back(win);
	}

	if (displayWindows.size())
	{
		bool dummy = true;

		const Vector2& win_size = COREENGINE_S.GetWindow().GetScreenSize();
		// Only set the position when the EditorImGuiFlag_None is not set
		if (!(editor_flags & EditorImGuiFlags_Free))
		{
			ImGui::SetNextWindowPos(ImVec2{ win_size.x * position.x, win_size.y * position.y });
			ImGui::SetNextWindowSize(ImVec2{ win_size.x * size.x, win_size.y * size.y });
		}

		if (editor_flags & EditorImGuiFlags_TransparentWindow)
			ImGui::SetNextWindowBgAlpha(0);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin(displayName.c_str(), &dummy, flags);
		// Do not do docking for tools window
		if (displayName.find("Tools") == std::string::npos)
		{
			//ImGui::BeginDockspace();
			Display(displayWindows);
			//ImGui::EndDockspace();
		}
		else
			Display(displayWindows);
		ImGui::End();

		ImGui::PopStyleVar();
	}
}

void GeneralWindowHolder::UpdateResolution()
{
	// Check if the free flag is toggled on (we do not have to deal with all the positioning)
	if (editor_flags & EditorImGuiFlags_Free)
		return;

	const Vector2& win_size = COREENGINE_S.GetWindow().GetScreenSize();
	// Depending on the flags and references passed in, we will determine the position of the windows
	Vector2 final_position{};
	Vector2 final_size{ win_size.x * size.x, win_size.y * size.y };
	// Default flag toggles
	flags |= (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	// Only run to this code when our window resizes after the first setup
	if ((previous_size != Vector2{}))
	{
		if (editor_flags & EditorImGuiFlags_NoYRescale)
		{
			// Retain the ratio for y-axis
			float y_diff = final_size.y - previous_size.y;
			size.y -= (y_diff / win_size.y);
			final_size.y = win_size.y * size.y;
		}
		if (editor_flags & EditorImGuiFlags_NoXRescale)
		{
			// Retain the ratio for x-axis
			float x_diff = final_size.x - previous_size.x;
			size.x -= (x_diff / win_size.x);
			final_size.x = win_size.x * size.x;
		}
	}
	if (editor_flags & EditorImGuiFlags_PanLeft)
		final_position.x = 0.f;
	else if (editor_flags & EditorImGuiFlags_PanRight)
		final_position.x = win_size.x - final_size.x;
	if (editor_flags & EditorImGuiFlags_PanBottom)
		final_position.y = win_size.y - final_size.y;
	else if (editor_flags & EditorImGuiFlags_PanTop)
		final_position.y = bar_size;

	if (reference && (editor_flags & EditorImGuiFlags_PanLeftReference))
		final_position.x = win_size.x * reference->position.x - win_size.x * size.x;
	else if (reference && (editor_flags & EditorImGuiFlags_PanRightReference))
		final_position.x = win_size.x * reference->position.x + win_size.x * reference->size.x;
	else if (reference && (editor_flags & EditorImGuiFlags_PanBelowReference))
		final_position.y = win_size.y * reference->position.y + win_size.y * reference->size.y;
	else if (reference && (editor_flags & EditorImGuiFlags_PanAboveReference))
		final_position.y = win_size.y * reference->position.y - win_size.y * size.y;

	if (reference && (editor_flags & EditorImGuiFlags_MatchReferenceYPos))
		final_position.y = win_size.y * reference->position.y;
	if (reference && (editor_flags & EditorImGuiFlags_MatchReferenceXPos))
		final_position.y = win_size.x * reference->position.x;

	if (editor_flags & EditorImGuiFlags_PanAutoSize)
	{
		// Calculate how much should we resize based on the remaining width to the end
		final_size.y = win_size.y - final_position.y;
		size.y = final_size.y / win_size.y;
	}

	// Save the position for the next iteration
	position.x = final_position.x / win_size.x;
	position.y = final_position.y / win_size.y;
	previous_size = final_size;
}
