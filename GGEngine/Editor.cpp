#include "stdafx.h"
#include "ToolsWindow.h"
#include "ext.hpp"
#include "ResourceWindow.h"
#include "serializer.h"
#include "GraphicsWindow.h"
#include "CameraOptions.h"
#include "MaterialWindow.h"
#include "GameSpaces.h"
#ifdef EDITOR
#include <il.h>
#endif

Editor::Editor(const Window& window):
	guiManager(std::make_unique<GuiManager>(window)), camera{ },
	windows(*this), settingsWindow(nullptr), objectHiWindow{ nullptr }, componentWindow{ nullptr },
	previouslySaved(true), playing(false), paused(false), ingameCamera{ nullptr },
	messageQueue(std::make_unique<ThreadsafeQueue<EditorMessage>>()),
	newScriptBuffer{ "New Script" }, current_operation{ ImGuizmo::TRANSLATE },
	current_mode{ ImGuizmo::LOCAL }, enable_gizmos{ true }, multipleGamespaces{ false },
	editorScene{ }, entityList{ nullptr }, mainLayerNum{ 0 }
{
	// Setup the scene
	editorScene.emplace_back();
	editorScene[0].gamespace = std::make_unique<GameSpace>(nullptr, "Editor", 0);
	editorScene[0].gamespace->entity_list.AssignID(0);
	editorScene[0].sceneFile = TEMPORARY_DIRECTORY;
	entityList = &editorScene[0].gamespace->entity_list;

	game_input.name = "GAME INPUT";
	editor_input.name = "EDITOR INPUT";
	AddSerializable("Current Level", currentLevel);
	AddSerializable("Camera Speed Multiplier", cameraSpeedMultiplier);
	AddSerializable("Gamespaces View", multipleGamespaces);
	AddSerializable("Editor Spaces Size", spacesSize);
	AddSerializable("Main Layer Number", mainLayerNum);
	// Capture all of the levels
	std::string full_path{ fs::current_path().generic_string() };
	full_path += "/";
	full_path += LEVEL_DIRECTORY;
	for (auto& path : fs::recursive_directory_iterator(full_path))
	{
		const std::string& currentPath = path.path().string();
		// Negate out all of the non-level files
		std::string extension = TOOLS::GetExtension(currentPath);
		if (extension != SAVEFILETYPE)
			continue;
		// Retrieve only the levelFile name
		size_t startPos = currentPath.rfind("/") + 1;
		size_t extensionPos = currentPath.rfind(".");
		std::string levelFile = currentPath.substr(startPos, extensionPos - startPos);
		// Add it into our map
		auto& camInfo = allLevels[levelFile];
		
		std::string serializeInfo{ levelFile + "_ECI" };
		AddSerializable(serializeInfo + "_POS", camInfo.position);
		AddSerializable(serializeInfo + "_TAR", camInfo.target);
	}
	
	CONSOLE_SYSTEM("ImGui Version: ", ImGui::GetVersion());
	guiManager->LoadStyle();

	// "New Frame" in this section to get the bar size 
	ImGui::NewFrame();
	ImGui::BeginMainMenuBar();
	windows.bar_size = ImGui::GetWindowSize().y;
	ImGui::EndMainMenuBar();
	ImGui::EndFrame();

	// Top Middle Window
	auto* tools = windows.AddWindow< ToolsWindow >("Window_Tools", Vector2{ 1.f, 0.045f });
	tools->flags |= ImGuiWindowFlags_NoScrollbar;
	tools->editor_flags |= EditorImGuiFlags_PanTop | EditorImGuiFlags_NoYRescale;
	// Left Window
	auto* settings = windows.AddWindow< ObjectHierarchyWindow, GameSettings, SharedOptions >("Window_Settings", Vector2{ 0.2f, 0.69f }, Vector2{}, tools);
	settings->editor_flags |= EditorImGuiFlags_PanLeft | EditorImGuiFlags_PanBelowReference;
	settingsWindow = static_cast<GameSettings*>(settings->windows[1]);
	objectHiWindow = static_cast<ObjectHierarchyWindow*>(settings->windows[0]);
	// Bottom Window
	auto* console = windows.AddWindow< ResourceWindow, ConsoleWindow, HelpWindow >("Window_Console", { 0.8f, 0.0f }, Vector2{}, settings);
	console->editor_flags |= EditorImGuiFlags_PanLeft | EditorImGuiFlags_PanBelowReference | EditorImGuiFlags_PanAutoSize;
	// Right Window
	auto* modifiable = windows.AddWindow< ComponentWindow, GraphicsWindow,
										  CollisionMatrix, PerformanceWindow, HistoryWindow, MaterialWindow >
	("Window_Modifiable", { 0.2f, 0.0f }, Vector2{}, tools);
	modifiable->editor_flags |= EditorImGuiFlags_PanRight | EditorImGuiFlags_PanBelowReference | EditorImGuiFlags_PanAutoSize;
	// Near Right Window
	auto* camoptions = windows.AddWindow< CameraOptions >("Window_Camera", { 0.095f, 0.105f }, Vector2{}, modifiable);
	camoptions->editor_flags |= EditorImGuiFlags_PanLeftReference | EditorImGuiFlags_MatchReferenceYPos | EditorImGuiFlags_TransparentWindow | EditorImGuiFlags_NoYRescale | EditorImGuiFlags_NoXRescale;
	componentWindow = static_cast<ComponentWindow*>(modifiable->windows[0]);

	LoadConfig();
	settingsWindow->Startup(*this);
	PHYSICS_S.PhysicsPause(true);
	INPUTMGR_S->SetInput(editor_input);
#ifdef EDITOR
	// Print out the version
	CONSOLE_SYSTEM("DevIL's version: ", (unsigned int)ilGetInteger(IL_VERSION_NUM));
#endif
	// Initialize our gui windows
	windows.Initialize();
}

