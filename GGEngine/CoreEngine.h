#pragma once
#include "Window.h"
#include "Broadcaster.h"
#include "Clock.h"
#include "ResourceUpdater.h"
#include "Compiler.h"

class Editor;
class GuiManager;
class GraphicsEngine;
class Physics;
struct ComponentMap;
class FrameRateController;
class SystemLog;
struct EditorMessage;
class AudioSystem;
struct EventMap;
class GameStateManager;
class GameMessages;
class DebugDraw;
struct GraphicsMessage;
class Scheduler;
class ProcessesWrapper;
class EntityList;

class CoreEngine
{
	static std::unique_ptr<CoreEngine> coreEngine;
	std::vector<CoreSystem*> all_sys;
	friend class PerformanceWindow;
	friend class ResourceWindow;

	// All our main systems
	std::unique_ptr<Window>					window;
	std::unique_ptr<GraphicsEngine>			graphics;
	std::unique_ptr<InputManager>			inputMgr;
	std::unique_ptr<Editor>					editor;
	std::unique_ptr<Physics>				physics;
	std::unique_ptr<FrameRateController>	frameCtr;
	std::unique_ptr<ComponentMap>			componentMap;
	std::unique_ptr<EventMap>				eventMap;
	std::unique_ptr<SystemLog>				log;
	std::unique_ptr<AudioSystem>			audio;
	std::unique_ptr<GameStateManager>		gsm;
	std::unique_ptr<DebugDraw>				debugDraw;
	std::unique_ptr<ResourceUpdater>		resourceLoader;
	std::unique_ptr<Scheduler>				scheduler;
	std::unique_ptr<Compiler>				compiler;
	std::unique_ptr<ProcessesWrapper>		allProcesses;

	token editorToken;

	// Our station for talking to these main systems
	MultiChannelBroadcaster
	<EditorMessage, GameMessages, GraphicsMessage, CompilerMessage>
	messageSystem;

	// All extra variables
	HWND mainHwnd;
	bool consoleExists, debugDrawActive;
	float pre_update_time, debug_draw_time;
	Clock clock;
	std::atomic<bool> window_freezed;

public:
	//TODO remove during holidays
	std::vector<EntityList*> gamespaces;
	static CoreEngine & GetCoreEngine();
	CoreEngine();

	template<typename ... Args>
	bool GenerateWindow(Args&&... args)
	{
		window = std::make_unique<Window>(std::forward<Args>(args)...);
		if (!window->Init())
			return false;
		mainHwnd = window->GetWindowHandle();
		// window->InstantiateStatusBar();
		return true;
	}
	void ResolutionChanged(const Vector2& dimension);
	void CreateEditor();
	void CreateDebugConsole(const std::string& title);
	void SetupEngine();
	bool Update();
	int ShutDown();
	void DestroyConsole();
	void LimitFPS(bool toggle);
	bool IsFPSCapped() const;
	inline void SetupGamespaces(const std::vector<EntityList*>& list) { gamespaces = list; }
	inline void SetFreezeToggle(bool toggle)			{ window_freezed = toggle; }
	inline bool IsWindowFreezed() const					{ return window_freezed; }
	inline bool GetDebugDrawActive() const				{ return debugDrawActive; }
	inline void SetDebugDrawActive(bool active)			{ debugDrawActive = active; }
	inline auto& GetComponentMap()	const				{ return *componentMap; }
	inline auto& GetEventMap()	const					{ return *eventMap; }
	inline auto& GetMessageSystem()						{ return messageSystem; }
	inline auto& GetPhysics() const						{ return *physics; }
	inline SystemLog& GetSystemLog() const				{ return *log; }
	inline GraphicsEngine& GetGraphicsEngine() const	{ return *graphics; }
	inline auto& GetGSM() const							{ return *gsm; }
	inline auto& GetWindow() const						{ return *window; }
	inline auto& GetEditor() const						{ return editor; }
	inline auto& GetInputMgr() const					{ return inputMgr; }
	inline auto& GetAudio() const						{ return *audio; }
	inline auto& GetFrameRateController() const			{ return *frameCtr; }
	inline auto& GetClock()								{ return clock; }
	inline auto& GetDebugDraw() const					{ return *debugDraw; }
	inline auto& GetProcessWrapper() const				{ return *allProcesses; }
};

