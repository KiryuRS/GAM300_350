/******************************************************************************/
/*!
\File	   	       ControllerC.h
\Primary Author    Ng Yan Fei 50%
\Secondary Author  Kenneth Toh 50%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once
#include "stdafx.h"
#include "Model.h"
#include "EnumParser.h"

enum KEYS
{
	UPKEY,
	DOWNKEY,
	LEFTKEY,
	RIGHTKEY,
	INTERACT1,
	INTERACT2,
	LEFTCLICK,
	RIGHTCLICK,
	NUMKEYS
};

enum KEYS_KEYBOARD
{
	LBUTTON = KEY_LBUTTON, RBUTTON = KEY_RBUTTON, MBUTTON = KEY_MBUTTON,
	BACK = KEY_BACK, TAB = KEY_TAB, RETURN = KEY_RETURN, LSHIFT = KEY_LSHIFT, RSHIFT = KEY_RSHIFT,
	LCTRL = KEY_LCTRL, RCTRL = KEY_RCTRL, LALT = KEY_LALT, RALT = KEY_RALT, PRINTSCREEN = KEY_PRINTSCREEN,
	SCROLLLOCK = KEY_SCROLLLOCK, PAUSEBREAK = KEY_PAUSEBREAK, CAPSLOCK = KEY_CAPSLOCK,
	ESCAPE = KEY_ESCAPE, SPACE = KEY_SPACE, PAGEUP = KEY_PAGEUP, PAGEDOWN = KEY_PAGEDOWN,
	END = KEY_END, HOME = KEY_HOME, INSERT = KEY_INSERT, K_DELETE = KEY_DELETE,
	A_LEFT = KEY_LEFT, A_RIGHT = KEY_RIGHT, A_DOWN = KEY_DOWN, A_UP = KEY_UP,
	ZERO = KEY_0, ONE = KEY_1, TWO = KEY_2, THREE = KEY_3, FOUR = KEY_4, FIVE = KEY_5, SIX = KEY_6, SEVEN = KEY_7, EIGHT = KEY_8, NINE = KEY_9,
	A = KEY_A, B = KEY_B, C = KEY_C, D = KEY_D, E = KEY_E, F = KEY_F, G = KEY_G, H = KEY_H, I = KEY_I,
	J = KEY_J, K = KEY_K, L = KEY_L, M = KEY_M, N = KEY_N, O = KEY_O, P = KEY_P, Q = KEY_Q, R = KEY_R,
	S = KEY_S, T = KEY_T, U = KEY_U, V = KEY_V, W = KEY_W, X = KEY_X, Y = KEY_Y, Z = KEY_Z,
	F1 = KEY_F1, F2 = KEY_F2, F3 = KEY_F3, F4 = KEY_F4, F5 = KEY_F5, F6 = KEY_F6, F7 = KEY_F7, F8 = KEY_F8,
	F9 = KEY_F9, F10 = KEY_F10, F11 = KEY_F11, F12 = KEY_F12, NUM_0 = KEY_NUMPAD0, NUM_1 = KEY_NUMPAD1, NUM_2 = KEY_NUMPAD2,
	NUM_3 = KEY_NUMPAD3, NUM_4 = KEY_NUMPAD4, NUM_5 = KEY_NUMPAD5, NUM_6 = KEY_NUMPAD6, NUM_7 = KEY_NUMPAD7,
	NUM_8 = KEY_NUMPAD8, NUM_9 = KEY_NUMPAD9, NUM_MUL = KEY_NUM_MULTIPLY, NUM_PLUS = KEY_NUM_PLUS, NUM_MINUS = KEY_NUM_MINUS,
	NUM_PERIOD = KEY_NUM_PERIOD, NUM_DIVIDE = KEY_NUM_DIVIDE, NUMLOCK = KEY_NUMLOCK, SEMICOLON = KEY_SEMICOLON,
	BACKSLASH = KEY_BACKSLASH, PLUS = KEY_PLUS, KOMMA = KEY_COMMA, MINUS = KEY_MINUS, PERIOD = KEY_PERIOD, TILDE = KEY_TILDE,
	LBRACKET = KEY_LBRACKET, FRONTSLASH = KEY_FRONTSLASH, RBRACKET = KEY_RBRACKET, QUOTE = KEY_QUOTE
};

enum KEYS_CONTROLLER
{
	XINPUT_DPAD_UP = XINPUT_BUTTON_DPAD_UP, XINPUT_DPAD_DOWN = XINPUT_BUTTON_DPAD_DOWN, XINPUT_DPAD_LEFT = XINPUT_BUTTON_DPAD_LEFT, XINPUT_DPAD_RIGHT = XINPUT_BUTTON_DPAD_RIGHT, XINPUT_START = XINPUT_BUTTON_START,
	XINPUT_BACK = XINPUT_BUTTON_BACK, XINPUT_LEFT_THUMB = XINPUT_BUTTON_LEFT_THUMB, XINPUT_RIGHT_THUMB = XINPUT_BUTTON_RIGHT_THUMB, XINPUT_LEFT_SHOULDER = XINPUT_BUTTON_LEFT_SHOULDER, XINPUT_RIGHT_SHOULDER = XINPUT_BUTTON_RIGHT_SHOULDER,
	XINPUT_A = XINPUT_BUTTON_A, XINPUT_B = XINPUT_BUTTON_B, XINPUT_X = XINPUT_BUTTON_X, XINPUT_Y = XINPUT_BUTTON_Y
};

;
namespace
{
	//								KEYBOARD					CONTROLLER
	using KB_CTR = std::pair<std::vector<std::string>, std::vector<std::string>>;
	enum KEY_TYPE
	{
		KT_DOWN,
		KT_UP,
		KT_HOLD
	};
}

struct KeyMapping
{
	std::map<KEYS, std::vector<int>>				keyboard_map;
	//std::map<KEYS, std::vector<GamePadButtons>>		gamepad_map;
	// Enumerator to String convertor [DO NOT MODIFY UNLESS INTENDED]
	EnumConversions<KEYS_KEYBOARD>					kb_ec;
	EnumConversions<KEYS_CONTROLLER>				ctr_ec;

	// Initialize the default key values
	KeyMapping();
	// Determine if the key is pressed
	bool KeyMatched(KEY_TYPE type, KEYS key)
	{
		// Getting the vector for keyboard_map
		const auto& keyboard_vect = keyboard_map[key];
		// Run through the vector and determine if any of the key has been pressed
		Input& ctr = INPUT_S;
		for (auto& keyboard_key : keyboard_vect)
		{
			switch (type)
			{
			case KT_DOWN:
				if (ctr.GetToggleKey(keyboard_key))
					return true;
				break;

			case KT_UP:
				if (ctr.GetReleaseKey(keyboard_key))
					return true;
				break;

			case KT_HOLD:
				if (ctr.GetHoldKey(keyboard_key))
					return true;
				break;
			}
		}

		// Reaching here means no key is activated
		return false;
	}

	bool KeyMatchedInt(KEY_TYPE type, int key)
	{
		// Run through the vector and determine if any of the key has been pressed
		Input& ctr = INPUT_S;
		switch (type)
		{
		case KT_DOWN:
			if (ctr.GetToggleKey(key))
				return true;
			break;

		case KT_UP:
			if (ctr.GetReleaseKey(key))
				return true;
			break;

		case KT_HOLD:
			if (ctr.GetHoldKey(key))
				return true;
			break;
		}

		// Reaching here means no key is activated
		return false;
	}

	// Determine for Controller
	/*bool KeyMatchedGP(KEY_TYPE type, KEYS key)
	{
		// Getting the vector for keyboard_map
		const auto& gamepad_vect = gamepad_map[key];
		// Run through the vector and determine if any of the key has been pressed
		Controller& ctr = COREENGINE_S.GetInput().GetController();
		for (auto& gamepad_key : gamepad_vect)
			switch (type)
			{
			case KT_DOWN:
				if (ctr.GetToggleKey<CONTROLLER>(gamepad_key))
					return true;
				break;

			case KT_UP:
				if (ctr.GetReleaseKey<CONTROLLER>(gamepad_key))
					return true;
				break;

			case KT_HOLD:
				if (ctr.GetHoldKey<CONTROLLER>(gamepad_key))
					return true;
				break;
			}

		// Reaching here means no key is activated
		return false;
	}*/
};