void Editor::ResolutionChanged(const Vector2 &)
{
	// Update all our GUI windows to reflect on the new changes
	auto& all_general_windows = windows.GetAllGeneralWindows();
	for (auto& window : all_general_windows)
		window->UpdateResolution();
}

void Editor::Update(Window& window, float dt)
{
	if (INPUT_S.GetReleaseKey(KEY_F1))
	{
		windows.ToggleDisplay();
		// enable_gizmos = !enable_gizmos;
	}

	if (INPUT_S.GetToggleKey(KEY_ESCAPE) && playing)
		BROADCAST_MESSAGE( EditorMessage{ EDITORMSG::PLAYBTN });
	if (INPUT_S.GetToggleKey(KEY_F2) && playing && !paused)
	{ 
		mouseSettings.enable = !mouseSettings.enable;
		// Enable mouse usage
		if (!mouseSettings.enable)
		{
			game_input.ShowMouse(true);
			game_input.LockMouseIntoCenter(false);
		}
		else
		{
			game_input.ShowMouse(mouseSettings.display);
			game_input.LockMouseIntoCenter(mouseSettings.locked);
		}
	}

	guiManager->Update(INPUT_S, window, dt);
	ImGuizmo::Enable(enable_gizmos);
	ImGuiIO& io = ImGui::GetIO();
	if (enable_gizmos)
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	MenuBar(window);
	windows.Display();

	//ImGui::ShowStyleEditor(&editorStyle);
	if (playing)
	{
		/*
			Different modes of play:
			1) Casual playing of scenes without pause
			2) Scene is paused but "NEXT FRAME" is toggled
		*/
		if (!paused)
		{
			// Update the audio's listening position
			AUDIO_S.SetListenerPosition(ingameCamera->GetPosition());
			UpdateTestPlay(DELTATIME);
		}
		else
		{
			// Paused state here
			UpdateMovement();
			AUDIO_S.SetListenerPosition(ingameCamera->GetPosition());

			// If the next frame is toggled, we will only want to update them after one frame
			if (sceneNextFrame && FRAMECTR_S.OneFramePassed())
			{
				// Skip for N frames (frame_skip)
				if (frameSkipCounter++ < frame_skips)
				{
					// Update everything for one frame
					UpdateTestPlay(1.f/60.f);
				}
				else
				{
					UpdateTestPlay(0);
					// Pause everything back
					AUDIO_S.PauseAll();
					PHYSICS_S.PhysicsPause(true);
					sceneNextFrame = false;
					frameSkipCounter = 0;
				}
			}
			else if (!sceneNextFrame)
			{
				// Only update the meshes
				UpdateTestPlay(0);
			}
		}
	}
	else
	{
		// Uses the default editor's movement updating
		UpdateMovement();
		// Update all the entities
		entityList->EditorUpdate(dt);
		entityList->Render();
		auto cam = entityList->GetActiveCamera();
		if (cam)
			camera.CopySettings(*cam->GetCamera());
	}

	HandleMessages();

	
}

