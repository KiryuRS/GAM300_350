#pragma once

class IdentifierC;
class Sprite2DC;
class ColliderAABB;

enum ButtonState
{
	BtnState_None					= 1 << 0,
	BtnState_Selected				= 1 << 1,
	BtnState_Highlighted			= 1 << 2,
	BtnState_Disabled				= 1 << 3,
	BtnState_Normal					= 1 << 4,
	BtnState_Enabled				= 1 << 5,
};

// Button Behaviour
BETTER_ENUM(ButtonBehaviour, char, None = 0, GoNext, Trigger, GoNextStack, Pop, Quit)

class ButtonC : public Component
{
	enum class BTNINTERNAL
	{
		NORMAL = 0,
		HIGHLIGHTED = 1,
		SELECTED = 2
	};

	ButtonState		state;
	std::string		selectedTex;
	std::string		highlightedTex;
	std::string		normalTex;
	IdentifierC		*idComp;
	Sprite2DC		*spriteComp;
	ColliderAABB	*colliderComp;
	std::string		nextFilename;
	std::string		command;

	char			guiFile[MAX_PATH];
	char			guiCommand[32];
	char			guiFilename[32];
	bool			preview;
	Timer			previewTimer;
	
	int modelNumSelected, modelNumHighlighted, modelNumNormal;

	void ChangeTexture();
	bool GUIChooseTexture(BTNINTERNAL _state);
	void ProcessCommand();

public:
	ButtonC();
	bool EditorGUI(Editor& edit) override;
	void Initialize() override;
	void Update(float dt) override;
	void AddSerializeData(LuaScript* luaScript = nullptr) override;
	void ResetToNormalTexture();

	// To be called from Button Manager
	void ExecuteButton();
	std::string GetCurrentTexture() const;

	// ==== FOR LUA SCRIPTING ====
	void SetBtnTexture(std::string str);
	void SetBtnVisibility(bool toggle);
	void SetBtnTransparency(float value);
	// ==== END LUA SCRIPTING ====

	static constexpr auto GetType()			{ return COMPONENTTYPE::BUTTON; }
};