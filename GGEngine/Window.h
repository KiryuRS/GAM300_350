#pragma once
#include <atomic>
#include <mutex>

#define MAX_LOADSTRING 100
using CALLBACK_VECTOR = std::vector<std::function<void(int, int, int, int)>>;

class Window;


LRESULT CALLBACK WndProc(HWND hwnd, UINT uint, WPARAM wParam, LPARAM lParam);
static void error_callback(int error, const char* description);

class Window
{
public:
	const Vector2 initialWindowSize;
private:
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HINSTANCE hInst;                                // current instance
	TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
	CALLBACK_VECTOR keycallbacks;

	ATOM RegisterClassW(HINSTANCE hInstance);
	BOOL InitInstance(Window* window, HINSTANCE hInstance);
	INT_PTR CALLBACK About(HWND hwnd, UINT uint, WPARAM wParam, LPARAM lParam);
	HINSTANCE hPrevInstance;
	LPWSTR lpCmdLine;
	int nCmdShow;
	HMENU ghMenu;
	HWND status_handle;
	HWND hDlg;
	HCURSOR cursor;
	HCURSOR default_cursor;
	RECT rectWnd, rectClient;
	bool changes_made;
	bool allow_drag_drop;
	std::atomic<bool> in_focus;
	HACCEL hAccelTable;
	HWND hWnd;
	MSG msg;
	LPCWSTR cursorpath = { L"Resources\\mouse.cur" };
	std::atomic<int> screenWidth, screenHeight;
	std::atomic<bool> fullscreen, focused, quit;
	float mouseWheel;
	std::mutex mouseWheel_m;


  /// fullscreenValue
  WINDOWPLACEMENT windowPlace;
  LONG windowStyle{ 0 };
  LONG windowStyleEx{ 0 };
  BOOL isFullScreen{ false };

public:
	Window(_In_ HINSTANCE t_hInstance, _In_opt_ HINSTANCE t_hPrevInstance, _In_ LPWSTR t_lpCmdLine, _In_ int t_nCmdShow);
  
	bool Init();
	// check for events, if event is quit return false so the main loop will end
	bool PollEvents();
	void Update();
	int GetExitParam();
	int GetScreenWidth() const						{ return screenWidth; }
	int GetScreenHeight() const						{ return screenHeight; }
	auto GetScreenSize() const						{ return Vector2(screenWidth, screenHeight); }
	void SetWindowHandle(HWND h);
	HWND GetWindowHandle();
	void InstantiateStatusBar();
	void AddKeyCallback(std::function<void(int, int, int, int)> func);
	bool IsFocused() const { return focused; }
	bool MapToClient(POINT * p) const;
	float GetScrollAmount()							{ std::lock_guard<std::mutex> lg(mouseWheel_m); return mouseWheel; }
	RECT GetRectWnd() const							{ return rectWnd; }
	RECT GetRectClient() const						{ return rectClient; }
	void Exit()										{ quit = true; }
	void Shutdown();
  void FullScreenSwitch();
};

#define WIDE_(exp) L##exp
#define WIDEN(exp)  WIDE_(exp)
#define _SHELL_TRAYWND "Shell_traywnd"
#define _W__SHELL_TRAYWND WIDEN(_SHELL_TRAYWND)
#define _FILE_FORMAT_SUPPORTED "File formats supported: "
#define _W_FILE_FORMAT_SUPPORTED WIDEN(_FILE_FORMAT_SUPPORTED)
#define _FILE_FORMAT_NOT_SUPPORTED "File format not supported"
#define _W_FILE_FORMAT_NOT_SUPPORTED WIDEN(_FILE_FORMAT_NOT_SUPPORTED)
#define _DOT "."
#define _W_DOT WIDEN(_DOT)
#define _AND " and "
#define _W_AND WIDEN(_AND)
#define _COMMA_AND_SPACE ", "
#define _W_COMMA WIDEN(_COMMA_AND_SPACE)
#define _EXIT_WITHOUT_SAVING "Exit without saving?"
#define _W_EXIT_WITHOUT_SAVING WIDEN(_EXIT_WITHOUT_SAVING)
#define _TEAM_NAME "Team GG"
#define _W_TEAM_NAME WIDEN(_TEAM_NAME)

#ifdef UNICODE
#define SHELL_TRAYWND _W__SHELL_TRAYWND
#define FILE_FORMAT_SUPPORTED _W_FILE_FORMAT_SUPPORTED
#define DOT _W_DOT
#define AND _W_AND
#define COMMA _W_COMMA
#define FILE_FORMAT_NOT_SUPPORTED _W_FILE_FORMAT_NOT_SUPPORTED
#define EXIT_WITHOUT_SAVING _W_EXIT_WITHOUT_SAVING
#define TEAM_NAME _W_TEAM_NAME
#else
#define SHELL_TRAYWND _SHELL_TRAYWND
#define FILE_FORMAT_SUPPORTED _FILE_FORMAT_SUPPORTED
#define DOT _DOT
#define AND _AND
#define COMMA _COMMA_AND_SPACE
#define FILE_FORMAT_NOT_SUPPORTED _FILE_FORMAT_NOT_SUPPORTED
#define EXIT_WITHOUT_SAVING _EXIT_WITHOUT_SAVING
#define TEAM_NAME _TEAM_NAME
#endif
