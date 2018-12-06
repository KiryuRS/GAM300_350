/******************************************************************************/
/*!
\File	   	       ControllerC.cpp
\Primary Author    Ng Yan Fei 50%
\Secondary Author  Kenneth Toh 50%
\Project Name      Nameless


Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "stdafx.h"
#include "ControllerC.h"
#include <cctype>

KeyMapping::KeyMapping()
{
	// Get our string to key mapping
	kb_ec =
	{
		{LBUTTON, "LMOUSE"}, {RBUTTON, "RMOUSE"}, {MBUTTON, "MMOUSE"}, {BACK, "BACKSPACE"},
		{TAB, "TAB"}, {RETURN, "ENTER"}, {LSHIFT, "LSHIFT"}, {RSHIFT, "RSHIFT"},
		{LCTRL, "LCTRL"}, {RCTRL, "RCTRL"}, {LALT, "LALT"}, {RALT, "RALT"}, {PRINTSCREEN, "PRTSC"},
		{SCROLLLOCK, "SCRLK"}, {PAUSEBREAK, "PAUSEBREAK"}, {CAPSLOCK, "CAPS"}, {ESCAPE, "ESC"},
		{SPACE, "SPACE"}, {PAGEUP, "PAGEUP"}, {PAGEDOWN, "PAGEDOWN"}, {END, "END"}, {HOME, "HOME"},
		{INSERT, "INS"}, {K_DELETE, "DEL"}, {A_LEFT, "LEFT"}, {A_RIGHT, "RIGHT"}, {A_UP, "UP"},
		{A_DOWN, "DOWN"}, {ZERO, "0"}, {ONE, "1"}, {TWO, "2"}, {THREE, "3"}, {FOUR, "4"}, {FIVE, "5"},
		{SIX, "6"}, {SEVEN, "7"}, {EIGHT, "8"}, {NINE, "9"}, {A, "A"}, {B, "B"}, {C, "C"}, {D, "D"},
		{E, "E"}, {F, "F"}, {G, "G"}, {H, "H"}, {I, "I"}, {J, "J"}, {K, "K"}, {L, "L"}, {M, "M"},
		{N, "N"}, {O, "O"}, {P, "P"}, {Q, "Q"}, {R, "R"}, {S, "S"}, {T, "T"}, {U, "U"}, {V, "V"}, {W, "W"}, {X, "X"},
		{Y, "Y"}, {Z, "Z"}, {F1, "F1"}, {F2, "F2"}, {F3, "F3"}, {F4, "F4"}, {F5, "F5"}, {F6, "F6"},
		{F7, "F7"}, {F8, "F8"}, {F9, "F9"}, {F10, "F10"}, {F11, "F11"}, {F12, "F12"}, {NUM_0, "NUM0"},
		{NUM_1, "NUM1"}, {NUM_2, "NUM2"}, {NUM_3, "NUM3"}, {NUM_4, "NUM4"}, {NUM_5, "NUM5"}, {NUM_6, "NUM6"},
		{NUM_7, "NUM7"}, {NUM_8, "NUM8"}, {NUM_9, "NUM9"}, {NUM_MUL, "NUM*"}, {NUM_PLUS, "NUM+"}, {NUM_MINUS, "NUM-"},
		{NUM_PERIOD, "NUM."}, {NUM_DIVIDE, "NUM/"}, {NUMLOCK, "NUMLK"}, {SEMICOLON, ";"}, {BACKSLASH, "/"},
		{PLUS, "+"}, {KOMMA, ","}, {MINUS, "-"}, {PERIOD, "."}, {TILDE, "`"}, {FRONTSLASH, "\\"},
		{RBRACKET, "["}, {LBRACKET, "]"}, {QUOTE, "'"}
	};

	ctr_ec =
	{
		{XINPUT_DPAD_UP, "DPAD_UP"}, {XINPUT_DPAD_DOWN, "DPAD_DOWN"}, {XINPUT_DPAD_LEFT, "DPAD_LEFT"}, {XINPUT_DPAD_RIGHT, "DPAD_RIGHT"},
		{XINPUT_START, "START"}, {XINPUT_BACK, "BACK"}, {XINPUT_LEFT_THUMB, "LEFT_THUMB"}, {XINPUT_RIGHT_THUMB, "RIGHT_THUMB"},
		{XINPUT_LEFT_SHOULDER, "LEFT_SHOULDER"}, {XINPUT_RIGHT_SHOULDER, "RIGHT_SHOULDER"},
		{XINPUT_A, "A"}, {XINPUT_B, "B"}, {XINPUT_X, "X"}, {XINPUT_Y, "Y"}
	};
}

#ifdef EDITOR
bool ControllerC::IsScenePaused() const
{
	return !EDITOR_S->IsPlaying();
}
#endif

ControllerC::ControllerC()
	: input{ nullptr },
	ctl{ nullptr },
	gpActive{ false },
	stickThreshold{0.5f},
	keystring_map{ }
{
	
}

void ControllerC::AddSerializeData(LuaScript* state)
{
	// Keyboard
	AddSerializable("Up_Key", keystring_map[UPKEY].first, "", state);
	AddSerializable("Down_Key", keystring_map[DOWNKEY].first, "", state);
	AddSerializable("Left_Key", keystring_map[LEFTKEY].first, "", state);
	AddSerializable("Right_Key", keystring_map[RIGHTKEY].first, "", state);
	AddSerializable("Interact_Key", keystring_map[INTERACT1].first, "", state);
	AddSerializable("Interact2_Key", keystring_map[INTERACT2].first, "", state);
	AddSerializable("LeftClick_Key", keystring_map[LEFTCLICK].first, "", state);
	AddSerializable("RightClick_Key", keystring_map[RIGHTCLICK].first, "", state);

	// Controller
	/*AddSerializable("Up_GamePad", keystring_map[UPKEY].second, "", state);
	AddSerializable("Down_GamePad", keystring_map[DOWNKEY].second, "", state);
	AddSerializable("Left_GamePad", keystring_map[LEFTKEY].second, "", state);
	AddSerializable("Right_GamePad", keystring_map[RIGHTKEY].second, "", state);
	AddSerializable("Interact_GamePad", keystring_map[INTERACT1].second, "", state);
	AddSerializable("Interact2_GamePad", keystring_map[INTERACT2].second, "", state);
	AddSerializable("LeftClick_GamePad", keystring_map[LEFTCLICK].second, "", state);
	AddSerializable("RightClick_GamePad", keystring_map[RIGHTCLICK].second, "", state);
	AddSerializable("Joystick_Threshold", stickThreshold, "", state);*/

	AddCallableFunction("GetKeyDown", &ControllerC::GetKeyDownStr, state);
	AddCallableFunction("GetKeyUp", &ControllerC::GetKeyUpStr, state);
	AddCallableFunction("GetKeyHold", &ControllerC::GetKeyHoldStr, state);
	AddCallableFunction("GetScrollAmount", &ControllerC::GetScrollAmount, state);
	AddCallableFunction("GetMousePosition", &ControllerC::GetMousePosition, state);
	AddCallableFunction("GetScrollAmtPerFrame", &ControllerC::GetScrollAmtPerFrame, state);
}

