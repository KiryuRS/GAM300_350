#pragma once
#include <functional>
#include <vector>
#include <utility>
#include <type_traits>

// Should not be able to copy into another timer class
class Timer
{
	using void_fn = std::function<void(void)>;
	// Currently the class can only support for function signature with void(void)
	std::vector<void_fn> vecFunc;

	// Variables for timers
	float interval, counter;

	// Misc variables
	bool paused, stop, onRepeat, hit;

public:

	Timer();
	Timer(float seconds, bool repeat = false);
	
	// Allows chaining of function calls when the timer is up (For now only allow adding of void(void) functions
	template <typename ... Functors, typename = std::enable_if_t<std::is_same<void, std::common_type_t<std::invoke_result_t<Functors>...>>::value>>
	void AssignFunction(Functors ... funcs)
	{
		(vecFunc.emplace_back(funcs), ...);
	}

	void Update();
	void ResetTimer(bool clearAllFunctions = true);
	void SetNewInterval(float newTimer);
	void PauseTimer()								{ paused = true; }
	void ResumeTimer()								{ paused = false; }
	void SetRepeatMode(bool repeat)					{ onRepeat = repeat; }
	void StopTimer()								{ stop = true; counter = 0; }
	bool TimesUp() const							{ return hit; }
	
	Timer(const Timer&)				= delete;
	Timer(Timer&&)					= delete;
	Timer& operator=(const Timer&)	= delete;
	Timer& operator=(Timer&&)		= delete;
};