class ControllerC : public Component
{
	Input* input;
	Controller* ctl;
	bool gpActive;
	// For serializable
	std::map<KEYS, KB_CTR> keystring_map;
	KeyMapping km;
	float stickThreshold;

#ifdef EDITOR
	bool IsScenePaused() const;
#endif

public:
	ControllerC();

	void Initialize() override;
	void Update(float dt);

	static constexpr COMPONENTTYPE GetType() { return COMPONENTTYPE::CONTROLLER; }

	bool GetKeyDown(KEYS key);
	bool GetKeyDownInt(int key);
	bool GetKeyDownStr(std::string key);
	bool GetKeyUp(KEYS key);
	bool GetKeyUpInt(int key);
	bool GetKeyUpStr(std::string key);
	bool GetKeyHold(KEYS key);
	bool GetKeyHoldInt(int key);
	bool GetKeyHoldStr(std::string key);
	Vector2 GetMousePosition();
	bool GamepadActive();
	bool GetLStickMove();
	bool GetRStickMove();
	Vector2 GetLStickPos();
	Vector2 GetRStickPos();
	bool GetLStickDown(KEYS);
	bool GetRStickDown(KEYS);
	void AddSerializeData(LuaScript* state = nullptr) override;
	int GetScrollAmount();
	int GetScrollAmtPerFrame();
};