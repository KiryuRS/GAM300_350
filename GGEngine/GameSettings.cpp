#include "stdafx.h"

bool GameSettings::CheckModalPopup(Editor &)
{
	bool success = false;

	// This section would be to save the actual set of gamespaces for levels
	ImGui::TextDisabled("This modal window shows the configuration of a gamescene setup");
	ImGui::Text("Next Level:");
	ImGui::SameLine();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", sceneConfig.imguiNextLevel, MAX_PATH);
	ImGui::PopID();
	TOOLS::AddToolTip("Determines the next level for the current game scene");
	ImGui::SameLine();
	ImGui::Text("Scene Pauseable:");
	ImGui::SameLine();
	ImGui::Checkbox("", &sceneConfig.pauseable);
	ImGui::Separator();
	/*
		Ideal set will have the following:
		1) Loading Screen Gamespace
		2) Pause Menu Gamespace
		3) UI Gamespace
		4) Main gameplay Gamespace
	*/
	int& size = sceneConfig.spacesSize;
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputInt("", &size, 1, 1);
	ImGui::PopID();
	TOOLS::AddToolTip("Ideal set will have the following\n1) Loading Screen Gamespace\n2) Pause Menu Gamespace\n3) UI Gamespace\n4) Main Gameplay Gamespace [DAY]\n5) Main Gameplay Gamespace [NIGHT]");
	size = size > 10 ? 10 : size;
	if (size - spaces.size() > 0)
	{
		int currentSize = spaces.size();
		spaces.resize(size);
		for (auto& elem : spaces)
			elem.UpdateSerializables();
	}
	for (size_t i = 0; i != size; ++i)
	{
		std::string headerName{ gamespaceName };
		headerName += std::to_string(i);
		if (ImGui::CollapsingHeader(headerName.c_str()))
			spaces[i].GenerateUI();
	}
	sceneConfig.spacesSize = size;
	ImGui::Separator();
	ImGui::Text("Scene Name:");
	ImGui::SameLine();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", saveFileLoc, MAXBUFFER);
	ImGui::PopID();
	TOOLS::AddToolTip("There's no need to input in the extension.");
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		if (!sceneConfig.spacesSize || !strlen(saveFileLoc))
		{
			TOOLS::CreateErrorMessageBox("Unable to save as a game scene! Did you set it up correctly?");
		}
		else
		{
			sceneConfig.UpdateSerializables(sceneConfig.spacesSize);
			Serializer serial;
			auto& config = serial.NewElement("Configuration");
			sceneConfig.Serialize(config);

			// Gamespace saving
			for (size_t i = 0; i != sceneConfig.spacesSize; ++i)
			{
				std::string headerName{ gamespaceName };
				headerName += std::to_string(i);
				auto& elem = serial.NewElement(headerName);
				spaces[i].Serialize(elem);
			}
			std::string saveLoc = saveFileDir + saveFileLoc + ".lvl";
			serial.SaveFile(saveLoc);
			TOOLS::CreateInfoMessageBox("GameScene created in: ", saveLoc);
			success = true;
		}
	}
	ImGui::PopID();
	ImGui::Separator();
	return success;
}

GameSettings::GameSettings(Editor& edit) :
	GuiWindow(edit, windowName),
	saveDirectory{ {0} }
{ 
	
}

void GameSettings::Startup(Editor & edit)
{
	Serializer serial;
	auto loadResult = serial.LoadFile(tempScene);
	if (loadResult == SERIALRESULT::SUCCESS)
	{
		for (int i = 0; i != edit.spacesSize; ++i)
		{
			// First update the serializables
			editorSpaces[i].UpdateSerializables();
			std::string headerName{ gamespaceName };
			headerName += std::to_string(i);
			auto child = serial.FirstChildElement(headerName);
			if (child)
				editorSpaces[i].ConvertSerializedData(*child);
		}
	}
}

void GameSettings::Shutdown(Editor & edit)
{
	// Save the tempSpaces.ffs (Edited)
	Serializer serial;

	// Gamespaces saving
	for (size_t i = 0; i != edit.spacesSize; ++i)
	{
		editorSpaces[i].UpdateSerializables();
		std::string headerName{ gamespaceName };
		headerName += std::to_string(i);
		auto& elem = serial.NewElement(headerName);
		editorSpaces[i].Serialize(elem);
	}
	serial.SaveFile(tempScene);
}

