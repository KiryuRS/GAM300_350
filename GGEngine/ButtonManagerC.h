#pragma once

class ScriptC;

class ButtonManagerC : public Component
{
	/*
		To get the list of buttons available for command, append all the buttons to be the children of this class!
		And the order matters! For keyboard / gamepad toggling
	*/

	std::vector<Entity*>	allButtons;
	ScriptC*				scriptComp;
	int						btnIndex;

	void UpdateIndex(int oldIndex, std::string textureType);

public:
	ButtonManagerC();
	void Initialize() override;
	void Update(float dt) override;
	void Awake() override;
	void AddSerializeData(LuaScript* script = nullptr) override;
	bool EditorGUI(Editor& editor) override;

	void ExecuteButton();
	void MoveUp();
	void MoveDown();
	void UpdateButton(int index, std::string textureType);
	int GetIndex();

	static constexpr auto GetType()			{ return COMPONENTTYPE::BUTTONMANAGER; }
};