void ControllerC::Initialize()
{
	input = &INPUT_S;
	ctl = &input->GetController();
	gpActive = ctl->IsConnected(GAMEPAD_0);
	// We will convert all of our strings into enum and place them into our Key Mapping
	for (unsigned i = UPKEY; i != NUMKEYS; ++i)
	{
		std::vector<int> vect_keyboard;
		//std::vector<GamePadButtons> vect_gamepad;

		// KEYBOARD SECTION
		for (auto& keyboard_key : keystring_map[static_cast<KEYS>(i)].first)
			vect_keyboard.emplace_back(static_cast<int>(km.kb_ec.GetEnum(keyboard_key)));

		// GAMEPAD SECTION
		//for (auto& gamepad_key : keystring_map[static_cast<KEYS>(i)].second)
		//	vect_gamepad.emplace_back(static_cast<GamePadButtons>(km.ctr_ec.GetEnum(gamepad_key)));

		// Push our keys into the keyboard mapping
		km.keyboard_map[static_cast<KEYS>(i)] = vect_keyboard;
		//km.gamepad_map[static_cast<KEYS>(i)] = vect_gamepad;
	}
}

void ControllerC::Update(float)
{
	// Updates the gamepad to see if the controller is plugged in
	gpActive = ctl->IsConnected(GAMEPAD_0);
}

bool ControllerC::GetKeyDown(KEYS key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool gamepad_key = gpActive ? km.KeyMatchedGP(KT_HOLD, key) : false;
	//if (key == KEYS::LEFTCLICK && input.MouseOverGUIWindow())
	//	return false;
	return km.KeyMatched(KT_DOWN, key);// || gamepad_key;
}

bool ControllerC::GetKeyUp(KEYS key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool gamepad_key = gpActive ? km.KeyMatchedGP(KT_UP, key) : false;
	return km.KeyMatched(KT_UP, key);// || gamepad_key;
}

