#include "stdafx.h"
#include "Input.h"
#include "Window.h"

bool AnyControllerConnected(const Controller& cntrl)
{
	for (unsigned i = 0; i != GAMEPAD_COUNT; ++i)
		if (cntrl.IsConnected((GAMEPAD_DEVICE)i))
			return true;
	return false;
}

Input::Input()
	: keyHold{ false },
	  keyReleased{ false }, keyToggled{ false }, doubleClickTimer{ 0 }
{ }

void Input::Update(Window& win)
{	
	UpdateMouseState(win);

	UpdateKeyState();
	
	// Update Controller when there's at least one connected
	if (AnyControllerConnected(cntrl))
		cntrl.Update();
}

bool Input::GetMousePress(unsigned key)
{
	switch (key)
	{
	case 0:
		return keyHold[KEY_LBUTTON];
	case 1:
		return keyHold[KEY_RBUTTON];
	case 2:
		return keyHold[KEY_MBUTTON];
	}
	return false;
}

DWORD Input::GetKeyboardIdleTime() const
{
	LASTINPUTINFO pInput;
	pInput.cbSize = sizeof(pInput);

	// Error messages
	TOOLS::Assert(GetLastInputInfo(&pInput), "Unable to get the Input information!");

	// Indicate in milliseconds
	return pInput.dwTime;
}

bool Input::GetHoldKey(int key)
{
#ifdef EDITOR
	if (ImGui::IsAnyItemActive())
		return false;
#endif // EDITOR
	return keyHold[key];
}

bool Input::GetToggleKey(int tKey)
{
#ifdef EDITOR
	if (ImGui::IsAnyItemActive())
		return false;
#endif // EDITOR
	return keyToggled[tKey];
}

bool Input::GetReleaseKey(int rKey)
{
#ifdef EDITOR
	if (ImGui::IsAnyItemActive())
		return false;
#endif // EDITOR

	return keyReleased[rKey];
}

Controller & Input::GetController()
{
	return cntrl;
}

void Input::ResetKeys()
{
	// Reset all of the keys to 0 (including the controller)
	for (unsigned i = 0; i != MAX_KEYS; ++i)
	{
		keyHold[i] = false;
		keyToggled[i] = false;
		keyReleased[i] = false;
	}

	// All 4 controllers
	for (unsigned i = 0; i != GAMEPAD_COUNT; ++i)
		if (cntrl.IsConnected((GAMEPAD_DEVICE)i))
			cntrl.ResetState((GAMEPAD_DEVICE)i);
}

bool Input::CheckCombination(int key1, int key2)
{
	if (keyHold[key1])
		if (keyToggled[key2])
			return true;
	return false;
}

bool Input::operator==(const Input & rhs) const
{
	return this == &rhs;
}

void Input::LockMouseIntoCenter(bool toggle)
{
	mouse.lock = toggle;
	mouse.ghostWindowMousePos = mouse.windowMousePos;
	mouse.ghostMousePos = mouse.mousePos;
}

void Input::ShowMouse(bool toggle)
{
	mouse.display = toggle;
	ActualShowCursor(mouse.display);
}

void Input::UpdateMouseState(Window& window)
{
	// For the mouse scrolling
	mouse.prevScrollAmt = mouse.scrollAmt;
	mouse.scrollAmt = (int)window.GetScrollAmount();
	mouse.scrollPerFrame = mouse.scrollAmt - mouse.prevScrollAmt;

	POINT p;
	if (GetCursorPos(&p))
	{
		window.MapToClient(&p);
		const Vector2& wh = window.GetScreenSize();
		if (mouse.lock)
		{
			// Retrieve the current position information
			RECT rectClient;
			GetWindowRect(window.GetWindowHandle(), &rectClient);

			// Append the ghostMousePosition
			Vector2 diff = Vector2{ p.x, p.y } - mouse.windowMousePos;
			mouse.ghostWindowMousePos += diff;
			mouse.ghostMousePos = Vector2{ mouse.ghostWindowMousePos.x / (wh.x * 0.5f) - 1, -mouse.ghostWindowMousePos.y / (wh.y * 0.5f) + 1 };

			// Once we get the difference, set the windowMousePos back to the "locked" position			
			LONG x = rectClient.right - rectClient.left;
			LONG y = rectClient.bottom - rectClient.top;
			SetCursorPos(x / 2 + rectClient.left, y / 2 + rectClient.top);
			GetCursorPos(&p);
			window.MapToClient(&p);
		}
		mouse.mousePos = Vector2{ p.x / (wh.x * 0.5f) - 1, -p.y / (wh.y * 0.5f) + 1 };
		mouse.windowMousePos = Vector2{ p.x, p.y };
	}

	// "Finite State Machine" to determine if it was clicked
	// Referenced from https://stackoverflow.com/questions/15226791/distinguish-single-click-from-double-click-c
	switch (mouse.clickState)
	{
		// Default state here
	case NO_MOUSE_CLICK:
		// Check for the mouse click (LMOUSE)
		if (keyToggled[KEY_LBUTTON])
			mouse.clickState = SINGLE_MOUSE_CLICK;
		break;

		// To check if there's the mouse click within the interval
	case SINGLE_MOUSE_CLICK:
		// This means we are still within the interval range
		if (doubleClickTimer < MAX_DOUBLE_CLICK_INTERVAL)
		{
			doubleClickTimer += DELTATIME;
			if (keyToggled[KEY_LBUTTON])
			{
				mouse.doubleClicked = true;
				mouse.clickState = DOUBLE_MOUSE_CLICK;
			}
		}
		// This means we exceeded the timer to wait
		else
		{
			// Reset back
			mouse.clickState = NO_MOUSE_CLICK;
			doubleClickTimer = 0;
		}
		break;

		// Achieved double click, we will now reset back to normal
	case DOUBLE_MOUSE_CLICK:
		mouse.doubleClicked = false;
		mouse.clickState = NO_MOUSE_CLICK;
		doubleClickTimer = 0;
		break;
	}
}

void Input::UpdateKeyState()
{
	bool key_pressed = false;
	for (unsigned i = 0; i != MAX_KEYS; ++i)
	{
		// Check if the key was pressed before
		key_pressed = keyHold[i];

		// Check if the low-order bit is 1 [means key is toggled]
		keyToggled[i] = !key_pressed && (GetAsyncKeyState(i) & 0x8000) ? true : false;
		// Check if the high-order bit is 1 [means key is down]
		keyHold[i] = (GetAsyncKeyState(i) & 0x8000) ? true : false;

		// Check if the key has been released from hold
		keyReleased[i] = key_pressed && !keyHold[i];
	}
}
