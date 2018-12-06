#include "stdafx.h"
#include "ButtonManagerC.h"
#include "ButtonC.h"

void ButtonManagerC::UpdateIndex(int oldIndex, std::string textureType)
{
	// Update the old index from selected to none
	auto oldBtnComp = allButtons[oldIndex]->GetComponent<ButtonC>();
	oldBtnComp->SetBtnTexture("normal");

	// Update the new index from none to selected
	auto newBtnComp = allButtons[btnIndex]->GetComponent<ButtonC>();
	newBtnComp->SetBtnTexture(textureType);
}

ButtonManagerC::ButtonManagerC()
	: scriptComp{ nullptr }, btnIndex{ 0 }
{
	RequiredComponents(scriptComp);
}

void ButtonManagerC::Initialize()
{

}

void ButtonManagerC::Update(float)
{

}

void ButtonManagerC::Awake()
{
	// Grab all of the children components
	allButtons = owner->children;
	auto btnComp = allButtons[btnIndex]->GetComponent<ButtonC>();
	btnComp->SetBtnTexture("highlighted");
	// Update the rest of the buttonts to normal
	size_t size = allButtons.size();
	for (unsigned i = 0; i != size; ++i)
	{
		if (i == btnIndex)
			continue;
		auto comp = allButtons[i]->GetComponent<ButtonC>();
		comp->SetBtnTexture("normal");
	}
}

void ButtonManagerC::AddSerializeData(LuaScript * script)
{
	AddSerializable("Current Index", btnIndex, "", script).display = false;
	AddCallableFunction("ExecuteButton", &ButtonManagerC::ExecuteButton, script);
	AddCallableFunction("MoveUp", &ButtonManagerC::MoveUp, script);
	AddCallableFunction("MoveDown", &ButtonManagerC::MoveDown, script);
	AddCallableFunction("UpdateButton", &ButtonManagerC::UpdateButton, script);
	AddCallableFunction("GetIndex", &ButtonManagerC::GetIndex, script);
}

bool ButtonManagerC::EditorGUI(Editor & editor)
{
	bool success = false;

	ImGui::Text("Current Selection");

	// Grab the size of the children
	size_t btnSize = owner->children.size();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputInt("", &btnIndex);
	ImGui::PopID();

	if (btnIndex < 0)
		btnIndex = 0;
	else if (btnIndex >= btnSize)
		btnIndex = btnSize - 1;

	return success;
}

void ButtonManagerC::ExecuteButton()
{
	// Search through the entity
	if (allButtons.empty())
		return;

	auto btnComp = allButtons[btnIndex]->GetComponent<ButtonC>();
	btnComp->ExecuteButton();
}

void ButtonManagerC::MoveUp()
{
	int oldIndex = btnIndex;

	--btnIndex;
	if (btnIndex < 0)
		btnIndex = allButtons.size() - 1;
	UpdateIndex(oldIndex, "highlighted");
}

void ButtonManagerC::MoveDown()
{
	int oldIndex = btnIndex;

	++btnIndex;
	if (btnIndex >= allButtons.size())
		btnIndex = 0;
	UpdateIndex(oldIndex, "highlighted");
}

void ButtonManagerC::UpdateButton(int index, std::string textureType)
{
	if (allButtons.size() < index || index < 0)
		throw std::runtime_error{ "Attempting to replace index with out of bounds range" };

	int oldIndex = btnIndex;
	btnIndex = index;
	UpdateIndex(oldIndex, textureType);
}

int ButtonManagerC::GetIndex()
{
	return btnIndex;
}