void Editor::MenuBar(Window& window)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		ImGui::TextDisabled("Entities & Scenes");
		if (ImGui::MenuItem("New Scene", "CTRL N"))
		{
			// Check if the current scene has some modifications yet to be saved
			if (!previouslySaved)
			{
				std::optional<bool> result = TOOLS::CreateYesNoMessageBox("You have made changes to the current scene!\nDo you want to save your changes to the current file?");
				if (result.value())
					BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVELEVELDEFAULT });
			}
			entityList->Clear();
			history.AddAction("Cleared Scene");
			previouslySaved = false;
			selectedEntities.clear();
		}
		if (ImGui::MenuItem("Create Entity", "CTRL E"))
			CreateEntity();
		ImGui::Separator();

		if (ImGui::MenuItem("Load Scene"))
		{
			std::string file{ };
			if (TOOLS::BrowseFolder(file, FILEDIALOGTYPE::OPEN))
			{
				// Reset
				for (unsigned i = 0; i != MAXBUFFER; ++i)
					settingsWindow->saveDirectory[i] = 0;
				// Modify
				TOOLS::MyCopy(file.begin(), file.end(), settingsWindow->saveDirectory);
				// Append the null terminating
				settingsWindow->saveDirectory[file.size()] = '\0';
				BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::LOADLEVEL, settingsWindow->saveDirectory });
			}
		}
		if (ImGui::MenuItem("Save", "CTRL S"))
		{
			BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVELEVEL, settingsWindow->saveDirectory });
		}
		
		if (ImGui::MenuItem("Exit", "ALT F4"))
			window.Exit();
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Delete Object", "DEL") && selectedEntities.size())
		{
			objectHiWindow->DeleteObjects(*this);
		}

		if (ImGui::MenuItem("Undo", "CTRL Z"))
		{
			if (history.selectedHistory)
			{
				--history.selectedHistory;
				selectedEntities.clear();
			}
		}
		if (ImGui::MenuItem("Redo", "CTRL Y"))
		{
			if (history.selectedHistory < static_cast<int>(history.actions.size()) - 1)
			{
				++history.selectedHistory;
				selectedEntities.clear();
			}
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Settings"))
	{
		if (ImGui::BeginMenu("Camera"))
		{
			// Pre-defined values
			ImGui::TextDisabled("Pre-Defined Values");
			if (ImGui::MenuItem("X1 Speed", nullptr, nullptr))
				cameraSpeedMultiplier = 1;
			if (ImGui::MenuItem("X2 Speed", nullptr, nullptr))
				cameraSpeedMultiplier = 2;
			if (ImGui::MenuItem("X3 Speed", nullptr, nullptr))
				cameraSpeedMultiplier = 3;
			if (ImGui::MenuItem("X5 Speed", nullptr, nullptr))
				cameraSpeedMultiplier = 5;
			if (ImGui::MenuItem("X8 Speed", nullptr, nullptr))
				cameraSpeedMultiplier = 8;

			ImGui::Separator();
			ImGui::TextDisabled("Custom Speed");
			TOOLS::PushID(UITYPE::DEFAULT);
			ImGui::InputInt("", &cameraSpeedMultiplier, 1, 5, ImGuiInputTextFlags_CharsNoBlank);
			ImGui::PopID();
			// Set the cap (to 100)
			cameraSpeedMultiplier = cameraSpeedMultiplier >= 100 ? 100 : cameraSpeedMultiplier;
			ImGui::Separator();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Frame Skip Count"))
		{
			// Pre-defined values
			ImGui::TextDisabled("Pre-Defined Values");
			if (ImGui::MenuItem("1 Frame Skip", nullptr, nullptr))
				frame_skips = 1;
			if (ImGui::MenuItem("5 Frame Skips", nullptr, nullptr))
				frame_skips = 5;
			if (ImGui::MenuItem("10 Frame Skips", nullptr, nullptr))
				frame_skips = 10;
			if (ImGui::MenuItem("15 Frame Skips", nullptr, nullptr))
				frame_skips = 15;
			if (ImGui::MenuItem("20 Frame Skips", nullptr, nullptr))
				frame_skips = 20;

			ImGui::Separator();
			ImGui::TextDisabled("Custom Frame Skips");
			TOOLS::PushID(UITYPE::DEFAULT);
			ImGui::SliderInt("", &frame_skips, 1, 60);
			ImGui::PopID();
			ImGui::Separator();
			ImGui::EndMenu();
		}

		CoreEngine& engine = COREENGINE_S;
		bool debug_active = engine.GetDebugDrawActive();
		if (ImGui::BeginMenu("Debug Draw"))
		{
			if (ImGui::MenuItem("True", NULL, debug_active))
				engine.SetDebugDrawActive(true);
			if (ImGui::MenuItem("False", NULL, !debug_active))
				engine.SetDebugDrawActive(false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Multiple Gamespaces"))
		{
			if (ImGui::MenuItem("True", NULL, multipleGamespaces))
				multipleGamespaces = true;
			if (ImGui::MenuItem("False", NULL, !multipleGamespaces))
				multipleGamespaces = false;
			ImGui::EndMenu();
		}

		bool cappedFPS = engine.IsFPSCapped();
		if (ImGui::BeginMenu("Cap FPS"))
		{
			if (ImGui::MenuItem("True", NULL, cappedFPS))
				engine.LimitFPS(true);
			if (ImGui::MenuItem("False", NULL, !cappedFPS))
				engine.LimitFPS(false);
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Open Visual Studio Code"))
			OpenIDE();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Windows"))
	{
		ImGui::MenuItem("DISPLAY SEVERAL WINDOWS", nullptr, false, false);
		windows.CreateMenuItems();
		ImGui::EndMenu();
	}

	unsigned engine_errors = 0;
	const auto& all_logs = SYSTEMLOG_S.GetLog();
	// Search for any errors or assertion errors
	for (auto& elem : all_logs)
	{
		if (elem.find("ERROR::") != std::string::npos)
			++engine_errors;
		else if (elem.find("ASSERTION ERROR::") != std::string::npos)
			++engine_errors;
	}

	// Display the current level
	std::string levelDisplay = "Current Level: ";
	std::string levelLoc{ settingsWindow->saveDirectory };
	levelDisplay += levelLoc.substr(levelLoc.rfind("/") + 1);
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
	ImGui::Text(levelDisplay.c_str());

	// Determine if there are any engine errors
	if (engine_errors)
	{
		errorBlinker += DELTATIME;
		if (errorBlinker > ERROR_BLINKER_INTERVAL)
		{
			blinkOn = !blinkOn;
			errorBlinker = 0;
		}

		if (!blinkOn)
		{
			std::stringstream ss;
			ss << engine_errors << (engine_errors == 1 ? " error " : " errors ");
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 175.f - (float)ss.str().length() );
			ImGui::TextColored(ImVec4{ 255.f,0,0,255.f }, ss.str().c_str());
		}
	}
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 125.f);
	ImGui::Text((engine_errors != 0 ? std::string{ " | " + FRAMECTR_S.GetFPS() }.c_str() : FRAMECTR_S.GetFPS().c_str()));
	ImGui::EndMainMenuBar();
}

void Editor::HandleMessages()
{
	const auto& messages = messageQueue->ReadMessages();
	for (const auto& msg : messages)
	{
		switch (msg.type)
		{
		case EDITORMSG::SAVEHISTORY:
			if (!IsPlaying())
			{
				history.AddAction(msg.content);
				previouslySaved = false;
			}
			break;
		case EDITORMSG::LOADHISTORYDATA:
		{
			std::unordered_map<size_t, std::string> entPos;
			auto& elist = entityList->entityList;
			for (auto ent : selectedEntities)
			{
				auto iter = std::find_if(elist.begin(), elist.end(),
					[=](const std::unique_ptr<Entity>& vItem)
				{
					return vItem.get() == ent;
				});
				if (iter != elist.end())
					entPos[iter - elist.begin()] = std::string(ent->prefabName.data());
			}
			selectedEntities.clear();
			LoadComponentData(history.actions[msg.data]->savedLayout);
			for (const auto& iter : entPos)
			{
				if (iter.first < elist.size())
				{
					std::string name = elist[iter.first]->prefabName.data();
					if (name == iter.second)
						selectedEntities.emplace_back(elist[iter.first].get());
				}
			}
		}
			break;
		case EDITORMSG::LOADLEVEL:
			if (!previouslySaved)
			{
				auto opt = TOOLS::CreateYesNoMessageBox("Do you want to save the current level?");
				if (opt)
				{
					if (opt.value())
					{
						// Save the previous level's camera
						size_t startPos = currentLevel.rfind("/") + 1;
						size_t extensionPos = currentLevel.rfind(".");
						std::string levelFile = currentLevel.substr(startPos, extensionPos - startPos);
						auto& camInfo = allLevels[levelFile];
						camInfo.position = camera.GetPosition();
						camInfo.target = camera.GetTarget();
						SaveComponentData(currentLevel);
					}
				}
				else
					break;
			}
			if (!LoadComponentData(msg.content))
				TOOLS::CreateErrorMessageBox("Could not load ffs!");
			else
			{
				// Save the previous level's camera
				size_t startPos = currentLevel.rfind("/") + 1;
				size_t extensionPos = currentLevel.rfind(".");
				std::string levelFile = currentLevel.substr(startPos, extensionPos - startPos);
				auto& camInfo = allLevels[levelFile];
				camInfo.position = camera.GetPosition();
				camInfo.target = camera.GetTarget();

				currentLevel = msg.content;
				UpdateSerializables(currentLevel);
				strcpy_s(settingsWindow->saveDirectory, currentLevel.c_str());
				history.Reset();
				history.AddAction("Loaded level: ", currentLevel);
				previouslySaved = true;
				componentWindow->CheckEntityListForPrefabs(*entityList);

				// Grab the levelFile only
				startPos = currentLevel.rfind("/") + 1;
				extensionPos = currentLevel.rfind(".");
				levelFile = currentLevel.substr(startPos, extensionPos - startPos);
				camInfo = allLevels[levelFile];
				camera.SetPosition(camInfo.position);
				camera.SetTarget(camInfo.target);
				TOOLS::CreateInfoMessageBox("Level loaded!");

			}
			break;
		case EDITORMSG::LOADTEMPORARY:
			if (!previouslySaved)
			{
				auto opt = TOOLS::CreateYesNoMessageBox("Do you want to save the current level?");
				if (opt)
				{
					if (opt.value())
						SaveComponentData(currentLevel);
				}
				else
					break;
			}
			if (!LoadComponentData(TEMPORARY_DIRECTORY))
				TOOLS::CreateErrorMessageBox("Temporary directory does not exist!");
			else
			{
				TOOLS::CreateInfoMessageBox("Level loaded!");
				currentLevel = msg.content;
				UpdateSerializables(currentLevel);
				history.Reset();
				history.AddAction("Loaded level: ", currentLevel);
				previouslySaved = true;
				// Grab the levelFile only
				size_t startPos = currentLevel.rfind("/") + 1;
				size_t extensionPos = currentLevel.rfind(".");
				std::string levelFile = currentLevel.substr(startPos, extensionPos - startPos);
				auto& camInfo = allLevels[levelFile];
				camera.SetPosition(camInfo.position);
				camera.SetTarget(camInfo.target);
				TOOLS::CreateInfoMessageBox("Level loaded!");
			}
			break;
		case EDITORMSG::SAVELEVEL:
			if (playing)
			{
				ReplaceCurrentWithTemp();
			}
			else
			{
				SaveComponentData(msg.content);
				currentLevel = msg.content;
				UpdateSerializables(currentLevel);
				previouslySaved = true;
				TOOLS::CreateInfoMessageBox("Level saved in ",
					msg.content, ".");
			}
			break;
		case EDITORMSG::SAVELEVELDEFAULT:
			if (playing)
			{
				ReplaceCurrentWithTemp();
				//TOOLS::CreateErrorMessageBox("Cannot save while playing scene!");
			}
			else
			{
				SaveComponentData(currentLevel);
				previouslySaved = true;
			}
			break;
		case EDITORMSG::PLAYBTN:
			// Not played yet (Starting to play)
			if (!playing)
				InitTestPlay();
			// Playing (to clear everything)
			else
				StopTestPlay();
			guiManager->SwitchStyle(playing);
			break;
		case EDITORMSG::PAUSEBTN:
			// Toggle boolean for the paused
			paused = !paused;
			if (paused)
			{
				if (mouseSettings.enable)
				{
					game_input.ShowMouse(true);
					game_input.LockMouseIntoCenter(false);
				}
				AUDIO_S.PauseAll();
			}
			else
			{
				if (mouseSettings.enable)
				{
					game_input.ShowMouse(mouseSettings.display);
					game_input.LockMouseIntoCenter(mouseSettings.locked);
				}
				AUDIO_S.ResumeAll();
				frameSkipCounter = 0;
				sceneNextFrame = false;
			}
			PHYSICS_S.PhysicsPause(paused);
			break;

		case EDITORMSG::SKIPFRAME:
			sceneNextFrame = true;
			AUDIO_S.ResumeAll();
			PHYSICS_S.PhysicsPause(false);
			break;

		case EDITORMSG::ENTITYREMOVED:
		{
			selectedEntities.erase(std::remove_if(selectedEntities.begin(), selectedEntities.end(),
				[&](Entity* ent) {
				if (ent == msg.ent)
					return true;
				return false;
			}), selectedEntities.end());
		}
			break;
		}
	}
}

void Editor::SaveConfig()
{
	// Grab the levelFile only
	size_t startPos = currentLevel.rfind("/") + 1;
	size_t extensionPos = currentLevel.rfind(".");
	std::string levelFile = currentLevel.substr(startPos, extensionPos - startPos);
	auto& camInfo = allLevels[levelFile];
	camInfo.position = camera.GetPosition();
	camInfo.target = camera.GetTarget();

	UpdateSerializables();
	Serializer Doc;
	SerializeElem& pRoot = Doc.NewElement("Root");
	//xmlDoc.InsertFirstChild(pRoot);

	SerializeElem& eElement = pRoot.InsertChild("Config");
	Serialize(eElement);
	//pRoot->InsertEndChild(eElement);
	Doc.SaveFile(EDITORCONFIG_DIRECTORY);
}

void Editor::LoadConfig()
{
	Serializer Doc;
	SERIALRESULT eResult = Doc.LoadFile(EDITORCONFIG_DIRECTORY);
	if (eResult != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading editor config: ", Doc.SerialResults[(int)eResult]);
		return;
	}
	SerializeElem *Root = Doc.FirstChild();
	if (!Root)
	{
		CONSOLE_ERROR("Invalid editor config file!");
		return;
	}
	SerializeElem* Element = Root->FirstChildElement();
	ConvertSerializedData(*Element);
	if (currentLevel.size())
	{
		LoadComponentData(currentLevel);
		settingsWindow->UpdateDirectory(currentLevel);
		componentWindow->CheckEntityListForPrefabs(*entityList);
		history.AddAction("Loaded level: ", currentLevel);
		previouslySaved = true;
		// Grab the levelFile only
		size_t startPos = currentLevel.rfind("/") + 1;
		size_t extensionPos = currentLevel.rfind(".");
		std::string levelFile = currentLevel.substr(startPos, extensionPos - startPos);
		auto& camInfo = allLevels[levelFile];
		camera.SetPosition(camInfo.position);
		camera.SetTarget(camInfo.target);
	}
}

void Editor::SaveComponentData(const std::string& str)
{
	Serializer serial;
	SaveComponentData(serial);
	serial.SaveFile(str);
}

void Editor::SaveComponentData(Serializer& serial)
{
	entityList->SaveComponentData(serial);
}

void Editor::UpdateMovement()
{
	// Determine if there was a scroll involved
	int scroll_diff = editor_input.GetScrollAmtPerFrame();

	// M_BUTTON will allow movement of the  y-axis
	bool mouse_y_movement = false;
	if (editor_input.GetToggleKey(KEY_MBUTTON))
	{
		mouse_y_movement = true;
		oldYMovement = editor_input.GetMousePosition().y;
	}
	if (editor_input.GetHoldKey(KEY_MBUTTON))
		mouse_y_movement = true;

	// Rotation active
	if (editor_input.GetToggleKey(KEY_RBUTTON))
	{
		oldMousePosition = editor_input.GetMousePosition();
	}
	Picking();
	// Get the camera from the graphics
	Camera *gfx_cam = GFX_S.GetCurrentCamera();
	Vector3 pos = gfx_cam->GetPosition();
	Vector3 tgt = gfx_cam->GetTarget();

	Vector3 front = tgt - pos;
	Vector3 left = gfx_cam->GetUp() ^ front;
	Vector3 up = gfx_cam->GetUp();

	// mouse rotate camera
	if (editor_input.GetHoldKey(KEY_RBUTTON))
	{
		Vector2 mouse_move = editor_input.GetMousePosition() - oldMousePosition;
		oldMousePosition = editor_input.GetMousePosition();

		if (mouse_move.x != 0.0f)
		{
			Vector4 ff = Vector4(front.x, front.y, front.z, 0.0f);
			Vector4 newf = Matrix4x4::Rotate(ff, -mouse_move.x * 1.5f, Vector3(up.x, up.y, up.z));
			// No target
			if (!target_rotation && !ent_target)
				tgt = pos + Vector3(newf.x, newf.y, newf.z);
			else
				pos = tgt - Vector3(newf.x, newf.y, newf.z);
		}

		front = tgt - pos;
		// need more testing in rotation calculation
		// rotation in y no generic rotation
		if (mouse_move.y != 0.0f)
		{
			auto newf = Rotate(-mouse_move.y * 1.5f, Vector3(left.x, left.y, left.z), front);
			if (!target_rotation && !ent_target)
				tgt = pos + newf;
			else
				pos = tgt - newf;
		}
	}

	front = (tgt - pos).Unit();
	left = gfx_cam->GetUp() ^ front;
	float cameraSpeed = BASE_SPEED * (float)cameraSpeedMultiplier * extraCameraSpeed;

	if (editor_input.GetHoldKey(KEY_A))
	{
		pos += left * cameraSpeed;
		tgt += left * cameraSpeed;
	}

	if (editor_input.GetHoldKey(KEY_D))
	{
		pos -= left * cameraSpeed;
		tgt -= left * cameraSpeed;
	}

	if (editor_input.GetHoldKey(KEY_W))
	{
		pos += front * cameraSpeed;
		tgt += front * cameraSpeed;
	}

	else if (scroll_diff > 0)
	{
		pos += front * cameraSpeed * 3.f;
		tgt += front * cameraSpeed * 3.f;
	}

	// Since CTRL+S is conflicting, we do not want to S move when CTRL or ALT is hold down
	if (!editor_input.GetHoldKey(KEY_LCTRL)
		&& editor_input.GetHoldKey(KEY_S))
	{
		pos -= front * cameraSpeed;
		tgt -= front * cameraSpeed;
	}

	else if (scroll_diff < 0)
	{
		pos -= front * cameraSpeed * 3.f;
		tgt -= front * cameraSpeed * 3.f;
	}

	float new_y_movement = editor_input.GetMousePosition().y;
	if (editor_input.GetHoldKey(KEY_Q) || (mouse_y_movement && new_y_movement - oldYMovement < 0))
	{
		pos -= gfx_cam->GetUp() * cameraSpeed;
		tgt -= gfx_cam->GetUp() * cameraSpeed;
	}

	if ((!editor_input.GetHoldKey(KEY_LALT) && editor_input.GetHoldKey(KEY_E)) || (mouse_y_movement && new_y_movement - oldYMovement > 0))
	{
		pos += gfx_cam->GetUp() * cameraSpeed;
		tgt += gfx_cam->GetUp() * cameraSpeed;
	}


	if (editor_input.GetHoldKey(KEY_LSHIFT))
	{
		extraCameraSpeed = EXTRA_CAMERA_SPEED;
	}

	if (editor_input.GetToggleKey(KEY_F))
	{
		if (!selectedEntities.empty())
		{
			Vector3 allPos{ 0,0,0 };
			Vector3 tgtToPos{ pos - tgt };

			for (auto & elem : selectedEntities)
				allPos += elem->GetComponent<TransformC>()->GetGlobalPosition();

			allPos /= static_cast<float>(selectedEntities.size());

			tgt = allPos + tgtToPos;
			pos = tgt + tgtToPos;
		}
	}

	if (editor_input.GetReleaseKey(KEY_LSHIFT))
	{
		extraCameraSpeed = NO_EXTRA_CAMERA_SPEED;
	}

	gfx_cam->SetPosition(pos);
	gfx_cam->SetTarget(tgt);
	// gfx_cam->MakeDirty();
}

void Editor::ReplaceCurrentWithTemp()
{
	std::filesystem::remove(currentLevel);
	std::filesystem::copy_file(TEMPORARY_DIRECTORY, currentLevel);
}

void Editor::Picking()
{
	// Picking (only available when ImGuizmo is not active)
	if ((!ImGuizmo::IsOver() && !ImGuizmo::IsUsing()) &&
		(!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyWindowFocused()))
	{
		if (editor_input.GetReleaseKey(KEY_LBUTTON) && mouseClick)
		{
			if (!multipleMouseSelect)
			{
				auto ent = entityList->PickEntity(editor_input.GetMousePosition(), camera);
				if (ent)
				{
					if (editor_input.GetHoldKey(KEY_LSHIFT))
					{
						bool search = false;
						auto sel_itr = selectedEntities.begin();
						auto sel_end = selectedEntities.end();
						while (sel_itr != sel_end)
						{
							if (*sel_itr == ent)
							{
								search = true;
								selectedEntities.erase(sel_itr);
								break;
							}
							sel_itr++;
						}
						if (!search)
							selectedEntities.insert(selectedEntities.begin(), ent);
					}
					else
					{
						selectedEntities.clear();
            selectedGroupEntities.clear();
            selectedEntities.emplace_back(ent);
					}
				}
        else
        {
          if (!editor_input.GetHoldKey(KEY_LSHIFT))
          {
            selectedEntities.clear();
            selectedGroupEntities.clear();
          }
        }
			}
			else
			{
				for (auto & elem : selectedGroupEntities)
				{

					bool search = false;

					auto sel_itr = selectedEntities.begin();
					auto sel_end = selectedEntities.end();

					while (sel_itr != sel_end)
					{

						if (*sel_itr == elem)
						{
							search = true;

							break;
						}


						sel_itr++;
					}

					if (!search)
						selectedEntities.emplace_back(elem);



				}
				selectedGroupEntities.clear();
			}

			mouseClick = false;
			multipleMouseSelect = false;
		}
		else if (editor_input.GetHoldKey(KEY_LBUTTON))
		{

			if (mouseClick == false)
				initialMousePosition = editor_input.GetMousePosition();

			else
			{
				if (!multipleMouseSelect)
				{
					if (initialMousePosition != editor_input.GetMousePosition())
						multipleMouseSelect = true;

				}
				else
				{
					auto group = entityList->PickGroupEntity(initialMousePosition, editor_input.GetMousePosition(), camera);
					std::vector<Entity *>::iterator sel_itr;
					std::vector<Entity *>::iterator sel_itrEnd;
          if (group.empty() && !editor_input.GetHoldKey(KEY_LSHIFT))
          {
            selectedEntities.clear();
            selectedGroupEntities.clear();
          }
          else
          {
            if (!selectedEntities.empty())
            {
              sel_itr = selectedGroupEntities.begin();
              sel_itrEnd = selectedGroupEntities.end();
            }
            else
            {
              sel_itr = selectedEntities.begin();
              sel_itrEnd = selectedEntities.end();
            }

            if (sel_itrEnd != sel_itr)
            {
              do
              {
                sel_itrEnd--;
                bool not_found = true;
                auto group_itr = group.begin();
                auto group_itrEnd = group.end();
                if (group_itrEnd != group_itr)
                {
                  do
                  {
                    group_itrEnd--;

                    if (*group_itrEnd == *sel_itrEnd)
                    {
                      not_found = false;

                      group.erase(group_itrEnd);
                      break;
                    }
                  } while (group_itrEnd != group_itr);
                }
                if (not_found)
                {
                  // if this ever breaks
                  if (!selectedEntities.empty())
                    selectedGroupEntities.erase(sel_itrEnd);
                  else
                    selectedEntities.erase(sel_itrEnd);
                }

              } while (sel_itrEnd != sel_itr);
            }
            //selectedEntities.clear();
            //

            for (auto & elem : group)
            {

              if (!selectedEntities.empty())
                selectedGroupEntities.emplace_back(elem);
              else
                selectedEntities.emplace_back(elem);
            }
          }

				}


			}

			mouseClick = true;




		}
		else
		{
			mouseClick = false;
			multipleMouseSelect = false;
		}
	}
}

void Editor::InitTestPlay()
{
	if (!entityList->TestScriptCompilation())
		return;
	editorLocked = true;
	selectedEntities.clear();
	playing = true;
	paused = false;

	INPUTMGR_S->Push(game_input);
	SaveComponentData(TEMPORARY_DIRECTORY);
	// Only one gamespace (which is the main)
	entityList->LoadComponentData(TEMPORARY_DIRECTORY);
	//if (multipleGamespaces)
	entityList->LoadMultipleGamespaces();
	entityList->SetupGameplay();
	CameraC* compCam = entityList->GetActiveCamera();
	ingameCamera = compCam->GetCamera();
	GFX_S.SetCamera(*ingameCamera);
	PHYSICS_S.Initialize(*entityList);

	PHYSICS_S.PhysicsPause(false);
	AUDIO_S.StopTests();
	AUDIO_S.SetListenerPosition(ingameCamera->GetPosition());
	mouseSettings = MouseSettings{ game_input.mouse.display, game_input.mouse.lock };
}

void Editor::UpdateTestPlay(float dt)
{
	for (auto& elem : editorScene)
		elem.gamespace->entity_list.Update(dt);
}

void Editor::StopTestPlay()
{
	sceneNextFrame = false;
	frameSkipCounter = 0;
	editorLocked = false;
	selectedEntities.clear();
	playing = false;
	paused = false;

	PHYSICS_S.Clear();
	PHYSICS_S.PhysicsPause(true);
	AUDIO_S.Unload();
	AUDIO_S.SetListenerPosition(Vector3{});
	INPUTMGR_S->Pop(true);

	//if (multipleGamespaces)
	//{
	//	// To reset back to only one gamespace
	//	editorScene.clear();
	//	editorScene.emplace_back();
	//	editorScene[0].gamespace = std::make_unique<GameSpace>(nullptr, "Editor", 0);
	//	editorScene[0].gamespace->entity_list.AssignID(0);
	//	editorScene[0].sceneFile = TEMPORARY_DIRECTORY;
	//	entityList = &editorScene[0].gamespace->entity_list;
	//	// Shortcut to resize the scene
	//	GFX_S.SetupSceneData(editorScene);
	//}

	LoadComponentData(TEMPORARY_DIRECTORY);
	GFX_S.SetCamera(camera);
	ingameCamera = nullptr;
	componentWindow->CheckEntityListForPrefabs(*entityList);
}


bool Editor::LoadComponentData(const std::string& componentData)
{
	Serializer serial;
	auto result = serial.LoadFile(componentData);
	if (result != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading save data: ", serial.SerialResults[(int)result]);
		return false;
	}
	if (!LoadComponentData(serial))
		return false;
	
	CONSOLE_SYSTEM("Loaded scene: \"", componentData, "\"");
	return true;
}

void Editor::ClearObjects()
{
	selectedEntities.clear();
}

void Editor::Render()
{
	guiManager->Render();
}

bool Editor::LoadComponentData(Serializer& serial)
{
	ClearObjects();
	entityList->LoadComponentData(serial);
	entityList->EditorSetup();
	entityList->PreloadObjects();
	return true;
}


void Editor::RecursiveEditorSetup(Entity&ent)
{
  ent.EditorSetup();
  for (auto& item : ent.children)
    RecursiveEditorSetup(*item);
}
Entity* Editor::CreateEntity(const std::string & str)
{
	selectedEntities.clear();
	auto ent = entityList->CreateEntity(str);
	if (!ent) return nullptr;
	selectedEntities.emplace_back(ent);
	if(!str.size())
		selectedEntities.back()->AddComponent<TransformC>();
  RecursiveEditorSetup(*selectedEntities.back());
	history.AddAction("New entity created");
	previouslySaved = false;
	return ent;
}
Entity* Editor::CreateEntity(const Serializer& ss)
{
	selectedEntities.clear();
	auto ent = entityList->CreateEntity(ss);
	if (!ent) return nullptr;
	selectedEntities.emplace_back(ent);
	RecursiveEditorSetup(*selectedEntities.back());
	history.AddAction("New entity created");
	previouslySaved = false;
	return ent;
}

void Editor::CopyPrefab(const Entity & ent)
{
	std::string str = ent.prefabName.data();
	auto prefabs = entityList->GetPrefabName(str);
	std::string prefabDirectory = PREFAB_DIRECTORY + str + ".pfb";
	for (auto item : prefabs)
	{
		item->LoadPrefab(prefabDirectory);
		item->UpdateComponentData();
		item->UpdateRequiredComponents();
		entityList->SetupEntityComponent(*item);
		item->EditorSetup();
	}
}

void Editor::FocusOnSelectedEntity(Camera & cam, Entity* selected_ent)
{
	Vector3 pos = cam.GetPosition();
	Vector3 tgt = cam.GetTarget();
	

	// Check for the selected entity first
	if (selected_ent)
	{
		Vector3 back{ pos - tgt };
		TransformC* comp_trans = selected_ent->GetComponent<TransformC>();
		tgt = selected_ent->GetComponent<TransformC>()->GetGlobalPosition() + back;
		pos = tgt + back;
		cam.SetPosition(pos);
		cam.SetTarget(tgt);
		ent_target = selected_ent;
		target_rotation = true;
	}
}

void Editor::GrabDroppedFiles(const std::vector<std::string>& droppedFiles)
{
	// Do something for the dropped files
	for (auto& elem : droppedFiles)
		CONSOLE_LN("File: ", elem);
}

void Editor::OpenIDE(const std::string& filename) const
{
	const std::string path{ fs::current_path().generic_string() };
	std::string script_file;
	if (filename.size())
	{
		script_file = path + "/";
		script_file += filename;
	}

	// Open IDE
	std::string ide{ fs::current_path().generic_string() + "/" + CUSTOM_IDE };
	PROCESSES_S.PushProcess({ ide, script_file }, true);
	// Checking if there's a settings.json file stored inside our VSCode
	std::string settings_path{ path };
	settings_path += "/Resources/Templates/VSCode/data/user-data/User/settings.json";
	if (!fs::exists(settings_path) && fs::exists(std::string{ path + "/Resources/Templates/VSCode/data/user-data/User/" }))
	{
		// Copy the settings template into that folder
		std::string template_settings{ path };
		template_settings += "/Resources/Templates/template_settings.json";
		std::string settings_json{ path };
		fs::copy_file(template_settings, settings_path);
	}
}

void Editor::OpenScriptModalWindow()
{
	InputManager& inputMgr = *INPUTMGR_S;
	bool modalClose = true;
	if (ImGui::BeginPopupModal("New Script - Name", &modalClose, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (modalClose && !inputMgr.IsCurrentInput(scriptInput))
			inputMgr.Push(scriptInput);

		ImGui::Text("Please enter the script name");

		ImGui::InputText("", &newScriptBuffer[0], sizeof(newScriptBuffer));
		ImGui::SameLine();
		EditorShowHelpMarker("Do avoid Window forbidden characters such as \\ / : * ? \" < > |");
		// Creating our script (when button is hit or enter is keyed)
		if (ImGui::Button("Create Script") || INPUT_S.GetToggleKey(KEY_RETURN))
		{
			bool conflict = false;
			std::string result = TOOLS::CreateScriptTemplate(newScriptBuffer, &conflict);
			ImGui::CloseCurrentPopup();
			std::string message{ conflict ? "Name conflicted with existing file in the folder!\n" : "Script file has been created!\n" };
			message += "The new script file is: " + result;
			TOOLS::CreateInfoMessageBox(message);
			CONSOLE_SYSTEM("New Script has been created: ", result);
		}
		// Detect for escape key (means we do not want to create this script)
		if (INPUT_S.GetToggleKey(KEY_ESCAPE))
		{
			ImGui::CloseCurrentPopup();
			modalClose = false;
		}
		ImGui::EndPopup();
	}

	if (!modalClose && inputMgr.IsCurrentInput(scriptInput))
		inputMgr.Pop(true);
}

Editor::~Editor()
{
	settingsWindow->Shutdown(*this);
	HandleMessages();
	SaveConfig();
	// Reset back the input
	INPUTMGR_S->RemoveInput(true);
}
