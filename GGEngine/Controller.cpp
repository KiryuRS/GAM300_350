#include "stdafx.h"
#include "Controller.h"

#pragma warning (disable : 4244)
#pragma warning (disable : 4554)

void Controller::UpdateDevice(unsigned index)
{
	XINPUT_STATE xs;
	if (!XInputGetState(index, &xs))
	{
		auto& curr = states[index];
		// Reset if the device was not already connected
		if (!(curr.flags & FLAG_CONNECTED))
			ResetState(static_cast<GAMEPAD_DEVICE>(index));

		// Mark that we are connected with rumble support
		curr.flags |= FLAG_CONNECTED | FLAG_RUMBLE;
		// Update state
		curr.bCurrent = xs.Gamepad.wButtons;
		curr.trigger[XINPUT_TRIGGER_LEFT].value = xs.Gamepad.bLeftTrigger;
		curr.trigger[XINPUT_TRIGGER_RIGHT].value = xs.Gamepad.bRightTrigger;
		curr.stick[XINPUT_STICK_LEFT].x = xs.Gamepad.sThumbLX;
		curr.stick[XINPUT_STICK_LEFT].y = xs.Gamepad.sThumbLY;
		curr.stick[XINPUT_STICK_RIGHT].x = xs.Gamepad.sThumbRX;
		curr.stick[XINPUT_STICK_RIGHT].y = xs.Gamepad.sThumbRY;
	}
	else
		// Disconnected
		states[index].flags &= ~FLAG_CONNECTED;
}

void Controller::UpdateStick(GAMEPAD_AXIS & axis, float deadzone)
{
	static constexpr float STICK_MAX_VALUE = 32767.f;
	static constexpr float QUARTER_PI = M_PI / 4;
	static constexpr float THREE_QUARTER_PI = 3 * M_PI / 4;
	static constexpr float HALF_PI = M_PI / 2;

	// Determine magnitude of stick
	axis.length = sqrtf(static_cast<float>(axis.x * axis.x + axis.y * axis.y));
	if (axis.length > deadzone)
	{
		// clamp length to maximum value
		axis.length = axis.length > STICK_MAX_VALUE ? STICK_MAX_VALUE : axis.length;
		// Normalize the x and y values
		axis.nx = axis.x / axis.length;
		axis.ny = axis.y / axis.length;
		// Adjust length for deadzone and find normalized length
		axis.length -= deadzone;
		axis.length /= (STICK_MAX_VALUE - deadzone);
		// Find the angle of stick in radius
		axis.angle = atan2f(static_cast<float>(axis.y), static_cast<float>(axis.x));
	}
	else
	{
		axis.x = axis.y = 0;
		axis.nx = axis.ny = 0;
		axis.length = axis.angle = 0;
	}

	// Update the stick direction
	axis.dir_last = axis.dir_current;
	axis.dir_current = XINPUT_STICKDIR_CENTER;

	// Check direction to determine if it's non-centered
	if (axis.length)
	{
		if (axis.angle >= QUARTER_PI && axis.angle < THREE_QUARTER_PI)
			axis.dir_current = XINPUT_STICKDIR_UP;
		else if (axis.angle >= -THREE_QUARTER_PI && axis.angle < -QUARTER_PI)
			axis.dir_current = XINPUT_STICKDIR_DOWN;
		else if (axis.angle >= THREE_QUARTER_PI || axis.angle < -THREE_QUARTER_PI)
			axis.dir_current = XINPUT_STICKDIR_LEFT;
		else // if (axis.angle < QUARTER_PI && axis.angle >= -QUARTER_PI)
			axis.dir_current = XINPUT_STICKDIR_RIGHT;
	}
}

void Controller::UpdateTrigger(GAMEPAD_TRIGINFO & trig)
{
	trig.pressed_last = trig.pressed_current;

	if (trig.value > GAMEPAD_DEADZONE_TRIGGER_VALUE)
	{
		trig.length = ((trig.value - GAMEPAD_DEADZONE_TRIGGER_VALUE) / (255.f - GAMEPAD_DEADZONE_TRIGGER_VALUE));
		trig.pressed_current = true;
	}
	else
	{
		trig.value = 0;
		trig.length = 0;
		trig.pressed_current = false;
	}
}

