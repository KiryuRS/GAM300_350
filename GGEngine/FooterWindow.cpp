#include "stdafx.h"
#include "FooterWindow.h"
#include "Common.h"
#include "CoreEngine.h"
#include "FrameRate.h"
#include "imgui.h"

void FooterWindow::BeginFooterBar()
{
	if (display_window)
	{
		const Vector2& win_size = COREENGINE_S.GetWindow().GetScreenSize();
		// Update our window_size
		size.x = win_size.x;
		size.y = y_bar;
		ImGui::SetNextWindowPos(ImVec2{ 0, win_size.y - size.y });
		ImGui::SetNextWindowSize(ImVec2{ size.x, size.y });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 0,0 });
		// Get the imgui font
		ImFont *font = ImGui::GetFont();
		old_imgui_font_scale = font->FontSize;
		font->FontSize = 0.2f;
		ImGui::PushFont(font);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
		ImGui::Begin("##FooterBar", &display_window, window_flags);
	}
}

void FooterWindow::EndFooterBar()
{
	if (display_window)
	{
		ImGui::End();
		ImGui::GetFont()->FontSize = old_imgui_font_scale;
		ImGui::PopFont();
		ImGui::PopStyleVar(2);
	}
}

void FooterWindow::Update()
{
	// For every frame, we would want to transit our message from right to left
	if (messages.size())
	{
		// Get the top element
		auto& top_elem = messages.front();
		ImGui::Text("");
		// ImGui::SameLine(top_elem.offset.x); 
		// ImGui::TextDisabled(top_elem.msg.c_str());
		// Check if our position is already off-screen
		if (top_elem.offset.x + top_elem.msg.length() < 0)
			// Pop the information out
			messages.pop();
		else
			top_elem.offset.x -= offset_per_frame * DELTATIME;
	}
}

void FooterWindow::AddMessage(const std::string & msg)
{
	// Grab the current footer's max window size
	const Vector2& win_size = COREENGINE_S.GetWindow().GetScreenSize();
	FooterMsgInfo info{};
	info.msg = msg;
	// Append to our messages so that it moves from right to left
	info.offset.x = win_size.x;
	info.offset.y = 0;
	// Push back the information
	messages.push(info);
}
