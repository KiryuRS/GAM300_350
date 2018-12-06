#pragma once

/*
// Pre-compiled Header Information

https://blogs.msdn.microsoft.com/vcblog/2016/10/26/recommendations-to-speed-c-builds-in-visual-studio/
*/

#include <codeanalysis/Warnings.h>

// Warning disables (for all of the external libraries used)
#pragma warning (disable : 6387)
#pragma warning (disable : 26495)
#pragma warning (disable : 26439)
#pragma warning (disable : 28182)
#pragma warning (disable : 26451)
#pragma warning (disable : 6011)
#pragma warning (disable : 6255)
#pragma warning (disable : 6031)
#pragma warning (disable : 6385)
#pragma warning (disable : 26434)
#pragma warning (disable : 26498)
#pragma warning (disable : 28020)
#pragma warning (disable : 26444)
#pragma warning (disable : 28159)
#pragma warning (disable : 6388)
#pragma warning (disable : 6054)
#pragma warning (disable : ALL_CODE_ANALYSIS_WARNINGS)

// All includes for C++ Libraries and Windows includes goes here
#define FULLC17			_MSC_VER >= 1914	// Full C++17 supports is available on at least MSVC 19.14
#define WIN32_LEAN_AND_MEAN					// not required but just in case
#include <Windows.h>
#include <tchar.h>
#include <memory>							// std::unique_ptr
#include <typeinfo>							// typeid
#include <utility>
#include <filesystem>						// std::c++17 filesystem (if not available use std::experimental::filesystem from c++14)
#if FULLC17
namespace fs = std::filesystem;				// Naming convention sake since its super long
#else
namespace fs = std::experimental::filesystem;
#endif
#include <iostream>							// std::cerr, std::cout
#include <chrono>							// std::chrono
#include <functional>						// std::function
#include <cstdio>
#include <algorithm>						// std::find, std::find_if
#include <cstdlib>							
#include <shellapi.h>						// DragQueryFile, DragFinish
#include <bitset>							// std::bitset
#include <type_traits>						// typeid, std::index_sequence, std::index_sequence_for
#include <typeindex>						// std::type_index
#include <optional>							// std::optional
#include <string>							// std::string
#include <tuple>							// std::tuple
#include <string>							// std::string


// C++ Containers (might be doing our own custom data structures in here)
#include <vector>							// std::vector
#include <array>							// std::array
#include <stack>							// std::stack
#include <queue>							// std::queue
#include <forward_list>						// std::forward_list


// ============================== Custom includes (engine side) goes here =======================================
#include "CustomException.h"
#include "BetterEnums.h"
#include "Common.h"
#include "Tools.h"
#include "Clock.h"
#include "Timer.h"
#include "CoreSystem.h"
#include "FilesChecker.h"
#include "AudioSystem.h"
#include "Math.h"
#include "Vector.h"
#include "Input.h"
#include "InputManager.h"
#include "WindowHolder.h"
#include "GuiWindow.h"
#include "Serializable.h"
#include "Component.h"
#include "CoreEngine.h"
#include "GraphicsEngine.h"
#include "GuiManager.h"
#include "Editor.h"
#include "Physics.h"
#include "Window.h"
#include "Resource.h"
#include "CoreEngine.h"
#include "GuiManager.h"
#include "Entity.h"
#include "OpenFileDialog.h"
#include "ComponentDirectory.h"
#include "GameSettings.h"
#include "Editor.h"
#include "GuiManager.h"
#include "ComponentWindow.h"
#include "HelpWindow.h"
#include "ConsoleWindow.h"
#include "PerformanceWindow.h"
#include "HistoryWindow.h"
#include "SharedOptions.h"
#include "CollisionMatrix.h"
#include "ObjectHierarchyWindow.h"
#include "CoreEngine.h"
#include "FrameRate.h"
#include "SystemLog.h"
#include "EntityList.h"
#include "MetaBoilerplates.h"
#include "EntityWrapper.h"
#include "DataStructWrapper.h"
#include "LuaScript.h"
#include "GameStateManager.h"
#include "Event.h"
#include "Entity.h"
#include "Controller.h"
#include "ThreadsafeQueue.h"
#include "DebugDraw.h"
#include "Scheduler.h"
#include "Serializer.h"
#include "Compiler.h"
#ifdef EDITOR
#include "DevIL.h"
#endif

// =============================================== Game Components goes here =====================================
#include "ButtonC.h"
#include "ButtonManagerC.h"
#include "BaseMeshC.h"
#include "ComponentIdentifier.h"
#include "CameraC.h"
#include "ScriptC.h"
#include "TransformC.h"
#include "ControllerC.h"
#include "PointLightC.h"
#include "DirectionalLightC.h"
#include "SpotLightC.h"
#include "SoundC.h"
#include "StaticMeshC.h"
#include "SkeletalMeshC.h"
#include "PlayerC.h"
#include "TextureLoader.h"
#include "SkeletalMesh.h"
#include "2DSpriteC.h"


// =============================================== This should be placed last ====================================
#include "ExecutableProcess.h"
#include "Variant.h"					// Variant class (note that any UDT-supported-Variant should be declared before including Variant.h)
#include "GameShared.h"
#include "GameScene.h"
#include "GameSpaces.h"

