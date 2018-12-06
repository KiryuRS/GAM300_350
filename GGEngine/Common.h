#pragma once

/*
	This class will include all of the common usage for the engine itself,
	which may include all possible defines, keyboard shortcuts, wrappers.

	Common.h ideally should not include any tool functions and is purely only
	for convenience usage
*/

/*
	Knowledge Section:
	C++17 List of Features:
	https://github.com/AnthonyCalandra/modern-cpp-features#selection-statements-with-initializer
	https://stackoverflow.com/questions/38060436/what-are-the-new-features-in-c17

	To determine if the current compiler can run the various c++17 features, please visit:
	https://en.cppreference.com/w/cpp/compiler_support
*/

static constexpr auto MAXBUFFER = 260U;

#define COREENGINE_S					CoreEngine::GetCoreEngine()
#define SYSTEMLOG_S						COREENGINE_S.GetSystemLog()
#define GFX_S							COREENGINE_S.GetGraphicsEngine()
#define WINDOW_S						COREENGINE_S.GetWindow()
#define AUDIO_S							COREENGINE_S.GetAudio()
#define PHYSICS_S						COREENGINE_S.GetPhysics()
#define INPUTMGR_S						COREENGINE_S.GetInputMgr()
#define INPUT_S							INPUTMGR_S->GetCurrentInput()
#define INPUT_CNTRL_S					INPUT_S.GetController()
#define FRAMECTR_S						COREENGINE_S.GetFrameRateController()
#define GSM_S							COREENGINE_S.GetGSM()
#define CLOCK_S							COREENGINE_S.GetClock()
#define DEBUGDRAW_S						COREENGINE_S.GetDebugDraw()
#define PROCESSES_S						COREENGINE_S.GetProcessWrapper()
#define EDITOR_S						COREENGINE_S.GetEditor()
#define WINDOW_S						COREENGINE_S.GetWindow()

#define DELTATIME						COREENGINE_S.GetFrameRateController().GetDeltaTime()
#define CAPFPS(boolean)					COREENGINE_S.LimitFPS(boolean)
#define QUITPROGRAM(checker)			GSM_S.SetExitProgram(checker)
#define TOSTRING(x)						#x
#define FUNCTIONNAME					__FUNCTION__
static constexpr float SCALINGFACTOR = 0.3f;
static constexpr float oneOverSixty = 1.0f / 60.0f;

#ifdef EDITOR
#define STARTDEVIL \
	DevIL::GetDevIL().Initialize();
#define DEVIL_S							DevIL::GetDevIL()
#define ENDDEVIL \
	DevIL::GetDevIL().DestroyDevIL();
#endif

// Different modes of printing (just some warnings below)
BETTER_ENUM(DebugTypes, char, DT_DEFAULT = 0, DT_ERROR, DT_WARNING, DT_SYSTEM, DT_SCRIPT, DT_ASSERTION)
// For Console Printing (without new line)
#define CONSOLE(STRING, ...)			SYSTEMLOG_S.PrintLog(false, DebugTypes::_from_string("DT_DEFAULT"), STRING, ##__VA_ARGS__)
// For Console Printing (with new line)
#define CONSOLE_LN(STRING, ...)			SYSTEMLOG_S.PrintLog(true, DebugTypes::_from_string("DT_DEFAULT"), STRING, ##__VA_ARGS__)
// For Error Console Printing (defaulted to new line)
#define CONSOLE_ERROR(STRING, ...)		SYSTEMLOG_S.PrintLog(true, DebugTypes::_from_string("DT_ERROR"), STRING, ##__VA_ARGS__)
// For System Console Printing (defaulted to new line)
#define CONSOLE_SYSTEM(STRING, ...)		SYSTEMLOG_S.PrintLog(true, DebugTypes::_from_string("DT_SYSTEM"), STRING, ##__VA_ARGS__)
// For Warning Console Printing (defaulted to new line)
#define CONSOLE_WARNING(STRING, ...)	SYSTEMLOG_S.PrintLog(true, DebugTypes::_from_string("DT_WARNING"), STRING, ##__VA_ARGS__)
// For Scripting Console Printing
#define CONSOLE_SCRIPT(STRING, ...)		SYSTEMLOG_S.PrintScriptLog(STRING, ##__VA_ARGS__)



