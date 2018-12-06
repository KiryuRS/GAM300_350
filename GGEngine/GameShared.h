#pragma once
struct GameSpace;

enum class GAMEMSG
{
	NONE,							// Monostate
	GAMESPACE_COMM,					// Communication between Gamespaces
};

struct GameSpacesInfo
{
	std::unique_ptr<GameSpace>		gamespace;			// Game space
	std::string						sceneFile;			// the location of the current scene file
	bool							isLevel;			// Determine if the current gamespace is a level 
	unsigned						layer;
};

// Messaging system that that game spaces can use to communicate with each other
class GameMessages
{
public:
	GAMEMSG msg_type;				// type of the message we want to capture
	Variant var;					// any type to be passed in for usage
	std::string space_name;			// name of the game space to pass to
	GameSpace* sender;				// Sender's gamespace
	bool all_game_spaces;			// messages to ALL of the game spaces in the game scene

	GameMessages(GAMEMSG _type = GAMEMSG::NONE, const Variant& _var = Variant{}, const std::string& _name = "", GameSpace*_sender = nullptr, bool _all = false)
		: msg_type{ _type }, var{ _var },
		  space_name{ _name }, sender{ _sender },
		  all_game_spaces{ _all }
	{ }
};

// Plant information
struct Plant
{
	std::string name;						// Name of the plant
	std::string fileName;					// File name for the mesh drawing
	int amount;								// Amount of plants available

	Plant(const std::string& _name = std::string{}, const std::string& _file = std::string{}, int amt = 0)
		: name{ _name }, fileName{ _file }, amount{ amt }
	{ }
};