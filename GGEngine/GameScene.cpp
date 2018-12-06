#include "stdafx.h"

namespace GSHELPER
{
	std::string GrabLevelInfo(const std::string& level_file, std::vector<GamespaceSerializer>& spaces, GameScene* _this)
	{
		//enum READTYPE
		//{
		//	NONE = 0,
		//	GAMESPACE,
		//	NEXTLEVEL,
		//	CONFIG,
		//};

		//READTYPE type{};
		//std::string readLine;
		//std::string nextLevel;

		//// ifstream file
		//std::ifstream readFile{ level_file };
		//if (!readFile.is_open())
		//{
		//	CONSOLE_ERROR("Wrong file passed in!");
		//	readFile.close();
		//	return nextLevel;
		//}

		//while (std::getline(readFile, readLine))
		//{
		//	// Reading in the type
		//	if (readLine.find("start nextlevel") != std::string::npos)
		//	{
		//		type = READTYPE::NEXTLEVEL;
		//		continue;
		//	}
		//	else if (readLine.find("start gamespace") != std::string::npos)
		//	{
		//		type = READTYPE::GAMESPACE;
		//		gameSpaces.emplace_back();
		//		continue;
		//	}
		//	else if (readLine.find("start config") != std::string::npos)
		//	{
		//		type = READTYPE::CONFIG;
		//		continue;
		//	}
		//	// Ending of relevant types
		//	else if (readLine.find("end gamespace") != std::string::npos ||
		//		readLine.find("end nextlevel") != std::string::npos ||
		//		readLine.find("end config") != std::string::npos)
		//	{
		//		type = READTYPE::NONE;
		//		continue;
		//	}


		//	switch (type)
		//	{
		//	case READTYPE::CONFIG:
		//		if (size_t pos = readLine.find("pauseable:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			const std::string& boolean = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//			_this->pauseable = StringToBool(boolean.c_str());
		//		}
		//		break;

		//	case READTYPE::NEXTLEVEL:
		//		if (size_t pos = readLine.find("name:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			// Capture it into the scene
		//			nextLevel = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//		}
		//		break;

		//	case READTYPE::GAMESPACE:
		//		if (size_t pos = readLine.find("name:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			// Capture it into the latest gamespace
		//			gameSpaces.back().name = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//		}
		//		else if (pos = readLine.find("file:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			// Capture it into the latest gamespace
		//			gameSpaces.back().fileLocation = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//		}
		//		else if (pos = readLine.find("focus:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			const std::string& boolean = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//			gameSpaces.back().focus = StringToBool(boolean.c_str());
		//		}
		//		else if (pos = readLine.find("active:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			const std::string& boolean = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//			gameSpaces.back().active = StringToBool(boolean.c_str());
		//		}
		//		else if (pos = readLine.find("layer:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			const std::string& layer = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//			unsigned currLayer = std::stoi(layer) - 1;
		//			gameSpaces.back().layer = currLayer;
		//			_this->highestLayer = currLayer >= _this->highestLayer ? currLayer : _this->highestLayer;
		//		}
		//		else if (pos = readLine.find("isLevel:") != std::string::npos)
		//		{
		//			size_t firstCol = readLine.find_first_of("\"", pos);
		//			size_t secCol = readLine.find_last_of("\"");
		//			const std::string& boolean = readLine.substr(firstCol + 1, secCol - firstCol - 1);
		//			gameSpaces.back().isLevel = StringToBool(boolean.c_str());
		//		}
		//		break;
		//	}
		//}

		//return nextLevel;

		const std::string gamespaceName{ "Gamespace " };
		
		// Update the serializables first
		_this->gsconfig.UpdateSerializables();
		Serializer serial;
		auto loadResult = serial.LoadFile(level_file);
		if (loadResult == SERIALRESULT::SUCCESS)
		{
			// Get the number of gamespaces
			auto config = serial.FirstChildElement("Configuration");
			if (config)
				_this->gsconfig.ConvertSerializedData(*config);

			// Once we get the config, resize our spaces
			spaces.resize(_this->gsconfig.spacesSize);
			for (int i = 0; i != _this->gsconfig.spacesSize; ++i)
			{
				// First update the serializables
				spaces[i].UpdateSerializables();
				std::string headerName{ gamespaceName };
				headerName += std::to_string(i);
				auto child = serial.FirstChildElement(headerName);
				if (child)
					spaces[i].ConvertSerializedData(*child);
				spaces[i].layer = i;
			}
		}

		_this->highestLayer = spaces.size() - 1;
		_this->pauseable = _this->gsconfig.pauseable;
		return _this->gsconfig.nextLevel;
	}
}