Controller::Controller()
	: states{ }
{
	for (auto& elem : states)
	{
		elem.flags = 0;
		elem.bCurrent = 0;
		elem.bLast = 0;
	}
}

void Controller::ResetState(GAMEPAD_DEVICE device)
{
	auto& sticks = states[device].stick;
	for (auto& elem : sticks)
		SecureZeroMemory(&elem, sizeof(elem));

	auto& triggers = states[device].trigger;
	for (auto& elem : triggers)
		SecureZeroMemory(&elem, sizeof(elem));

	states[device].bCurrent = states[device].bLast = 0;
}

void Controller::Update()
{
	// Update all devices (for common values)
	for (unsigned i = 0; i != GAMEPAD_COUNT; ++i)
	{
		auto& curr = states[i];
		// Store previous button state
		curr.bLast = states[i].bCurrent;
		// per-platform update routine
		UpdateDevice(i);
		// Calculate refined stick and trigger values
		if (curr.flags & FLAG_CONNECTED)
		{
			UpdateStick(curr.stick[XINPUT_STICK_LEFT], GAMEPAD_DEADZONE_LEFT_STICK_VALUE);
			UpdateStick(curr.stick[XINPUT_STICK_RIGHT], GAMEPAD_DEADZONE_RIGHT_STICK_VALUE);

			UpdateTrigger(curr.trigger[XINPUT_TRIGGER_LEFT]);
			UpdateTrigger(curr.trigger[XINPUT_TRIGGER_RIGHT]);
		}

	}
}

bool Controller::IsConnected(GAMEPAD_DEVICE device) const
{
	return (states[device].flags & FLAG_CONNECTED);
}

bool Controller::ButtonDown(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const
{
	return (states[device].bCurrent & BUTTON_TO_FLAG(button));
}

bool Controller::ButtonTriggered(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const
{
	return (!(states[device].bLast & BUTTON_TO_FLAG(button))) &&
		   (states[device].bCurrent & BUTTON_TO_FLAG(button));
}

bool Controller::ButtonReleased(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) const
{
	return (!(states[device].bCurrent & BUTTON_TO_FLAG(button))) &&
		   (states[device].bLast & BUTTON_TO_FLAG(button));
}

int Controller::TriggerValue(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const
{
	return states[device].trigger[trigger].value;
}

float Controller::TriggerLength(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const
{
	return states[device].trigger[trigger].length;
}

bool Controller::TriggerDown(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const
{
	return states[device].trigger[trigger].pressed_current;
}

bool Controller::TriggerTriggered(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const
{
	return (states[device].trigger[trigger].pressed_current && !states[device].trigger[trigger].pressed_last);
}

bool Controller::TriggerReleased(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) const
{
	return (!states[device].trigger[trigger].pressed_current && states[device].trigger[trigger].pressed_last);
}

void Controller::StickXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int & outX, int & outY) const
{
	outX = states[device].stick[stick].x;
	outY = states[device].stick[stick].y;
}

float Controller::StickLength(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const
{
	return states[device].stick[stick].length;
}

void Controller::StickNormXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int & outX, int & outY) const
{
	outX = states[device].stick[stick].nx;
	outY = states[device].stick[stick].ny;
}

float Controller::StickAngle(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const
{
	return states[device].stick[stick].angle;
}

float Controller::StickDir(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) const
{
	return states[device].stick[stick].dir_current;
}

bool Controller::StickDirTriggered(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR dir)
{
	return (states[device].stick[stick].dir_current == dir && states[device].stick[stick].dir_current != states[device].stick[stick].dir_last);
}

void Controller::SetRumble(GAMEPAD_DEVICE device, float left_motor, float right_motor)
{
	// Hardcoded value to vibrate the controller
	const unsigned rumble_value = 65535;

	if (states[device].flags & FLAG_RUMBLE != 0)
	{
		XINPUT_VIBRATION vib;
		SecureZeroMemory(&vib, sizeof(XINPUT_VIBRATION));
		vib.wLeftMotorSpeed = static_cast<WORD>(left_motor * rumble_value);
		vib.wRightMotorSpeed = static_cast<WORD>(right_motor * rumble_value);
		XInputSetState(device, &vib);
	}
}