void GameSettings::Display(Editor& edit)
{
	ImGui::BeginChild("Game Settings Section");
	ImGui::Text("Current Directory:");
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", saveDirectory, MAXBUFFER);
	ImGui::PopID();
	ImGui::SameLine();
	if (ImGui::Button("..."))
	{
		std::string file;
		if (TOOLS::BrowseFolder(file, FILEDIALOGTYPE::OPEN))
		{
			strcpy_s(saveDirectory, file.c_str());
			BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::LOADLEVEL, saveDirectory }));
		}
	}
	if (ImGui::Button("Save"))
	{
		BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::SAVELEVEL, saveDirectory }));
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::LOADLEVEL, saveDirectory }));
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Unsaved Backup"))
		BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::LOADTEMPORARY, saveDirectory }));

	// Graphical side settings
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Skybox Settings"))
	{
		if (edit.entityList->settings.GenerateUI())
			edit.entityList->BroadcastSettings();
	}


	// Gamespaces view
	bool modalClose = true;
	InputManager& inputMgr = *INPUTMGR_S;
	ImGui::Separator();
	if (!edit.multipleGamespaces)
	{
		ImGui::TextDisabled("(!)");
		TOOLS::AddToolTip("Gamespaces view is disabled!\nPlease enable it in Tools > Gamespaces View");
		ImGui::SameLine();
	}
	ImGui::Text("Gamespaces Section: ");
	// Display the button to setup the game scene
	if (ImGui::Button("Setup Game Scene"))
	{
		spaces.clear();
		sceneConfig.spacesSize = 2;
		spaces.resize(sceneConfig.spacesSize);
		for (int i = 0; i != sceneConfig.spacesSize; ++i)
		{
			spaces[i] = editorSpaces[i];
			spaces[i].UpdateSerializables();
		}

		ImGui::OpenPopup("New Game Scene - Setup");
	}

	ImGui::Text("Main GS:");
	ImGui::SameLine();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputInt("", &edit.mainLayerNum);
	ImGui::PopID();
	// Sanity Check
	edit.mainLayerNum = edit.mainLayerNum < 1 ? 1 : edit.mainLayerNum > edit.spacesSize ? edit.spacesSize : edit.mainLayerNum;

	// Display the temp.lvl settings (which should only consists of UI and Gameplay)
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputInt("", &edit.spacesSize);
	ImGui::PopID();
	TOOLS::AddToolTip("For displaying UI and gameplay during Editor Mode");
	edit.spacesSize = edit.spacesSize < 1 ? 1 : edit.spacesSize > 2 ? 2 : edit.spacesSize;
	for (int i = 0; i != edit.spacesSize; ++i)
	{
		std::string headerName{ gamespaceName };
		headerName += std::to_string(i);
		if (ImGui::CollapsingHeader(headerName.c_str()))
			editorSpaces[i].GenerateUI();
	}

	if (ImGui::BeginPopupModal("New Game Scene - Setup", &modalClose, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (modalClose && !inputMgr.IsCurrentInput(gameSceneInput))
			inputMgr.Push(gameSceneInput);
		bool result = CheckModalPopup(edit);
		if (result || INPUT_S.GetToggleKey(KEY_ESCAPE))
		{
			ImGui::CloseCurrentPopup();
			modalClose = false;
		}

		ImGui::EndPopup();
	}

	if (!modalClose && inputMgr.IsCurrentInput(gameSceneInput))
		inputMgr.Pop(true);
	ImGui::Separator();

	ImGui::EndChild();
}

void GameSettings::Update(Editor& editor)
{
	// Only want the keys to be active when there isn't any scene playing in editor mode
	if (!editor.EditorKeyLocked())
	{
		if (editor.editor_input.CheckCombination(KEY_LCTRL, KEY_S))
			BROADCAST_MESSAGE(EditorMessage({ EDITORMSG::SAVELEVEL, saveDirectory }));
		if (editor.editor_input.CheckCombination(KEY_LCTRL, KEY_E))
			editor.CreateEntity();
		if (editor.editor_input.CheckCombination(KEY_LCTRL, KEY_N))
		{
			editor.entityList->Clear();
			editor.history.AddAction("Cleared Scene");
			editor.previouslySaved = false;
			editor.selectedEntities.clear();
		}
	}
}

void GameSettings::UpdateDirectory(const std::string & str)
{
	strcpy_s(saveDirectory, str.c_str());
}