void GameScene::HandleMessages()
{
	// Based on the messages in the queue, dispatch the messages to the respective game spaces
	for (const auto& msg : msg_queue)
	{
		// Check if the message is to dispatch to all of the game spaces
		if (msg.all_game_spaces)
		{
			for (auto& elem : allGameSpaces)
				elem.gamespace->HandleMessages(msg);
			continue;
		}

		auto func = [&msg](const GameSpacesInfo& gs) { return msg.space_name == gs.gamespace->name; };
		// Depending on which of the game space that its supposed to cater for, we dispatch the message
		auto iter = std::find_if(allGameSpaces.begin(), allGameSpaces.end(), func);
		if (iter != allGameSpaces.end())
			// Dispatch the messaging into the game space
			iter->gamespace->HandleMessages(msg);
	}

	// After which, we clear all the messages in the queue
	msg_queue.clear();
}

void GameScene::ReceiveMessage(const GameMessages & msg)
{
	msg_queue.emplace_back(msg);
}

void GameScene::TransitionIn()
{
	// Transition in means going from "black screen" to the game scene
}

void GameScene::TransitionOut()
{
	// Transition out means going from game scene to "black scene"
}

GameScene::GameScene(const std::string & levels_file)
	: transition_in{ false }, transition_out{ false }, pauseable{ true },
	  scene_paused{ false }, highestLayer{ 0 }
{
	next_level = GSHELPER::GrabLevelInfo(levels_file, spaces, this);

	// Store all of them into our Game Spaces
	size_t total_size = spaces.size();
	allGameSpaces.resize(total_size);
	for (unsigned i = 0; i != total_size; ++i)
	{
		// Reading our gamespaces from the back since the "highest" layer should be rendered at the "back"
		const GamespaceSerializer& curr = spaces[i];
		// Add to the game spaces
		auto& newGameSpace = allGameSpaces[i];
		// Grab out only the file name
		size_t pos = curr.file.rfind("/");
		std::string levelFile;
		if (pos != std::string::npos)
			levelFile = curr.file.substr(pos + 1);

		// Gamespace configurations
		newGameSpace.layer = curr.layer;
		newGameSpace.gamespace = std::make_unique<GameSpace>(const_cast<GameScene*>(this), levelFile, newGameSpace.layer);
		newGameSpace.gamespace->name = curr.name;
		newGameSpace.sceneFile = curr.file;
		newGameSpace.gamespace->focusOn = curr.focus;
		newGameSpace.isLevel = curr.isLevel;
		newGameSpace.gamespace->active = curr.active;
	}

	// Register to listen to specific messages
	std::function<void(GameMessages)> recv_func = [this](const GameMessages& msg)
	{
		ReceiveMessage(msg);
	};
	msg_token = REGISTER_RECIPIENT<GameMessages>(recv_func);
}

void GameScene::Init()
{
	//WINDOW_S.FullScreenSwitch();
	// Initialize all of the game spaces
	for (auto& elem : allGameSpaces)
	{
		elem.gamespace->Init(elem.sceneFile);
	}

	// Append the PHYSICS to only the first (since that's the main gameplay)
	PHYSICS_S.Initialize(allGameSpaces[0].gamespace->entity_list);
	PHYSICS_S.PhysicsPause(false);

	// Initialize the scripts
	for (auto& elem : allGameSpaces)
	{
		elem.gamespace->SetupGameplay();
	}

}

void GameScene::Cleanup()
{
	// Clear up all of the pending messages in the queue
	msg_queue.clear();

	// Clear every single stuff in the game_spaces
	for (auto& elem : allGameSpaces)
		elem.gamespace->Cleanup();
}

bool GameScene::Pause(bool actual_pause)
{
	// Notify all of the game spaces to stop their updating
	for (auto& elem : allGameSpaces)
		elem.gamespace->Pause(actual_pause);

	// Inform physics to pause
	PHYSICS_S.PhysicsPause(true);

	return true;
}

bool GameScene::Resume(bool actual_resume)
{
	// Notify all of the game spaces to resume updating (except for pause menus)
	for (auto& elem : allGameSpaces)
	{
		elem.gamespace->Resume(actual_resume);
		// Disable the gamespace level that are not on focus
		if (elem.isLevel && !elem.gamespace->focusOn)
			elem.gamespace->active = false;
	}

	// Inform physics to unpause
	PHYSICS_S.PhysicsPause(false);

	return true;
}