bool ControllerC::GetKeyHold(KEYS key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool gamepad_key = gpActive ? km.KeyMatchedGP(KT_HOLD, key) : false;
	return km.KeyMatched(KT_HOLD, key);// || gamepad_key;
}

bool ControllerC::GetKeyDownInt(int key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool ctrKey = gpActive ? ctl.GetHoldKey<CONTROLLER>(key) : false;
	//if (key == KEYS::LEFTCLICK && input.MouseOverGUIWindow())
	//	return false;
	return km.KeyMatched(KT_DOWN, (KEYS)key);// || gamepad_key;
}

bool ControllerC::GetKeyDownStr(std::string key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	return km.KeyMatchedInt(KT_DOWN, km.kb_ec.GetEnum(key));
}

bool ControllerC::GetKeyUpInt(int key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool ctrKey = gpActive ? ctl.GetReleaseKey<CONTROLLER>(key) : false;
	return km.KeyMatched(KT_UP, (KEYS)key);// || gamepad_key;
}

bool ControllerC::GetKeyUpStr(std::string key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	return km.KeyMatchedInt(KT_UP, km.kb_ec.GetEnum(key));
}


bool ControllerC::GetKeyHoldInt(int key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	//bool ctrKey = gpActive ? ctl.GetHoldKey<CONTROLLER>(key) : false;
	return km.KeyMatched(KT_HOLD, (KEYS)key);// || gamepad_key;
}

bool ControllerC::GetKeyHoldStr(std::string key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	return km.KeyMatchedInt(KT_HOLD, km.kb_ec.GetEnum(key));
}

Vector2 ControllerC::GetMousePosition()
{
	return input->GetMousePosition();
}

bool ControllerC::GamepadActive()
{
	return gpActive;
}

bool ControllerC::GetLStickMove()
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	/*return ctl.GetCtrLThumbStickPos() != 0;*/
	//return GetLStickDown(KEYS::LEFTKEY) || GetLStickDown(KEYS::RIGHTKEY) || GetLStickDown(KEYS::UPKEY) || GetLStickDown(KEYS::DOWNKEY);
	return false;
}

bool ControllerC::GetRStickMove()
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	/*return ctl.GetCtrRThumbStickPos() != 0;*/
	//return GetRStickDown(KEYS::LEFTKEY) || GetRStickDown(KEYS::RIGHTKEY) || GetRStickDown(KEYS::UPKEY) || GetRStickDown(KEYS::DOWNKEY);
	return false;
}

Vector2 ControllerC::GetLStickPos()
{
	/*Vec2D pos = ctl.GetCtrLThumbStickPos();
	pos /= pow(2.f, 15);
	return pos;*/
	return Vector2();
}

Vector2 ControllerC::GetRStickPos()
{
	/*Vec2D pos = ctl.GetCtrRThumbStickPos();
	pos /= pow(2.f, 15);
	return pos;*/
	return Vector2();
}

bool ControllerC::GetLStickDown(KEYS key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	UNREFERENCED_PARAMETER(key);
	/*switch (key)
	{
	case KEYS::LEFTKEY:
		if (GetLStickPos().x < -stickThreshold)
			return true;
		break;
	case KEYS::RIGHTKEY:
		if (GetLStickPos().x > stickThreshold)
			return true;
		break;
	case KEYS::UPKEY:
		if (GetLStickPos().y > stickThreshold)
			return true;
		break;
	case KEYS::DOWNKEY:
		if (GetLStickPos().y < -stickThreshold)
			return true;
		break;
	}*/
	return false;
}

bool ControllerC::GetRStickDown(KEYS key)
{
#ifdef EDITOR
	if (IsScenePaused())
		return false;
#endif
	UNREFERENCED_PARAMETER(key);
	/*switch (key)
	{
	case KEYS::LEFTKEY:
		if (GetRStickPos().x < 0.1f)
			return true;
		break;
	case KEYS::RIGHTKEY:
		if (GetRStickPos().x > 0.1f)
			return true;
		break;
	case KEYS::UPKEY:
		if (GetRStickPos().y > 0.1f)
			return true;
		break;
	case KEYS::DOWNKEY:
		if (GetRStickPos().y < 0.1f)
			return true;
		break;
	}*/
	return false;
}

int ControllerC::GetScrollAmount()
{
	return input->GetScrollAmount();
}

int ControllerC::GetScrollAmtPerFrame()
{
	return input->GetScrollAmtPerFrame();
}
