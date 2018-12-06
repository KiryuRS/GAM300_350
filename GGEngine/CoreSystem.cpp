#include "stdafx.h"
#include "CoreSystem.h"

void CoreSystem::PreUpdate()
{
#if defined EDITOR
	g_clock.StartStopwatch();
#endif
}

void CoreSystem::PostUpdate()
{
#if defined EDITOR
	g_clock.StopStopwatch();
	time_interval = g_clock.GetStopwatchTime();
	g_clock.ClearStopwatch();
#endif
}

void CoreSystem::SelfUpdate()
{
	// Capture the stop watch
	PreUpdate();
	UpdateWndFocus();
	/*
		override_update determines if the system should be updated irregardless of the window's focus
		to_update is the boolean check for the windows's focus
	*/
	if (override_update || to_update)
		// Perform actual updating (only exception to GSM where it requires overriding of this function)
		Update();
	// Stop the stop watch
	PostUpdate();
}

void CoreSystem::SetName(const std::string & str)
{
	system_name = str;
}

void CoreSystem::OverrideUpdate()
{
	override_update = true;
}

void CoreSystem::UpdateWndFocus()
{
	// Update the to_update
	to_update = COREENGINE_S.GetWindow().IsFocused();
}

CoreSystem::CoreSystem()
	: to_update{ true }, system_name{ },
	  time_interval{ 0 }, g_clock{ CLOCK_S },
	  override_update{ false }
{ }

void CoreSystem::PrintInfo() const
{
	std::string to_print;
	to_print += "System - ";
	to_print += system_name;
	to_print += "'s performance time per frame: ";
	CONSOLE_SYSTEM(to_print, GetInterval());
}
