#pragma once
#include <string>				// std::string

/*
	Used to determine which classes are the core system for the engine
	- Useful for potential performance checks on each of the system
	- Could be used for checking if the window is off focus
*/
class CoreSystem
{
	bool override_update;		// determines if the system should be updating regardless of the to_update status
	bool to_update;				// determines if the system should be updating on certain conditions
	std::string system_name;	// the name of the system
	float time_interval;		// the current time for the system to complete a certain task
	Clock& g_clock;				// the global clock from the game engine

	float update_interval;		

protected:
	// Activates the stopwatch
	void PreUpdate();
	// Stops the stopwatch
	void PostUpdate();
	// Set the system name
	void SetName(const std::string& str);
	// Override the updating section
	void OverrideUpdate();
	// Updates the focus
	void UpdateWndFocus();

	virtual void Update() = 0;

public:
	CoreSystem();
	inline bool IsActive() const				{ return to_update; }
	inline std::string GetName() const			{ return system_name; }
	inline float GetInterval() const			{ return time_interval; }
	inline bool GetOverrideUpdate() const		{ return override_update; }
	void PrintInfo() const;
	// Any misc. updates to the system (e.g. to determine if a window should be updated)
	virtual void SelfUpdate();
};