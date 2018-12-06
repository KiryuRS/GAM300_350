#include "stdafx.h"
#include "ConsoleApp.h"
ConsoleApp::ConsoleApp()
	: scrollToBottom{ false }, system_filter{ true }, warning_filter{ true },
	  error_filter{ true }, logging_filter{ true }, prev_imgui_text_count{ 0 },
	  assertion_filter{ true }
{ }

ConsoleApp::~ConsoleApp()
{
	ClearLog();
}

void ConsoleApp::AddLog(const char *str, ...)
{
	// C-Style vardiac argument
	char buf[1024];
	constexpr int SIZE = sizeof(buf) / sizeof(*buf);
	va_list args;
	va_start(args, str);
	vsnprintf(buf, SIZE, str, args);
	buf[SIZE - 1] = 0;
	va_end(args);
	// creates a duplicate and assign to our texts
	texts.push_back(_strdup(buf));
	scrollToBottom = true;
}

void ConsoleApp::ClearLog()
{
	for (int i = 0; i != texts.Size; ++i)
		free(texts[i]);
	texts.clear();
}

void ConsoleApp::Draw()
{
	ClearLog();
	static ImGuiTextFilter filter;

	// Filtering of logs
	ImGui::Checkbox("Logs", &logging_filter, true);
	ImGui::SameLine();
	ImGui::Checkbox("Warnings", &warning_filter, true);
	ImGui::SameLine();
	ImGui::Checkbox("Errors", &error_filter, true);
	ImGui::SameLine();
	ImGui::Checkbox("Systems", &system_filter, true);
	ImGui::SameLine();
	ImGui::Checkbox("Assertions", &assertion_filter, true);
	ImGui::SameLine();
	if (ImGui::Button("Scroll to Bottom"))
		scrollToBottom = true;

	// Display our filter
	ImGui::SameLine();
	ImGui::Text("Filter");
	ImGui::SameLine();
	filter.Draw("", 250.f);
	ImGui::SameLine();
	if (ImGui::Button("Clear"))
		filter.Clear();
	// Add a separator to have a clear cut
	ImGui::Separator();

	// Inner "window" display
	ImGui::BeginChild("ScrollingRegion", ImVec2{}, false, ImGuiWindowFlags_HorizontalScrollbar);
	// Display all of the logs into ImGui
	const std::vector<std::string>& logger = SYSTEMLOG_S.GetLog();
	unsigned imgui_texts_count = 0;

	for (const auto& elem : logger)
	{
		// Check for filtering
		if (!filter.PassFilter(elem.c_str()))
			continue;
		size_t sys_pos = elem.find("SYSTEM::");
		size_t err_pos = elem.find("ERROR::");
		size_t war_pos = elem.find("WARNING::");
		size_t ass_pos = elem.find("ASSERTION ERROR::");
		if (!system_filter && sys_pos != std::string::npos)
			continue;
		if (!error_filter && err_pos != std::string::npos)
			continue;
		if (!warning_filter && war_pos != std::string::npos)
			continue;
		if (!assertion_filter && ass_pos != std::string::npos)
			continue;
		if (!logging_filter &&
			(sys_pos == std::string::npos &&
			 err_pos == std::string::npos &&
			 war_pos == std::string::npos &&
			 ass_pos == std::string::npos))
			continue;

		// Display all the texts involved
		ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		ImVec4 col = col_default_text;
		if (err_pos != std::string::npos || ass_pos != std::string::npos)
			col = ImColor{ 1.f, 0.3f, 0.3f, 1.f };
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(elem.c_str());
		ImGui::PopStyleColor();
		++imgui_texts_count;
	}

	bool autoScroll = imgui_texts_count > prev_imgui_text_count;
	// Check for the scrolling
	if (autoScroll || scrollToBottom)
		ImGui::SetScrollHere();
	scrollToBottom = false;
	prev_imgui_text_count = imgui_texts_count;

	ImGui::EndChild();			// end inner "window" display
}

ToggleBool::ToggleBool()
	: disableShortcuts{ false },
	mouseOverWindow{ false }, mouseHeld{ false }, movingAround{ false },
	isCopy{ false }, movingMesh{ false }, imgui{ false },
	styleEditor{ false }, user_guide{ false }
{ }