void GameScene::HandleEvents(GameStateManager *gsm)
{
	Input& input = INPUT_S;
	Controller& cntrl = INPUT_CNTRL_S;
	// Check if the "paused" key is pressed
	if (pauseable)
	{
		// Controller side
		bool triggered = false;
		for (unsigned i = 0; i != GAMEPAD_COUNT; ++i)
			if (cntrl.ButtonTriggered((GAMEPAD_DEVICE)i, XINPUT_BUTTON_START))
				triggered = true;

		// Keyboard side
		if (input.GetToggleKey(KEY_ESCAPE))
			triggered = true;

		if (triggered)
		{
			if (scene_paused)
			{
				// Unpause the game (however, will only be done on the next frame)
				gsm->SetSpecialState(GSM_STATE::RESUME_STATE);
				scene_paused = false;
			}
			else
			{
				// Pause the game (however, will only be done on the next frame)
				gsm->SetSpecialState(GSM_STATE::PAUSED_STATE);
				scene_paused = true;
			}
		}
	}
}

bool GameScene::Update(GameStateManager *)
{
	// We can get the delta time from Common.h
	float dt = DELTATIME;

	// Skip updating if the delta time exceeds
	if (dt > 1)
		return false;

	// First, we handle any messages that game spaces need to be aware of
	HandleMessages();

	// Update all of our game spaces
	for (auto& elem : allGameSpaces)
		elem.gamespace->Update(dt);

	return true;
}

void GameScene::Draw(GameStateManager *)
{

}

bool GameScene::HasTransitionCompleted() const
{
	return false;
}

GameScene::~GameScene()
{
	// Clear up the game spaces
	allGameSpaces.clear();
	spaces.clear();
}

GameSpace* GameScene::GrabSpace(const std::string & str) const
{
	auto func = [&str](const GameSpacesInfo& gs)
				{
					return str == gs.gamespace->name;
				};

	auto iter = std::find_if(allGameSpaces.begin(), allGameSpaces.end(), func);
	return (iter != allGameSpaces.end() ? iter->gamespace.get() : nullptr);
}

void GameScene::RegisterNewMessage(const GameMessages & game_msg)
{
	bool to_add = false;
	// Sanity check to determine if the message is meant for the current game scene
	if (game_msg.all_game_spaces)
		to_add = true;
	else
	{
		auto iter = std::find_if(allGameSpaces.begin(), allGameSpaces.end(),
			[&game_msg](const GameSpacesInfo& up_gs)
		{
			return game_msg.space_name == up_gs.gamespace->name;
		});

		if (iter != allGameSpaces.end())
			to_add = true;
	}
	if (to_add)
		BROADCAST_MESSAGE(GameMessages{ game_msg });
}

bool GameScene::ScenePaused() const
{
	return scene_paused;
}

void GameScene::RestartScene()
{
	// @todo: handle any miscellanous game scene resets if required

	// Reset all of the game spaces in the scene
	size_t size = allGameSpaces.size();
	for (unsigned i = 0; i != size; ++i)
	{
		// Retrieve the gamespaceread info first
		const GamespaceSerializer& info = spaces[i];
		allGameSpaces[i].gamespace->ResetGameSpace(info);
	}
}

bool GameScene::SwitchInternalLevel(const std::string & gamespace_name) const
{
	// Find the current gamespace_name in the list of game spaces
	auto iter = std::find_if(allGameSpaces.begin(), allGameSpaces.end(), [&gamespace_name](const GameSpacesInfo& elem)
	{
		return elem.gamespace->name == gamespace_name;
	});

	if (iter == allGameSpaces.end())
	{
		CONSOLE_ERROR("Gamespace name not found in the Game Scene!");
		return false;
	}

	// Switch out the game space
	GameSpace* current = GetCurrentLevelGamespace();
	current->active = false;
	current->focusOn = false;
	
	GameSpace* toSwitch = iter->gamespace.get();
	toSwitch->active = true;
	toSwitch->focusOn = true;
	GFX_S.SetCamera(*toSwitch->camera);

	return true;
}

std::vector<std::string> GameScene::GetGamespacesLevel() const
{
	std::vector<std::string> levelGamespaces;
	for (auto& elem : allGameSpaces)
	{
		if (elem.isLevel)
			levelGamespaces.emplace_back(elem.gamespace->name);
	}

	return levelGamespaces;
}

GameSpace * GameScene::GetCurrentLevelGamespace() const
{
	GameSpace *current = nullptr;
	for (auto& elem : allGameSpaces)
		if (elem.gamespace->focusOn && elem.isLevel)
		{
			current = elem.gamespace.get();
			break;
		}

	return current;
}

std::vector<Camera*> GameScene::GetAllCameras() const
{
	size_t size = allGameSpaces.size();
	std::vector<Camera*> allCams{ size };
	for (unsigned i = 0; i != size; ++i)
	{
		auto& elem = allGameSpaces[i];
		allCams[i] = elem.gamespace->camera;
	}

	return allCams;
}
