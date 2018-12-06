#include "stdafx.h"
#pragma comment(lib, "Shlwapi.lib") //lib only needs to be included once. If using Shlwapi.h functions in other files, just include the header is enough

extern HWND mainHwnd;

Window::Window(_In_ HINSTANCE t_hInstance, _In_opt_ HINSTANCE t_hPrevInstance, _In_ LPWSTR t_lpCmdLine, _In_ int t_nCmdShow)
	: hInst(t_hInstance), hWnd{ },
	hPrevInstance(t_hPrevInstance),
	lpCmdLine(t_lpCmdLine),
	nCmdShow(t_nCmdShow), changes_made(false), in_focus(true),
	initialWindowSize(Vector2{ 1422 , 800 }),
	screenWidth((int)initialWindowSize.x), screenHeight((int)initialWindowSize.y), fullscreen(false), focused(true), quit(false), mouseWheel(0.f),
	allow_drag_drop{ true }
{
	// Only allow the drag and drop feature in editor mode
#ifndef EDITOR
	allow_drag_drop = false;
#endif
}

bool Window::Init()
{
	// Initialize global strings
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_GRAPHICSENGINE, szWindowClass, MAX_LOADSTRING);
	RegisterClassW(hInst);

	// Perform application initialization:
	if (!InitInstance(this, hInst))
	{
		return false;
	}

	hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(0));

	cursor = LoadCursorFromFileW(cursorpath);
	default_cursor = LoadCursor(NULL, IDC_ARROW);
	// Allow drag and drop of files
	DragAcceptFiles(hWnd, allow_drag_drop);

	return true;
}

ATOM Window::RegisterClassW(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMELOGO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GRAPHICSENGINE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GAMELOGO));

	return RegisterClassEx(&wcex);
}

int GetTaskBarHeight()
{
	RECT rect;
	HWND taskBar = FindWindow(SHELL_TRAYWND, NULL);
	if (taskBar && GetWindowRect(taskBar, &rect)) {
		return rect.bottom - rect.top;
	}
	return 0;
}


int GetTaskBarWidth()
{
	RECT rect;
	HWND taskBar = FindWindow(SHELL_TRAYWND, NULL);
	if (taskBar && GetWindowRect(taskBar, &rect)) {
		if (rect.bottom - rect.top > rect.right - rect.left)
			return rect.right - rect.left;
	}
	return 0;
}

