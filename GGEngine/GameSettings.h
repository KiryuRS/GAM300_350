#pragma once
#include "GuiWindow.h"

struct GamespaceSerializer : public Serializable
{
	std::string name;
	std::string file;
	bool focus;
	bool active;
	int layer;
	bool isLevel;

	void AddAllSerializables()
	{
		AddSerializable("Name", name);
		AddSerializable("File", file);
		AddSerializable("Focus", focus, "To allow camera to focus on that current gamespace (in use with isLevel)");
		AddSerializable("Active", active, "Current gamespace should be active?");
		AddSerializable("isLevel", isLevel, "Is the current gamespace a level (for playing) or just UI/PauseMenu?");
	}

	GamespaceSerializer()
		: focus{ true }, active{ true }, layer{ 0 }, isLevel{ true }
	{
		AddAllSerializables();
	}

	GamespaceSerializer(const GamespaceSerializer& rhs)
		: focus{ rhs.focus }, active{ rhs.active }, layer{ rhs.layer }, isLevel{ rhs.isLevel }
	{
		AddAllSerializables();
	}
};

struct GamesceneConfig : public Serializable
{
	char imguiNextLevel[MAX_PATH];			// For keying the next level
	bool pauseable;
	std::string nextLevel;
	int spacesSize;

	GamesceneConfig()
		: pauseable{ true }, spacesSize{ 0 }
	{
		AddSerializable("Space Size", spacesSize).display = false;
		AddSerializable("Pauseable", pauseable);
		AddSerializable("Next Level", nextLevel);
	}
};

class GameSettings : public GuiWindow
{
	friend class Editor;

	static constexpr size_t MAXBUFFER = MAX_PATH;
	std::string sceneTemplate{ "Resources/Templates/scene.template" };
	std::string spaceTemplate{ "Resources/Templates/space.template" };
	const std::string gamespaceName{ "Gamespace " };
	const std::string saveFileDir{ "Resources/Savefile/Levels/" };

	std::vector<GamespaceSerializer> spaces;			// Meant for setting up the game scene data
	std::array<GamespaceSerializer, 2> editorSpaces;	// For editor view (only gameplay + UI)

	GamesceneConfig sceneConfig;
	char saveFileLoc[MAXBUFFER];
	Input gameSceneInput;

	bool CheckModalPopup(Editor&);

public:
	static constexpr char windowName[] = "Game_Settings";
	char saveDirectory[MAXBUFFER];
	bool mouseLock;
	bool mouseHide;
	std::string tempScene{ "Resources/Savefile/Misc/TempSpaces.ffs" };
	GameSettings(Editor& edit);
	// Loading of the temp file
	void Startup(Editor& edit);
	// Saving of the temp file
	void Shutdown(Editor& edit);
	void Display(Editor&) override;
	void Update(Editor&) override;
	void UpdateDirectory(const std::string&);
};