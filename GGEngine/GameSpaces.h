#pragma once
#include "GameShared.h"

class EntityList;
class GameScene;
class Camera;

/*
	Game Spaces for each "layer". And this would be part of the Game Scene class.
*/
struct GameSpace
{
	std::string name;									// game space name
	GameScene *attached_scene;							// game scene that attaches to the game space
	EntityList entity_list;								// entity list for the game space
	Camera *camera;										// Camera for each game space
	unsigned layer;										// determines the layer numbering for graphics to draw
	bool active;										// determines if the current game space should be handled
	bool focusOn;										// For camera settings

	void HandleMessages(const GameMessages& msg);
	GameSpace(GameScene* curr_scene, const std::string& gs_name, unsigned layer_num);
	bool Init(const std::string& level_name);
	bool OnlyLoadLevelObjects(const std::string& level_name);
	void Update(float dt);
	bool SubGamespaceInit(const std::string & level_name);
	void SetupGameplay();
	void Cleanup();
	void Pause(bool actual_pause);
	void Resume(bool actual_resume);
	void ResetGameSpace(const GamespaceSerializer& gsr);
	// If allSpaces is set to true, then it will be broadcasted to ALL gamespaces in the scene
	void SendMessageToAnotherGamespace(const Variant& msg, const std::string& gamespaceName, bool allSpaces = false);
	~GameSpace();
};