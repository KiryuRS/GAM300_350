#include "stdafx.h"

PerformanceWindow::PerformanceWindow(Editor & entityList)
	: GuiWindow(entityList, windowName), timer{ MAX_INTERVAL, true }, fps_overtime{ },
	  fps_overtime_counter{ MAX_FPS_COUNTER - 1 }
{
	auto& all_sys = COREENGINE_S.all_sys;
	for (auto& elem : all_sys)
		sys_names.emplace_back(elem->GetName());
	sys_names.emplace_back("Pre-Updates");
#ifdef EDITOR
	sys_names.emplace_back("Debug Draw");
#endif
	sys_names.emplace_back("Idle");
}

void PerformanceWindow::Display(Editor&)
{
	ImGui::BeginChild("Debugging_Controls");

	// Display of Performance Graphs
	auto& fps = FRAMECTR_S;
	float fpsMS = fps.GetDeltaTime() * 1000.f;
	ImGui::TextUnformatted("Performance Graph:");
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.f);
		std::stringstream ss;
		ss << "Max value is " << fpsMS << ". Min value is 0";
		ImGui::TextUnformatted(ss.str().c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::PlotHistogram(sys_names, "", sys_time.data(), (int)sys_time.size(), 0, NULL, 0, fpsMS, ImVec2{ ImGui::GetWindowWidth(), 80 });

	// Display of FPS
	ImGui::TextUnformatted("FPS Overtime:");
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.f);
		ImGui::TextUnformatted("Leftmost displays the latest FPS. Max FPS is 59.9999. Lowest FPS is 0");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::PlotLines("", fps_overtime, MAX_FPS_COUNTER, 0, nullptr, 0, 60.f, ImVec2{ ImGui::GetWindowWidth(), 80 });
	ImGui::Separator();

	ImGui::EndChild();			// end first inner child
}

void PerformanceWindow::Update(Editor&)
{
	// Capturing the information
	timer.Update();
	if (timer.TimesUp())
	{
		float used_time = 0;
		sys_time.clear();
		const auto& all_sys = COREENGINE_S.all_sys;
		for (const auto& elem : all_sys)
		{
			used_time += elem->GetInterval();
			sys_time.emplace_back(elem->GetInterval());
		}
		// Import the "pre-updates" calculation into the window
		float pre_update_time = COREENGINE_S.pre_update_time;
		sys_time.emplace_back(pre_update_time);
		// Import the "debug-draw" calculation into the window
		float debug_draw_time = COREENGINE_S.debug_draw_time;
		sys_time.emplace_back(debug_draw_time);
		// Calculate for Idle time
		auto& fps = FRAMECTR_S;
		float fpsMS = fps.GetDeltaTime() * 1000.f;
		float idle_time = fpsMS - used_time - pre_update_time - debug_draw_time;
		sys_time.emplace_back(idle_time);
	}

	// Updating the FPS counter
	if (fps_overtime_counter < 0)
		fps_overtime_counter = MAX_FPS_COUNTER - 1;
	fps_overtime[fps_overtime_counter--] = (1.f / DELTATIME);
}