BOOL Window::InitInstance(Window* window, HINSTANCE hInstance)
{
	hInst = hInstance; // Store instance handle in our global variable

						//Get the screen resolution
	int cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
	if (screenWidth >= cx || screenHeight >= cy)
	{
		screenWidth = cx;
		screenHeight = cy - GetSystemMetrics(SM_CXSIZEFRAME);
	}
	RECT rect = { 0, 0, (LONG)(screenWidth - 1), (LONG)(screenHeight - 1) };
	//The AdjustWindowRect computes the exact client area without the title bar and all the extra pixels
	AdjustWindowRectEx(&rect, dwStyle, FALSE, WS_EX_APPWINDOW);
	window->SetWindowHandle(CreateWindow(szWindowClass, szTitle, dwStyle,
		(cx - rect.right - GetTaskBarWidth()) / 2, (cy - rect.bottom - GetTaskBarHeight()) / 4, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, static_cast<LPVOID>(this)));
	
	if (!window->GetWindowHandle())
	{
		TOOLS::PrintError("Window", GetLastError());
		return false;
	}
	//SetWindowLong(window->GetWindowHandle(), GWL_STYLE, 0); //remove all window styles, check MSDN for details

	ShowWindow(window->GetWindowHandle(), nCmdShow);
	UpdateWindow(window->GetWindowHandle());

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* pWnd = &CoreEngine::GetCoreEngine().GetWindow();

	auto& editor = CoreEngine::GetCoreEngine().GetEditor();
	if (editor.get() && pWnd->focused)
	{
		editor->GetGuiManager().UpdateIOs(message, wParam, lParam);
	}

	switch (message)
	{
	case WM_LBUTTONDOWN:
		break;

	case WM_SETFOCUS:
		pWnd->focused = true;
		break;

	case WM_KILLFOCUS:
		pWnd->focused = false;
		// Once the window is off focus, we will set all of the INPUT_S' keys to false
		if (INPUTMGR_S.get())
			INPUT_S.ResetKeys();
		break;

	case WM_MOUSEWHEEL:
		if (!editor.get() || !ImGui::IsMouseHoveringAnyWindow())
		{
			std::lock_guard<std::mutex> lg(pWnd->mouseWheel_m);
			pWnd->mouseWheel += (float)(GET_WHEEL_DELTA_WPARAM(wParam) / 120);
		}
		break;

	case WM_CREATE:
	{
		break;
	}
	case WM_SYSKEYDOWN:
	{
		// Preventing freezing the game when alt key is pressed
		switch (wParam)
		{
			// Disabling the F10 key opening windows menu bar
		case VK_F10:
		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
			return 0;

		case VK_F4:
			// Checking for Alt-F4
			DWORD dwAltKeyMask{ 1 << 29 };
			if (lParam & dwAltKeyMask)
			{
				if (editor.get() && !editor->previouslySaved)
				{
					auto result = TOOLS::CreateYesNoMessageBox("Want to save your changes to the current file?");
					if (result)
					{
						pWnd->quit = true;
						if (result.value())
							BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVELEVELDEFAULT });
					}
				}
				else
					pWnd->quit = true;
			}
			return 0;
		}
		break;
	}
	case WM_SIZE:
	{
		pWnd->screenWidth = LOWORD(lParam);
		pWnd->screenHeight = HIWORD(lParam);
		// Inform Engine that the resolution has changed
		COREENGINE_S.ResolutionChanged(Vector2{ pWnd->screenWidth, pWnd->screenHeight });
		GetWindowRect(pWnd->hWnd, &(pWnd->rectWnd));
		GetClientRect(pWnd->hWnd, &(pWnd->rectClient));

		SendMessage(pWnd->status_handle, WM_SIZE, wParam, lParam);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UNREFERENCED_PARAMETER(hdc);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_ACTIVATEAPP :
	{
		auto ALT= std::bitset<16>(HIWORD(GetAsyncKeyState(VK_MENU)));
		auto TAB= std::bitset<16>(HIWORD(GetAsyncKeyState(VK_TAB)));
		auto CTRL = std::bitset<16>(HIWORD(GetAsyncKeyState(VK_CONTROL)));
		auto SHIFT = std::bitset<16>(HIWORD(GetAsyncKeyState(VK_SHIFT)));
		auto ESC = std::bitset<16>(HIWORD(GetAsyncKeyState(VK_ESCAPE)));

		if (ALT[0] && TAB[0] || CTRL[0] && SHIFT[0] && ESC[0])
			if (!IsIconic(hWnd))
			{
				ShowWindow(hWnd, SW_SHOWMINIMIZED);
				UpdateWindow(hWnd);
			}
			else
			{
				ShowWindow(hWnd, SW_SHOWDEFAULT);
				UpdateWindow(hWnd);
			}
		break;
	}
	case WM_SETCURSOR:
	{
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (pWnd->cursor != NULL)
			{
				SetCursor(pWnd->cursor);
			}
			else
			{
				SetCursor(pWnd->default_cursor);
			}
		}
		break;
	}
	case WM_DROPFILES:
	{
		// Dropping files into the window
		TCHAR lpszFile[MAX_PATH]{ };
		UINT uFile = 0;
		HDROP hDrop = HDROP(wParam);
		lpszFile[0] = '\0';
		uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
		std::vector<std::string> dropped_files{ uFile, std::string{ } };
		for (UINT i = 0; i != uFile; ++i)
			// Capture all drop files
			if (DragQueryFile(hDrop, i, lpszFile, MAX_PATH))
				dropped_files[i] = TOOLS::TCharToString(lpszFile);
		DragFinish(hDrop);
		// Send the vector of dropped files to the editor side
#ifdef EDITOR
		editor->GrabDroppedFiles(dropped_files);
#endif
		break;
	}
	case WM_CLOSE:
	{
		if (editor.get() && !editor->previouslySaved)
		{
			auto result = TOOLS::CreateYesNoMessageBox("Want to save your changes to the current file?");
			if (result)
			{
				pWnd->quit = true;
				if (result.value())
					BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVELEVELDEFAULT });
			}
		}
		else
			pWnd->quit = true;
		break;
	}
  
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool Window::PollEvents()
{
	return !quit;
}

