#include "stdafx.h"
#include "Clock.h"

Clock::Clock(void)
	: m_time{ 0 },
	  m_stopwatch_start{ }, m_stopwatch_value{ 0 }
{ }

std::chrono::high_resolution_clock::time_point Clock::GetHighestResolutionTime() const
{
	using HighResClock = std::chrono::high_resolution_clock;
	return HighResClock::now();
}

void Clock::StartStopwatch()
{
	m_stopwatch_start = GetHighestResolutionTime();
}

void Clock::StopStopwatch()
{
	using ChronoFloat = std::chrono::duration<float>;
	ChronoFloat time_span = std::chrono::duration_cast<ChronoFloat>(GetHighestResolutionTime() - m_stopwatch_start);
	m_stopwatch_value += (time_span.count() * 1000.f);
}

void Clock::ClearStopwatch()
{
	m_stopwatch_value = 0;
}

float Clock::GetStopwatchTime() const
{
	return m_stopwatch_value;
}
