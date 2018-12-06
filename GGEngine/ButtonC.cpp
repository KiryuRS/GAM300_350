#include "stdafx.h"
#include "2DSpriteC.h"
#include "ComponentIdentifier.h"
#include "CollisionC.h"

void ButtonC::ChangeTexture()
{
	switch (state)
	{
	case BtnState_None:
		
		break;

	case BtnState_Selected:
		spriteComp->SetAndUpdateTexture(selectedTex, false);
		break;

	case BtnState_Highlighted:
		spriteComp->SetAndUpdateTexture(highlightedTex, false);
		break;

	case BtnState_Disabled:
		// For now disabled will just hide the texture
		spriteComp->SetVisibility(false);
		break;

	case BtnState_Enabled:
		// For now enabled will show the texture
		spriteComp->SetVisibility(true);
		break;

	case BtnState_Normal:
		spriteComp->SetAndUpdateTexture(normalTex, false);
		break;
	}

	// Reset the state
	state = BtnState_None;
}

bool ButtonC::GUIChooseTexture(BTNINTERNAL _state)
{
	bool success = false;
	auto& tList = GFX_S.GetTextureList();

	std::string displayTexture, *updateTexture = nullptr;
	int* modelNum = nullptr;
	
	switch (_state)
	{
	case BTNINTERNAL::NORMAL:
		displayTexture = "Default Texture";
		modelNum = &modelNumNormal;
		updateTexture = &normalTex;
		state = BtnState_Normal;
		preview = false;
		break;

	case BTNINTERNAL::HIGHLIGHTED:
		displayTexture = "Highlighted Texture";
		modelNum = &modelNumHighlighted;
		updateTexture = &highlightedTex;
		state = BtnState_Highlighted;
		preview = true;
		break;

	case BTNINTERNAL::SELECTED:
		displayTexture = "Selected Texture";
		modelNum = &modelNumSelected;
		updateTexture = &selectedTex;
		state = BtnState_Selected;
		preview = true;
		break;
	}

	ImGui::Text(displayTexture.c_str());
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::Combo("", modelNum, tList.data(), int(tList.size()), int(tList.size())))
	{
		success = true;
		*updateTexture = tList[*modelNum];
		UpdateSerializables(*updateTexture);
		UpdateSerializables(*modelNum);
		// If there's any changes, we would also want to update the Sprite2DC
		ChangeTexture();
		if (preview)
		{
			auto func = [this](void) -> void { this->ResetToNormalTexture(); };
			previewTimer.ResetTimer(true);
			previewTimer.AssignFunction(func);
			previewTimer.SetNewInterval(1.5);
		}
		// Update Sprite2D's component's texture
		if (_state == BTNINTERNAL::NORMAL)
			spriteComp->SetAndUpdateTexture(*updateTexture, true);
	}
	ImGui::PopID();
	return success;
}

void ButtonC::ProcessCommand()
{
	// Using the command and better enums combination
	ButtonBehaviour behaviour = ButtonBehaviour::_from_string_nocase(command.c_str());
	switch (behaviour)
	{
	case ButtonBehaviour::None:
		// Of coz this will do nothing
		break;

	case ButtonBehaviour::GoNext:
		// This will inform the GameStateManager to trigger another scene
#ifdef EDITOR

#else
		GSM_S.ChangeState(std::make_unique<GameScene>(nextFilename));
#endif
		break;

	case ButtonBehaviour::GoNextStack:
		// This will inform the GameStateManager to push a scene on top
#ifdef EDITOR

#else
		GSM_S.PushState(std::make_unique<GameScene>(nextFilename));
#endif
		break;

	case ButtonBehaviour::Trigger:
		
		break;

	case ButtonBehaviour::Quit:
		// Quit means we will quit the whole program
#ifdef EDITOR

#else
		GSM_S.SetQuit(true);
#endif
		break;
	}
}

void ButtonC::ResetToNormalTexture()
{
	spriteComp->SetAndUpdateTexture(normalTex, false);
}

ButtonC::ButtonC()
	: idComp{ nullptr }, state{ (ButtonState)0 }, spriteComp{ nullptr },
	  modelNumSelected{ 0 }, modelNumHighlighted{ 0 }, modelNumNormal{ 0 },
	  colliderComp{ nullptr }, guiFile{ }, guiCommand{ }, preview{ false }
{
	RequiredComponents(idComp, spriteComp);
}

