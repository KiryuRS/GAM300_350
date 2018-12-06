#pragma once

#include "Serializable.h"

class Entity;

// When adding events, remember to add to the 3 areas below!
enum class EVENTTYPE {
	ACTIVATEFUNCTION, CAMERASHAKE
};
#define EVENTTYPES ActivateFunction, CameraShake
namespace EventDirectory
{
	static constexpr auto names = strArr( "Activate_Function", "Camera_Shake" );
}


class Event : public Serializable
{
	bool finished;
	EVENTTYPE type;
public:
	Entity * owner;
	int eventNum;
	Event() : finished{ false }, owner{ nullptr }
	{}
	virtual void Activate() {}
	virtual void Update(float) {}
	void EndEvent() { finished = true; }
	inline bool EventEnded() const { return finished; }
	inline EVENTTYPE GetType() const { return type; }
	void SetType(EVENTTYPE etype) { type = etype; }
	std::unique_ptr<Event> Clone() const;
	virtual ~Event() {}
};

class CameraShake : public Event
{
	float intensity;
	float shakeTime;
public:
	CameraShake() : intensity{ 0.002f }, shakeTime{ 1.f }
	{
		AddSerializable("Intensity", intensity);
		AddSerializable("Shake_Time", shakeTime);
	}
	void Activate();
	void Update(float);
	static constexpr auto GetType() { return EVENTTYPE::CAMERASHAKE; }
};


class ActivateFunction : public Event
{
	Component* comp;
	std::string targetID;
	COMPONENTTYPE type;
public:
	std::unique_ptr<GeneralCallableFunction> callFunction;
	ActivateFunction() : type{ COMPONENTTYPE::TRANSFORM }
	{
		AddSerializable("Object_ID", targetID, "If ID is empty, the target is itself.");
		AddSerializable("Function", callFunction, "");
	}

	void Activate();
	static constexpr auto GetType() { return EVENTTYPE::ACTIVATEFUNCTION; }
};
