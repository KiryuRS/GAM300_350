#include "stdafx.h"
#include "ext.hpp"

void GameSpace::HandleMessages(const GameMessages & msg)
{
	// Handle the relevant messages here
	switch (msg.msg_type)
	{
	case GAMEMSG::GAMESPACE_COMM:
		entity_list.GrabMessageFromGS(msg.var, msg.sender);
		break;
	}
}

GameSpace::GameSpace(GameScene * curr_scene, const std::string & gs_name, unsigned layer_num)
	: attached_scene{ curr_scene }, name{ gs_name }, layer{ layer_num },
	  entity_list{ }, camera{ nullptr },
	  active(true)
{  }

bool GameSpace::Init(const std::string & level_name)
{
	// Assign an ID to each layer
	entity_list.AssignID(layer);
	// Generate the level based on the file passed in
	Serializer serial;
	auto result = serial.LoadFile(level_name);
	if (result != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading save data: ", (int)result);
		return false;
	}

	// Grab all the components from the Entities Section
	auto *p_root = serial.FirstChild();

	if (!p_root)
	{
		CONSOLE_ERROR("The FFS passed in: \"", level_name, "\" is invalid!");
		return false;
	}
	entity_list.LoadComponentData(serial);

	// Initialize the entity
	entity_list.currGamespace = this;
	
	// Find the camera component in the list of entity_list (and change the values accordingly)
	CameraC* comp_cam = entity_list.GetActiveCamera();
	camera = comp_cam->camera.get();
	// Setting up the camera for graphics will be done in the graphics' section

	CONSOLE_SYSTEM("Game Space scene has loaded: ", level_name);
	return true;
}

bool GameSpace::OnlyLoadLevelObjects(const std::string & level_name)
{
	// Assign an ID to each layer
	entity_list.AssignID(layer);
	// Generate the level based on the file passed in
	Serializer serial;
	auto result = serial.LoadFile(level_name);
	if (result != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading save data: ", (int)result);
		return false;
	}

	// Grab all the components from the Entities Section
	auto *p_root = serial.FirstChild();

	if (!p_root)
	{
		CONSOLE_ERROR("The XML passed in: \"", level_name, "\" is invalid!");
		return false;
	}

	auto elem = p_root->FirstChildElement("Entities");
	entity_list.LoadComponentData(elem->FirstChildElement());

	return true;
}

void GameSpace::Update(float dt)
{
	// Do not update anything if the game space is disabled
	if (!active)
		return;

	if (focusOn)
		// Update the listener's position (audio) from camera
		AUDIO_S.SetListenerPosition(camera->GetPosition());

	// Update our entity list
	entity_list.HandleMessages();
	entity_list.Update(dt);
}

bool GameSpace::SubGamespaceInit(const std::string & level_name)
{
	// Assign an ID to each layer
	entity_list.AssignID(layer);
	// Generate the level based on the file passed in
	Serializer serial;
	auto result = serial.LoadFile(level_name);
	if (result != SERIALRESULT::SUCCESS)
	{
		CONSOLE_ERROR("Error loading save data: ", (int)result);
		return false;
	}

	// Grab all the components from the Entities Section
	auto *p_root = serial.FirstChild();

	if (!p_root)
	{
		CONSOLE_ERROR("The FFS passed in: \"", level_name, "\" is invalid!");
		return false;
	}
	entity_list.LoadComponentData(serial);

	// Initialize the entity
	entity_list.currGamespace = this;

	// Find the camera component in the list of entity_list (and change the values accordingly)
	CameraC* comp_cam = entity_list.GetActiveCamera();
	camera = comp_cam->camera.get();
	// Setting up the camera for graphics will be done in the graphics' section

	CONSOLE_SYSTEM("Game Space scene has loaded: ", level_name);
	return true;
}

void GameSpace::SetupGameplay()
{
	entity_list.LoadMultipleGamespaces();
	entity_list.SetupGameplay();
}

void GameSpace::Cleanup()
{
	// Clear all of the entity_list
	entity_list.Clear();
}

void GameSpace::Pause(bool actual_pause)
{
	// Unpause if its the PauseMenu, otherwise pause it
	active = !actual_pause;
	if (name == "PauseMenu")
	{
		active = true;
		INPUTMGR_S->Push(attached_scene->pauseInput);
	}
}

void GameSpace::Resume(bool actual_resume)
{
	// Pause if its the PauseMenu, otherwise unpause it
	active = actual_resume;
	if (name == "PauseMenu")
	{
		active = false;
		INPUTMGR_S->Pop(true);
	}
}

void GameSpace::ResetGameSpace(const GamespaceSerializer& gsr)
{
	/*
		When resetting the gamespace, we have to note the following:
		1) All values should be the same from .lvl files
		2) All entityList should be reset
		3) Camera settings should be reset as well
	*/
	// (1)
	focusOn = gsr.focus;
	active = gsr.active;

	// (2)
	Init(gsr.file);

	// (3)
	camera = entity_list.GetActiveCamera()->camera.get();
}

void GameSpace::SendMessageToAnotherGamespace(const Variant & msg, const std::string & gamespaceName, bool allSpaces)
{
#ifndef EDITOR
	attached_scene->RegisterNewMessage(GameMessages{ GAMEMSG::GAMESPACE_COMM, msg, gamespaceName, this, allSpaces });
#endif
}

GameSpace::~GameSpace()
{

}


