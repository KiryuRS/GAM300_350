#pragma once
#include <chrono>
class CoreEngine;

class Clock
{
	float m_time;
	std::chrono::high_resolution_clock::time_point m_stopwatch_start;
	float m_stopwatch_value;

public:
	Clock(void);
	
	inline float GetElapsedTime() const						{ return m_time; }
	std::chrono::high_resolution_clock::time_point GetHighestResolutionTime() const;
	void StartStopwatch();
	void StopStopwatch();
	void ClearStopwatch();
	float GetStopwatch() const								{ return m_stopwatch_value; }
	float GetStopwatchTime() const;
};