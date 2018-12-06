#include "stdafx.h"
#include "FrameRate.h"

inline float FrameRateController::ConvertToFPS()		{ return 1.f / delta_time; }
float FrameRateController::GetDeltaTime() const			{ return delta_time; }
long long FrameRateController::GetElapsedTime() const	{ return time_end - time_start; }
float FrameRateController::GetLogicalTime() const		{ return logical_time; }
bool FrameRateController::OneFramePassed() const		{ return frame_counter == frame_previous_counter; }

FrameRateController::FrameRateController()
	: delta_time{ 0 }, elapsed_time{ 0 },
	  tStart{ {} }, tEnd{ {} }, Lock{ false }, logical_time{ 0 },
	  frame_previous_counter{ 0 }, frame_counter{ 0 }, capFPS{ true }
{
	// Get the current time
	time_start = static_cast<DWORD>(GetTickCount64());
}

void FrameRateController::SystemStartFrame()
{
	// Get the previous elapsed time
	elapsed_time = (tEnd - tStart).count();

	// Accumulate the previous delta time and append to our logical time
	logical_time += delta_time;

	// Capture the previous frame counter
	frame_previous_counter = frame_counter;

	// Calculate the tStart
	tStart = HighResClock::now();
}

void FrameRateController::SystemEndFrame()
{
	static constexpr auto dtCap = 0.1f;
	delta_time = 0;
	if (capFPS)
	{
		while (delta_time < frame_time)
		{
			using ChronoFloat = std::chrono::duration<float>;

			tEnd = HighResClock::now();
			ChronoFloat time_span = std::chrono::duration_cast<ChronoFloat>(tEnd - tStart);
			delta_time = time_span.count();
		}
		delta_time = delta_time > dtCap ? dtCap : delta_time;
	}
	else
	{
		using ChronoFloat = std::chrono::duration<float>;
		
		tEnd = HighResClock::now();
		ChronoFloat time_span = std::chrono::duration_cast<ChronoFloat>(tEnd - tStart);
		delta_time = time_span.count();
	}

	// One frame time passed
	++frame_counter;

	// Get the current time
	time_end = static_cast<DWORD>(GetTickCount64());
}

void FrameRateController::LockFrameRate(bool lock)
{
	Lock = lock;
}

bool FrameRateController::FPSLocked() const
{
	return Lock;
}

std::string FrameRateController::GetFPS()
{
	// Retrieve our FPS in float
	float fps = ConvertToFPS();
	// Create our buffer for storing float into string
	char buffer[11];
	int result = snprintf(buffer, sizeof(buffer), "%f", fps);

	std::string str;
	if (result)
	{
		str = buffer;
		str += " FPS";
	}

	return str;
}
