#pragma once
#include <chrono>
#include <ctime>
#include <Windows.h>			// Using the query performance counter
#include <string>

typedef class FrameRateController final
{
	using HighResClock = std::chrono::high_resolution_clock;

	float delta_time, logical_time;
	long long elapsed_time;
	long long frame_counter, frame_previous_counter;
	HighResClock::time_point tStart, tEnd;				// Used for delta time and FPS
	DWORD time_start, time_end;							// Used for calculation of system usage time
														// 60 FPS
	static constexpr float FPS = 60.f;
	static constexpr float frame_time = 1.f / FPS;

	// Conversion function
	float inline ConvertToFPS();

	bool Lock;
	bool capFPS;

public:
	FrameRateController();

	void SystemStartFrame();
	void SystemEndFrame();

	// For multithreading stuff
	void LockFrameRate(bool);
	bool FPSLocked() const;

	// Converting the delta time into the FPS version
	std::string GetFPS();
	// The delta time of the game (per frame)
	float GetDeltaTime() const;
	// Grabbing the elapsed system (game) time
	long long GetElapsedTime() const;
	// Grabbing the elapsed logical time (in seconds)
	float GetLogicalTime() const;
	// Determine if a frame has passed
	bool OneFramePassed() const;

	// Free Frame Rate
	void CapFrameRate(bool toggle)			{ capFPS = toggle; }
	bool IsFrameRateCapped() const			{ return capFPS; }
} FRC;