void Window::Update()
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)               // Have We Received A Quit Message?
		{
			Shutdown();
		}
		else                            // If Not, Deal With Window Messages
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
	}

	// Determine the cursor position in the client window
	//const Vector2& mouse_game{ INPUT_S.mouse.windowMousePos };
	//bool default_mouse = abs(mouse_game.x) >= 1.f || abs(mouse_game.y) >= 1.02f;
	//// Determine if the window is our main
	//if (Wnd == GetAncestor(Wnd, GA_ROOT) && displayCursor && !default_mouse)
	//	ShowCursor(displayCursor = false);
	//else if (default_mouse && !displayCursor)
	//	ShowCursor(displayCursor = true);

  if (INPUT_S.GetToggleKey(KEY_F11))
  {
    FullScreenSwitch();
  }

}

void Window::Shutdown()
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}
}

int Window::GetExitParam()
{
  return (int)msg.wParam;
}

void Window::SetWindowHandle(HWND h)
{
  hWnd = h;
}

HWND Window::GetWindowHandle()
{
	return hWnd;
}

void Window::InstantiateStatusBar()
{
	//Creates the status bar
	//Hides the status bar
	ShowWindow(status_handle, SW_HIDE);
}

void Window::AddKeyCallback(std::function<void(int, int, int, int)> func)
{
  keycallbacks.push_back(func);
}


bool Window::MapToClient(POINT * p) const
{
	// Value that ScreenToClient return is non-zero if the function succceeds
	return !(ScreenToClient(hWnd, p) == 0);
}


void Window::FullScreenSwitch()
{
  if (isFullScreen) /// to switch to window next
  {
    /// switch to window mode
    /// set back the style and position
    SetWindowLong(hWnd, GWL_STYLE, windowStyle);
    SetWindowLong(hWnd, GWL_EXSTYLE, windowStyleEx);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    SetWindowPlacement(hWnd, &windowPlace);
  }
  else
  {
    /// switch to fullScreen
    /// get the windowplacement and store it for future use 
    GetWindowPlacement(hWnd, &windowPlace);
    if (windowStyle == 0)
      windowStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (windowStyleEx == 0)
      windowStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);

    /// set the new window data and store it for future use 
    LONG fullScreenStyle = windowStyle;
    fullScreenStyle &= ~WS_BORDER;
    fullScreenStyle &= ~WS_DLGFRAME;
    fullScreenStyle &= ~WS_THICKFRAME;

    LONG fullScreenStyleEx = windowStyleEx;
    fullScreenStyleEx &= ~WS_EX_WINDOWEDGE;


    /// set the new window data
    SetWindowLong(hWnd, GWL_STYLE, fullScreenStyle | WS_POPUP);
    SetWindowLong(hWnd, GWL_EXSTYLE, fullScreenStyleEx | WS_EX_TOPMOST);
    ShowWindow(hWnd, SW_SHOWMAXIMIZED);

  }
  /// switch window and fullscreen
  isFullScreen = !isFullScreen;

}