// Directories
#define CONFIGFILE_DIRECTORY		"Resources/SaveFile/Config"
#define TEMPORARY_DIRECTORY			"Resources/SaveFile/Misc/Temp.cfg"
#define LOGGING_DIRECTORY			"Resources/Logs/"
#define EDITORCONFIG_DIRECTORY		"Resources/SaveFile/EditorConfig.cfg"
#define PREFAB_DIRECTORY			"Resources/SaveFile/Prefabs/"
#define MATERIAL_DIRECTORY			"CompiledAssets/Materials/"
//#define PREFAB_DIRECTORY			"CompiledAssets/Prefabs/"
#define LEVEL_DIRECTORY				"Resources/SaveFile/Levels/"
#define SCRIPT_DIRECTORY			"Resources/Scripts/"
#define TEMPLATE_DIRECTORY			"Resources/Templates/"

#define BROADCAST_MESSAGE			CoreEngine::GetCoreEngine().GetMessageSystem().Send
#define REGISTER_RECIPIENT			CoreEngine::GetCoreEngine().GetMessageSystem().Register
#define COMPONENTMAP				CoreEngine::GetCoreEngine().GetComponentMap()

#define SAVEFILETYPE				".ffs"
#define PREFAB_TYPE					".pfb"
#define SCRIPT_EXTENSION			".lua"
#define CUSTOM_IDE					"Resources/Templates/VSCode/Code.exe"

/* ========== INPUT START ========== */
// For more key codes: http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx

static void ActualShowCursor(bool toggle)
{
	// ShowCursor isn't a on or off. Its based on counter!

	/*
		Invisible : return value < 0
		Visible : return value >= 0
	*/
	INT ret = ShowCursor(toggle);
	/*
		Toggle = true means we want to display the mouse
		Toggle = false means we want to hide the mouse
	*/
	while (toggle ? (ret < 0) : (ret >= 0))
		ret = ShowCursor(toggle);
}

#define MAX_KEYS					255

#define KEY_LBUTTON					VK_LBUTTON
#define KEY_RBUTTON					VK_RBUTTON
#define KEY_MBUTTON					VK_MBUTTON

#define KEY_BACK					VK_BACK
#define KEY_TAB						VK_TAB
#define KEY_RETURN					VK_RETURN
#define KEY_LSHIFT					VK_LSHIFT
#define KEY_RSHIFT					VK_RSHIFT
#define KEY_LCTRL					VK_LCONTROL
#define KEY_RCTRL					VK_RCONTROL
#define KEY_LALT					VK_LMENU
#define KEY_RALT					VK_RMENU
#define KEY_PRINTSCREEN				VK_SNAPSHOT
#define KEY_SCROLLLOCK				VK_SCROLL
#define KEY_PAUSEBREAK				VK_PAUSE
#define KEY_CAPSLOCK				VK_CAPITAL
#define KEY_ESCAPE					VK_ESCAPE
#define KEY_SPACE					VK_SPACE
#define KEY_PAGEUP					VK_PRIOR
#define KEY_PAGEDOWN				VK_NEXT
#define KEY_END						VK_END
#define KEY_HOME					VK_HOME
#define KEY_INSERT					VK_INSERT
#define KEY_DELETE					VK_DELETE
#define KEY_LEFT					VK_LEFT
#define KEY_RIGHT					VK_RIGHT
#define KEY_DOWN					VK_DOWN
#define KEY_UP						VK_UP

#define KEY_0						0x30
#define KEY_1						0x31
#define KEY_2						0x32
#define KEY_3						0x33
#define KEY_4						0x34
#define KEY_5						0x35
#define KEY_6						0x36
#define KEY_7						0x37
#define KEY_8						0x38
#define KEY_9						0x39

