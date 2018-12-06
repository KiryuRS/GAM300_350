#pragma once
#include "xinput.h"
#pragma comment(lib, "xinput.lib")

// Only 4 devices are supported by Windows
enum GAMEPAD_DEVICE
{
	GAMEPAD_0 = 0,
	GAMEPAD_1 = 1,
	GAMEPAD_2 = 2,
	GAMEPAD_3 = 3,

	GAMEPAD_COUNT 
};

#define GAMEPAD_DEADZONE_LEFT_STICK_VALUE			7849
#define GAMEPAD_DEADZONE_RIGHT_STICK_VALUE			8689
#define GAMEPAD_DEADZONE_TRIGGER_VALUE						30
#define FLAG_CONNECTED								(1 << 0)
#define FLAG_RUMBLE									(1 << 1)
#define BUTTON_TO_FLAG(b)							(1 << (b))

struct GAMEPAD_AXIS
{
	int x, y;
	float nx, ny;
	float length;
	float angle;
	GAMEPAD_STICKDIR dir_last, dir_current;
};

struct GAMEPAD_TRIGINFO
{
	int value;
	float length;
	bool pressed_last, pressed_current;
};

struct GAMEPAD_STATE
{
	unsigned flags;
	int bLast, bCurrent;
	std::array<GAMEPAD_AXIS, XINPUT_STICK_COUNT> stick;
	std::array<GAMEPAD_TRIGINFO, XINPUT_TRIGGER_COUNT> trigger;
};

class Controller
{
	friend bool AnyControllerConnected(const Controller& cntrl);

	std::array<GAMEPAD_STATE, GAMEPAD_COUNT> states;

	void UpdateDevice(unsigned index);
	void UpdateStick(GAMEPAD_AXIS& axis, float deadzone);
	void UpdateTrigger(GAMEPAD_TRIGINFO& trig);

public:
	Controller();
	void ResetState(GAMEPAD_DEVICE device);
	void Update();

	// Determines if the current controller is connected to the computer
	bool IsConnected(GAMEPAD_DEVICE device) const;

	// Test if a particular button is pressed
	bool ButtonDown(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const;

	// Test if a particular button has been "de-pressed" since the previous call to update()
	bool ButtonTriggered(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const;

	// Test if a particular button has been released since the previous call to update
	bool ButtonReleased(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const;

	// Get the trigger value (depression magnitude) in its raw form
	int TriggerValue(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const;

	// Get the trigger value (depression magnitude) in normalized form (0 - 1)
	float TriggerLength(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const;

	// Test if a trigger is	pressed
	bool TriggerDown(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const;

	// Test if a trigger has been "de-pressed" since the previous call to update()
	bool TriggerTriggered(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const;

	// Test if a trigger has been released since the previous call to update
	bool TriggerReleased(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const;

	/*
	
		Query the position of an analog stick as raw values

		The values retrieved by this function represent the magnitude of the analog stick in each direction.
		Note that it shouldn't be possible to get full magnitude in one direction unless the other direction has a magnitude of zero,
		as the stick has a circular movement range.

	*/
	void StickXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int& outX, int& outY) const;

	/*
	
		Query the magnitude of an analog stick.

		This returns the normalized value of the magnitude of the stick.
		That is, if the stick is paused all the way in any direction, it returns 1.0

	*/
	float StickLength(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const;

	/*
	
		Query the position of an analog stick as normalized values.

		The values retrieved by this function represented the magntiude of the analog stick in each direction.
		Note that it shouldn't be possible to get full magnitude in one direction unless the other direction has a magnitude of zero,
		as the stick has a circular movement range

	*/
	void StickNormXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int& outX, int& outY) const;

	/*
	
		Query the direction of a stick (in radians)

		This returns the direction of the stick. This value is in radians, not degrees.
		Zero is the right, and the angle increases in a counter-clockwise direction.
	
	*/
	float StickAngle(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const;

	/*
	
		Get the direction the stick is pushed in (if any)
		
		This is a useful utility function for when the stick should be treated as a simple directional pad, such as for menu UIs
	
	*/
	float StickDir(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const;

	/*
	
		Test whether a stick has been pressed in a particular direction since the last update.

		This only returns true if the stick was centered last frame.

		This is a useful utility function for when the stick should be treated as a simple directional pad, such as for menu UIs.
	
	*/
	bool StickDirTriggered(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR dir);

	/*
		Set the rumble motors on/off.
		To turn off the rumble effect, set the values to 0 for both motors.
		The left-motor is the low-frequency / strong motor, and the right motor is the high-frequency / weak motor

		Input values to be between 0 - 1
	*/
	void SetRumble(GAMEPAD_DEVICE device, float left_motor, float right_motor);
};
