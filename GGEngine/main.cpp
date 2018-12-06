#pragma once

#include "stdafx.h"
#include "CoreEngine.h"
// Visual Leak Detector only in DebugMode
// #define _MEMLEAKCHECKS
#if defined _DEBUG && defined _MEMLEAKCHECKS
#include <vld.h>
#pragma comment(lib, "vld.lib")
#endif

#if defined _WIN64
#pragma comment(lib, "Shlwapi.lib")
// Include the relevant library for different versions
#if (_MSC_VER >= 1915)
#if defined _DEBUG
#pragma comment(lib, "lua_64.lib")
#else
#pragma comment(lib, "lua_64.lib")
#endif
#elif defined _DEBUG
// Only for debug mode
#pragma comment(lib, "Luax64-d_school.lib")
#else
// Only for release mode
#pragma comment(lib, "luax64_school.lib")
#endif 
#endif

// @TODO: Comment this when launching for deployment
#define RELEASECONSOLE


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// Create our core engine for display
	auto& coreEngine = CoreEngine::GetCoreEngine();

	// Create our debugging console if we are in debug mode
#if defined EDITOR || defined _DEBUG
#if defined _DEBUG && defined EDITOR
	std::string console_name{ "Editor-Debug Console" };
#elif defined _DEBUG
	std::string console_name{ "Debug Console" };
#elif defined EDITOR
	std::string console_name{ "Editor-Release Console" };

#endif
	coreEngine.CreateDebugConsole(console_name);
#elif defined RELEASECONSOLE
	std::string console_name{ "Release-Console [REMOVE DURING DEPLOYMENT] !" };
	coreEngine.CreateDebugConsole(console_name);
#endif

#ifndef _SYSTEMTESTS
	// Create our window using the core engine
	coreEngine.GenerateWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	// Setup the engine using the created window
	coreEngine.SetupEngine();
	// Create the editor if in EditorMode
#ifdef EDITOR
	coreEngine.CreateEditor();
#endif
	// Running the core engine
	while (coreEngine.Update());
#endif

	return coreEngine.ShutDown();

}