#define KEY_A						0x41
#define KEY_B						0x42
#define KEY_C						0x43
#define KEY_D						0x44
#define KEY_E						0x45
#define KEY_F						0x46
#define KEY_G						0x47
#define KEY_H						0x48
#define KEY_I						0x49
#define KEY_J						0x4A
#define KEY_K						0x4B
#define KEY_L						0x4C
#define KEY_M						0x4D
#define KEY_N						0x4E
#define KEY_O						0x4F
#define KEY_P						0x50
#define KEY_Q						0x51
#define KEY_R						0x52
#define KEY_S						0x53
#define KEY_T						0x54
#define KEY_U						0x55
#define KEY_V						0x56
#define KEY_W						0x57
#define KEY_X						0x58
#define KEY_Y						0x59
#define KEY_Z						0x5A
#define KEY_F1						VK_F1
#define KEY_F2						VK_F2
#define KEY_F3						VK_F3
#define KEY_F4						VK_F4
#define KEY_F5						VK_F5
#define KEY_F6						VK_F6
#define KEY_F7						VK_F7
#define KEY_F8						VK_F8
#define KEY_F9						VK_F9
#define KEY_F10						VK_F10
#define KEY_F11						VK_F11
#define KEY_F12						VK_F12

#define KEY_NUMPAD0					VK_NUMPAD0
#define KEY_NUMPAD1					VK_NUMPAD1
#define KEY_NUMPAD2					VK_NUMPAD2
#define KEY_NUMPAD3					VK_NUMPAD3
#define KEY_NUMPAD4					VK_NUMPAD4
#define KEY_NUMPAD5					VK_NUMPAD5
#define KEY_NUMPAD6					VK_NUMPAD6
#define KEY_NUMPAD7					VK_NUMPAD7
#define KEY_NUMPAD8					VK_NUMPAD8
#define KEY_NUMPAD9					VK_NUMPAD9
#define KEY_NUM_MULTIPLY			VK_MULTIPLY
#define KEY_NUM_PLUS				VK_ADD
#define KEY_NUM_MINUS				VK_SUBTRACT
#define KEY_NUM_PERIOD				VK_DECIMAL
#define KEY_NUM_DIVIDE				VK_DIVIDE
#define KEY_NUMLOCK					VK_NUMLOCK

#define KEY_SEMICOLON				VK_OEM_1
#define KEY_BACKSLASH				VK_OEM_2
#define KEY_PLUS					VK_OEM_PLUS
#define KEY_COMMA					VK_OEM_COMMA
#define KEY_MINUS					VK_OEM_MINUS
#define KEY_PERIOD					VK_OEM_PERIOD
#define KEY_TILDE					VK_OEM_3
#define KEY_LBRACKET				VK_OEM_4
#define KEY_FRONTSLASH				VK_OEM_5
#define KEY_RBRACKET				VK_OEM_6
#define KEY_QUOTE					VK_OEM_7

// Gamepad Buttons (for array access)
enum GAMEPAD_BUTTON
{
	XINPUT_BUTTON_DPAD_UP = 0,
	XINPUT_BUTTON_DPAD_DOWN,
	XINPUT_BUTTON_DPAD_LEFT,
	XINPUT_BUTTON_DPAD_RIGHT,
	XINPUT_BUTTON_START,
	XINPUT_BUTTON_BACK,
	XINPUT_BUTTON_LEFT_THUMB,
	XINPUT_BUTTON_RIGHT_THUMB,
	XINPUT_BUTTON_LEFT_SHOULDER,
	XINPUT_BUTTON_RIGHT_SHOULDER,
	XINPUT_BUTTON_A = 12,
	XINPUT_BUTTON_B,
	XINPUT_BUTTON_X,
	XINPUT_BUTTON_Y,

	XINPUT_BUTTON_COUNT
};

enum GAMEPAD_TRIGGER
{
	XINPUT_TRIGGER_LEFT = 0,
	XINPUT_TRIGGER_RIGHT,

	XINPUT_TRIGGER_COUNT
};

enum GAMEPAD_STICK
{
	XINPUT_STICK_LEFT = 0,
	XINPUT_STICK_RIGHT,

	XINPUT_STICK_COUNT
};

enum GAMEPAD_STICKDIR
{
	XINPUT_STICKDIR_CENTER = 0,
	XINPUT_STICKDIR_UP,
	XINPUT_STICKDIR_DOWN,
	XINPUT_STICKDIR_LEFT,
	XINPUT_STICKDIR_RIGHT,

	XINPUT_STICKDIR_COUNT
};

/* ================================= */


extern unsigned int attachments[16];