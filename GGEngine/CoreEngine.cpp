#include "stdafx.h"
#include "FrameRate.h"
#include "ScriptC.h"
#include "TextureLoader.h"
#include "StaticMeshRendererC.h"
#include "SkeletalMeshRendererC.h"
#include "DebugDraw.h"
#include "ResourceUpdater.h"
#include "Scheduler.h"
#include "ExecutableProcess.h"

CoreEngine::CoreEngine():
	mainHwnd{ nullptr }, 
	consoleExists{ false },
	debugDrawActive{ true },
	window_freezed{ false }
{ }


void CoreEngine::ResolutionChanged(const Vector2& dimension)
{
	if (graphics.get())
		graphics->ChangeScreenResolution((int)dimension.x, (int)dimension.y);
#if EDITOR
	// Should only update when we change our resolution, alt-tab will cause the dimension to end up with 0,0
	// and that causes the gui windows on our editor to not display anything
	if (editor.get() && dimension != Vector2{})
		editor->ResolutionChanged(dimension);
#endif
}

void CoreEngine::CreateEditor()
{
	resourceLoader = std::make_unique<ResourceUpdater>();
	editor = std::make_unique<Editor>(*window);
	if (!graphics.get())
	{
		TOOLS::CreateErrorMessageBox("Graphics are not initialized!");
	}
	graphics->SetCamera(editor->camera);
	physics->Initialize(*editor->entityList);
	resourceLoader->running = true;
#if EDITOR
	compiler = std::make_unique<Compiler>();
	scheduler->QueueThread([&]()->void {compiler->Update(); });
	scheduler->QueueThread([&]()->void {resourceLoader->Update(); });
#endif
}

void CoreEngine::CreateDebugConsole(const std::string & title)
{
	consoleExists = true;
	AllocConsole();
	FILE * stream = nullptr;
	//freopen_s(&stream, "conin$", "r", stdin);
	freopen_s(&stream, "conout$", "w", stdin);
	freopen_s(&stream, "conout$", "w", stdout);
	SetConsoleTitle(TOOLS::StringToWString(title).c_str());
	std::cout << "========== Game Engine Debugging Console ==========\n";
}

void CoreEngine::SetupEngine()
{
	// Create instances of our systems
	log = std::make_unique<SystemLog>();
	// Setup the multithreading section FIRST
	scheduler = std::make_unique<Scheduler>(10);
	componentMap = std::make_unique<ComponentMap>();
	componentMap->AddToMap<COMPONENTTYPES>();
	eventMap = std::make_unique<EventMap>();
	eventMap->AddToMap<EVENTTYPES>();
  graphics = std::make_unique<GraphicsEngine>(mainHwnd);
  graphics->Init();
	// Setting up the GSM because Graphics side would need to know the number of gamespaces during a level setup
	gsm = std::make_unique<GameStateManager>();
	gsm->Init();
	inputMgr = std::make_unique<InputManager>(*window);
	physics = std::make_unique<Physics>();
	frameCtr = std::make_unique<FrameRateController>();
	audio = std::make_unique<AudioSystem>();
	audio->Init();
	debugDraw = std::make_unique<DebugDraw>();
	allProcesses = std::make_unique<ProcessesWrapper>();

	// Messed up Hardcoding :(
	for (unsigned i = 0; i != 5; ++i)
		all_sys.emplace_back(nullptr);
	all_sys[0] = graphics.get();
	all_sys[1] = inputMgr.get();
	all_sys[2] = physics.get();
	all_sys[3] = gsm.get();
	all_sys[4] = audio.get();
	
}

bool CoreEngine::Update()
{
	if (!window->PollEvents() || !gsm->IsRunning())
		return false;

	// Start of game loop
	frameCtr->SystemStartFrame();

	graphics->GraphicsFrameStart();
	allProcesses->Update();

	// "Pre-Updates"
#if defined EDITOR
	clock.StartStopwatch(); 
	gsm->HandleStateChanges();
	window->Update();
	clock.StopStopwatch();
	pre_update_time = clock.GetStopwatchTime();
	clock.ClearStopwatch();
#else
	gsm->HandleStateChanges();
	window->Update();
#endif

	// Actual Updating
	inputMgr->SelfUpdate();
	physics->SelfUpdate();
	audio->SelfUpdate();
	gsm->SelfUpdate();

	// "Post-Updates" & Debug Drawing
#ifdef EDITOR
	clock.StartStopwatch();
	if (debugDrawActive) debugDraw->Draw();
	clock.StopStopwatch();
	debug_draw_time = clock.GetStopwatchTime();
	clock.ClearStopwatch();
	editor->Update(*window, frameCtr->GetDeltaTime());
#endif

	graphics->SelfUpdate();
	graphics->GFXPostProcessing();

#ifdef EDITOR
	editor->Render();
#endif

	// End of game loop
	graphics->GraphicsFrameEnd();
	frameCtr->SystemEndFrame();
	return true;
}

int CoreEngine::ShutDown()
{
	if(consoleExists)
		DestroyConsole();
	int exitcode = window->GetExitParam();
	allProcesses->Shutdown();
	window->Shutdown();
	graphics->Exit(); // clear graphics engine states
	gsm->Cleanup();

#ifdef EDITOR
	resourceLoader->running = false;
	compiler->running = false;
#endif
	return exitcode;
}

void CoreEngine::DestroyConsole()
{
	if (!FreeConsole())
		MessageBox(NULL, L"Failed to free the console!", NULL, MB_ICONEXCLAMATION);
}

void CoreEngine::LimitFPS(bool toggle)
{
	frameCtr->CapFrameRate(toggle);
}

bool CoreEngine::IsFPSCapped() const
{
	return frameCtr->IsFrameRateCapped();
}

// initialize the model (static)
std::unique_ptr<CoreEngine> CoreEngine::coreEngine;

CoreEngine& CoreEngine::GetCoreEngine()
{
	if (!coreEngine.get())
		coreEngine = std::make_unique<CoreEngine>();
	return *coreEngine;
}
