#pragma once

class ConsoleApp final
{
	ImVector<char *>		texts;
	bool					scrollToBottom;
	bool					system_filter;
	bool					warning_filter;
	bool					error_filter;
	bool					logging_filter;
	bool					assertion_filter;
	unsigned				prev_imgui_text_count;
	unsigned				error_count;

public:
	ConsoleApp();
	~ConsoleApp();
	void AddLog(const char *str, ...);
	void ClearLog();
	void Draw();
};

struct ToggleBool final
{
	bool disableShortcuts, mouseOverWindow, mouseHeld;
	bool movingAround, isCopy, movingMesh;
	bool imgui, styleEditor, user_guide;

	// Default all to false
	ToggleBool();
};