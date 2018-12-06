#pragma once
#include "Math.h"
#include <queue>

namespace
{
	struct FooterMsgInfo
	{
		std::string msg;					// message to display in the footer
		bool displaying = false;			// determine if we should display the messages
		Vector2 offset;						// current position for transition
	};
}

class FooterWindow
{
	std::queue<FooterMsgInfo> messages;
	static constexpr float offset_per_frame = 0.1f;
	static constexpr float y_bar = 19.f;
	float old_imgui_font_scale;
	Vector2 size;

public:
	static constexpr char windowName[] = "Footer";
	bool display_window = true;
	void BeginFooterBar();
	void EndFooterBar();
	void Update();
	void AddMessage(const std::string& msg);
	inline Vector2 GetFooterSize() const			{ return size; }
};