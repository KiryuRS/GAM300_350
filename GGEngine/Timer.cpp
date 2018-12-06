#include "stdafx.h"

Timer::Timer()
	// Delegating constructor
	: Timer{ 0 }
{ }

Timer::Timer(float time, bool repeat)
	: interval{ time }, onRepeat{ repeat },
	  counter{ 0 }, hit{ false }, paused{ false }, stop{ interval ? false : true }
{ }

void Timer::Update()
{
	// On the next frame if "hit" is true, set it to false
	hit &= 0;

	// We do not update anything if these two booleans are actives
	if (stop || paused)
		return;

	// Update based on the delta time
	counter += DELTATIME;
	// Ring Ring if the timer is up
	if (counter >= interval)
	{
		if (!onRepeat)
			stop = true;
		counter = 0;

		hit = true;
		// Call our functions
		for (auto& elem : vecFunc)
			elem();
	}
}

void Timer::ResetTimer(bool clearAllFunctions)
{
	if (clearAllFunctions)
		vecFunc.clear();

	// Reset all of the variables
	counter = interval = 0.f;
	hit = false;
	paused = false;
	stop = true;
}

void Timer::SetNewInterval(float newTimer)
{
	// Call for reset
	ResetTimer(false);

	// Assign the new time
	interval = newTimer;
	stop = false;
}