bool ButtonC::EditorGUI(Editor & edit)
{
	bool success = false;
	
	success = GUIChooseTexture(BTNINTERNAL::NORMAL);
	success = GUIChooseTexture(BTNINTERNAL::HIGHLIGHTED);
	success = GUIChooseTexture(BTNINTERNAL::SELECTED);

	ImGui::Separator();
	ImGui::Text("Command");
	TOOLS::MyCopy(command.begin(), command.end(), guiCommand);
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::InputText("", guiCommand, MAX_PATH, ImGuiInputTextFlags_CharsNoBlank))
	{
		success = true;
		// Update our string file
		command = guiCommand;
		UpdateSerializables(command);
	}
	ImGui::PopID();

	ImGui::Text("Transverse Filename");
	TOOLS::MyCopy(nextFilename.begin(), nextFilename.end(), guiFile);
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::InputText("", guiFile, MAX_PATH, ImGuiInputTextFlags_CharsNoBlank))
	{
		success = true;
		// Update our string file
		nextFilename = guiFile;
		UpdateSerializables(nextFilename);
	}
	ImGui::PopID();

	previewTimer.Update();

	return success;
}

void ButtonC::Initialize()
{
	ChangeTexture();
}

void ButtonC::Update(float)
{
	//Input& input = INPUT_S;
	//// Left Clicked on the button
	//if (input.GetReleaseKey(KEY_LBUTTON))
	//{
	//	EntityList
	//	auto ent = entityList->PickEntity(input.GetMousePosition(), )
	//}
}

void ButtonC::AddSerializeData(LuaScript * luaScript)
{
	AddSerializable("Selected Texture", selectedTex, "", luaScript).display = false;
	AddSerializable("Highlighted Texture", highlightedTex, "", luaScript).display = false;
	AddSerializable("Default Texture", normalTex, "", luaScript).display = false;
	AddSerializable("Selected Texture Num", modelNumSelected, "", luaScript).display = false;
	AddSerializable("Highlighted Texture Num", modelNumHighlighted, "", luaScript).display = false;
	AddSerializable("Normal Texture Num", modelNumNormal, "", luaScript).display = false;
	AddSerializable("Transverse filename", nextFilename, "", luaScript).display = false;
	AddSerializable("Command", command, "", luaScript).display = false;

	AddCallableFunction("SetBtnTexture", &ButtonC::SetBtnTexture, luaScript);
	AddCallableFunction("SetBtnVisibility", &ButtonC::SetBtnVisibility, luaScript);
	AddCallableFunction("SetBtnTransparency", &ButtonC::SetBtnTransparency, luaScript);
}

void ButtonC::ExecuteButton()
{
	/*
		If there are transitions available before the execution, we will
		process the command after the transition is done
	*/
	state = BtnState_Selected;
	ChangeTexture();
	ProcessCommand();
}

std::string ButtonC::GetCurrentTexture() const
{
	std::string ret;

	switch (state)
	{
	case BtnState_Selected:
		ret = selectedTex;
		break;
	
	case BtnState_Highlighted:
		ret = highlightedTex;
		break;

	case BtnState_Normal:
		ret = normalTex;
		break;

	default:
		CONSOLE_ERROR("Something went wrong here! For ButtonC");
		break;
	}

	return ret;
}

void ButtonC::SetBtnTexture(std::string str)
{
	// change the str to lower case
	std::transform(str.begin(), str.end(), str.begin(), TOOLS::modifiedtolower);
	// Compare and check for similar comparison (then we change the state accordingly)
	switch (str[0])
	{
	case 's':
		state = BtnState_Selected;
		break;
	case 'h':
		state = BtnState_Highlighted;
		break;
	case 'n':
		state = BtnState_Normal;
		break;
	}
	ChangeTexture();
}

void ButtonC::SetBtnVisibility(bool toggle)
{
	state = toggle ? BtnState_Enabled : BtnState_Disabled;
}

void ButtonC::SetBtnTransparency(float value)
{
	spriteComp->SetTransparency(value);
}


