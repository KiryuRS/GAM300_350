#pragma once
#include "Controller.h"
#include <imgui.h>

class Window;
bool AnyControllerConnected(const Controller& cntrl);
#define MAX_DOUBLE_CLICK_INTERVAL			0.5f

namespace
{
	enum MOUSECLICKSTATES
	{
		NO_MOUSE_CLICK = 0,
		SINGLE_MOUSE_CLICK,
		DOUBLE_MOUSE_CLICK
	};
}

class Input final
{
	// Mouse Information (for relative and global)
	struct MouseInfo
	{
		bool display;						// Determines if the mouse is to be displayed
		bool lock;							// Determine if the mouse is to be locked in the middle of the screen
		bool doubleClicked;					// Check if the mouse is double clicked
		MOUSECLICKSTATES clickState;		// Click State
		int scrollAmt;						// Scrolling intensity
		int scrollPerFrame;					// Scroll Amount per frame
		int prevScrollAmt;					// For calculation of the scrollPerFrame
		Vector2 mousePos;					// Mouse position from -1 to 1
		Vector2 windowMousePos;				// Mouse position in pixels
		Vector2 ghostMousePos;				// Ghost mouse position normalized
		Vector2 ghostWindowMousePos;		// Ghost mouse position in pixels

		MouseInfo()
			: scrollAmt{ 0 }, display{ true }, lock{ false }, doubleClicked{ false },
			  clickState{ NO_MOUSE_CLICK }, scrollPerFrame{ 0 }, prevScrollAmt{ 0 }
		{ }
	};

	// Friend classes
	friend class GuiManager;
	friend class InputManager;
	friend class Editor;

	Controller cntrl;
	MouseInfo mouse;
	std::array<bool, MAX_KEYS> keyHold, keyReleased, keyToggled;
	float doubleClickTimer;

	void UpdateMouseState(Window& win);
	void UpdateKeyState();

public:

	std::string name;
	Input();
	void Update(Window& win);
	// Gets the mouse position from -1 to 1
	auto GetMousePosition() const						{ return mouse.lock ? mouse.ghostMousePos : mouse.mousePos; }
	// Gets the mouse position in pixels
	auto GetPixelMousePosition() const					{ return mouse.lock ? mouse.ghostWindowMousePos : mouse.windowMousePos; }
	/*
		Values:
		0 : LMB
		1 : RMB
		2 : MMB
	*/
	bool GetMousePress(unsigned);
	// Determine if its a double click
	inline bool DoubleClicked() const					{ return mouse.doubleClicked; }
	inline void IncreaseScrollPosition(int amt)			{ mouse.scrollAmt += amt; }
	int GetScrollAmount() const							{ return mouse.scrollAmt; }
	int GetScrollAmtPerFrame() const					{ return mouse.scrollPerFrame; }
	inline bool MouseDisplay() const					{ return mouse.display; }
	inline bool MouseLocked() const						{ return mouse.lock; }
	DWORD GetKeyboardIdleTime() const;
	// Test if a key (or mouse input) is on hold since the last update()
	bool GetHoldKey(int key);
	// Test if a key (or mouse input) is pressed
	bool GetToggleKey(int tKey);
	// Test if a key (or mouse input) is depressed
	bool GetReleaseKey(int rKey);
	Controller& GetController();
	void ResetKeys();
	/*
		key1 would be the main (hold)
		key2 would be the final (pressed)
	*/
	bool CheckCombination(int key1, int key2);
	bool operator==(const Input& rhs) const;
	void LockMouseIntoCenter(bool toggle);
	void ShowMouse(bool toggle);
};

