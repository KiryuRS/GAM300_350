#pragma once
#include "GameShared.h"
#include "GameSpaces.h"
#include "Broadcaster.h"

class GameStateManager;
class GameState;
class GameSpace;

namespace GSHELPER
{
	// Acceptable input: "true" OR "false"
	constexpr bool StringToBool(const std::string_view& str)
	{
		return str == "true";
	}

	std::string GrabLevelInfo(const std::string&, std::vector<GamespaceSerializer>&, GameScene*);
}

class GameScene final : public GameState
{
	friend class GameStateManager;
	friend struct GameSpace;
	friend std::string GSHELPER::GrabLevelInfo(const std::string&, std::vector<GamespaceSerializer>&, GameScene*);
	std::vector<GamespaceSerializer>	spaces;
	GamesceneConfig						gsconfig;

	std::vector<GameSpacesInfo>		allGameSpaces;
	std::vector<GameMessages>		msg_queue;

	token msg_token;
	bool transition_in, transition_out;				// determines the transition between each scene (scene in or scene out)
	bool transitioning;								// deteremines if the transition is ongoing
	bool pauseable;									// determines if the current scene can be paused
	bool scene_paused;								// determines if the scene is paused
	std::string next_level;							// retrieves the next level_file to generate
	Input pauseInput;								// Mainly for pause menu
	unsigned highestLayer;							// Highest layer in the current scene

	void HandleMessages();
	void ReceiveMessage(const GameMessages& msg);

	// Transition in will be called when needed 
	void TransitionIn() override;
	// Transition out will be called when needed
	void TransitionOut() override;

public:
	GameScene(const std::string& levels_file);
	void Init() override;
	void Cleanup() override;
	bool Pause(bool actual_pause) override;
	bool Resume(bool actual_resume) override;
	void HandleEvents(GameStateManager* gsm) override;
	bool Update(GameStateManager* gsm) override;
	void Draw(GameStateManager* gsm) override;
	bool HasTransitionCompleted() const override;
	~GameScene();

	// Grabs the gamespace if it exists
	GameSpace* GrabSpace(const std::string& str) const;
	// Register any messages we have to (to each of the game spaces affected)
	void RegisterNewMessage(const GameMessages& game_msg);
	// Determine if the current scene is paused
	bool ScenePaused() const;
	// For restarting of the game
	void RestartScene();
	// To switch between game spaces (main)
	bool SwitchInternalLevel(const std::string& gamespace_name) const;
	// Grabs all of the Gamespace names that are levels
	std::vector<std::string> GetGamespacesLevel() const;
	// Grab the current active level in the current game scene
	GameSpace* GetCurrentLevelGamespace() const;
	// Grab all of the current cameras in the scene
	std::vector<Camera*> GetAllCameras() const;
	// Determine the total number of gamespaces
	inline unsigned TotalGameSpaces() const									{ return allGameSpaces.size(); }
	// Grab all of the gamespaces
	inline const std::vector<GameSpacesInfo>& GetAllGamespaces() const		{ return allGameSpaces; }
};