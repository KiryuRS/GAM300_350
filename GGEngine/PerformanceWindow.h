#pragma once
#include "GuiWindow.h"

#define MAX_INTERVAL			1.5f
#define MAX_FPS_COUNTER			60

class PerformanceWindow :public GuiWindow
{
	std::vector<float> sys_time;
	std::vector<std::string> sys_names;
	Timer timer;
	// Displaying the over-time FPS
	float fps_overtime[MAX_FPS_COUNTER];
	int fps_overtime_counter;

public:
	static constexpr char windowName[] = "Performance";
	PerformanceWindow(Editor& entityList);
	void Display(Editor&) override;
	void Update(Editor&) override;
};