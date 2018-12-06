#pragma once
#include <memory>
#include "WindowHolder.h"
#include "Serializable.h"
#include "History.h"
#include "Camera.h"

class CameraC;
class Window;
class GuiManager;
class GameSettings;
class EntityList;
class Entity;
template<typename T>
struct ThreadsafeQueue;
class Serializer;
class ComponentWindow;
class ObjectHierarchyWindow;
struct GameSpace;

enum class EDITORMSG
{
	SAVEHISTORY,				// Save History of the action taken
	LOADHISTORYDATA,			// Load the Histories of action
	LOADLEVEL,					// Load a particular level
	LOADTEMPORARY,				// Load the temporary level saved
	SAVELEVEL,					// Saves a level
	SAVELEVELDEFAULT,			
	PLAYBTN,					// Play the current scene
	PAUSEBTN,					// Pause the current scene
	SKIPFRAME,					// Skip a frame (for paused state only)
	ENTITYREMOVED,				// Removed an entity from the scene
};

struct EditorMessage
{
	EDITORMSG type;
	std::string content = "";
	int data = 0;
	std::string name = "";
	Entity* ent = nullptr;
};

struct SerializeCameraSettings
{
	Vector3			position = Vector3{ 0,0,10.f };
	Vector3			target   = Vector3{ 0,0,0 };
};

#define ERROR_BLINKER_INTERVAL		0.5f
#define EXTRA_CAMERA_SPEED			3.f
#define NO_EXTRA_CAMERA_SPEED		1.f
#define BASE_SPEED					2.f

class Editor : public Serializable
{
	struct MouseSettings
	{
		bool display;
		bool locked;
		bool enable = true;
	};

	friend class EntityWrapper;

	std::map<std::string, SerializeCameraSettings> allLevels;
	std::vector<GameSpacesInfo> editorScene;

	int cameraSpeedMultiplier = 1;
	float extraCameraSpeed = 3.f;
	float timer = 0.0f;
	const float rotationMovement = 2.f;
	float errorBlinker = 0;
	bool blinkOn = false;
	bool mouseClick = false;
	bool multipleMouseSelect = false;
	float oldYMovement;
	int frameSkipCounter = 0;
	Vector2 initialMousePosition;
	Vector2 oldMousePosition;
	std::unique_ptr<ThreadsafeQueue<EditorMessage>> messageQueue;
	char newScriptBuffer[64];
	bool editorLocked = false;
	bool sceneNextFrame = false;
	MouseSettings mouseSettings;

	//other variables
	std::unique_ptr<GuiManager> guiManager;
	WindowSet<Editor> windows;
	GameSettings* settingsWindow;
	ObjectHierarchyWindow* objectHiWindow;
	ComponentWindow* componentWindow;
	std::string currentLevel;
	Input scriptInput;
	Camera* ingameCamera;

	void MenuBar(Window&);
	void HandleMessages();
	void UpdateMovement();
	void ReplaceCurrentWithTemp();
	Matrix4x4 identity_matrix;
	void Picking();

	void InitTestPlay();
	void UpdateTestPlay(float dt);
	void StopTestPlay();

public:
	// ImGui Gizmos
	bool enable_gizmos;
	ImGuizmo::MODE current_mode;
	ImGuizmo::OPERATION current_operation;
	bool gizmos_snapping = false;
	float gsnap[3]{ };

	bool target_rotation = false;
	Entity* ent_target = nullptr;
	int frame_skips = 1;
	int spacesSize = 1;										// Determine how many spaces for editor view
	int mainLayerNum;
	Camera camera;
	Input editor_input, game_input;
	bool playing, paused;
	bool multipleGamespaces;
	std::atomic<bool> previouslySaved;
	History history;

	EntityList* entityList;
	std::vector<Entity*> selectedEntities;
	std::vector<Entity*> selectedGroupEntities;

	Editor(const Window& window);
	void ResolutionChanged(const Vector2& dimension);
	void Update(Window& window, float dt);
	auto& GetGuiManager()														{ return *guiManager; }
	auto& GetAllGuiWindows()													{ return windows.GetAllGeneralWindows(); }
	void SaveConfig();
	void LoadConfig();
	void SaveComponentData(const std::string&);
	void SaveComponentData(Serializer&);
	bool LoadComponentData(const std::string&);
	bool LoadComponentData(Serializer&);
	void ClearObjects();
	void Render();
	bool IsPlaying() const														{ return (playing && !paused); }
	bool FrameSkipping() const													{ return sceneNextFrame; }
	Entity* CreateEntity(const std::string& = "");
	Entity* CreateEntity(const Serializer&);
	void CopyPrefab(const Entity&);
	inline bool EditorKeyLocked() const											{ return editorLocked; }
	void FocusOnSelectedEntity(Camera& cam, Entity* selected_ent = nullptr);
	void GrabDroppedFiles(const std::vector<std::string>& droppedFiles);
	void OpenIDE(const std::string& filename = "") const;
	void OpenScriptModalWindow();
  void RecursiveEditorSetup(Entity&);
	~Editor();
};

static void EditorShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

