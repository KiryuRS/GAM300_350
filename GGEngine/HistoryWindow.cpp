#include "stdafx.h"
#include "HistoryWindow.h"

void HistoryWindow::Display(Editor& edit)
{
	ImGui::Text("Previous Actions");
	std::vector<const char*> args;
	if (edit.history.actions.size())
	{ 
		for (auto& item : edit.history.actions)
			args.push_back(item->name.c_str());
		char** charArgs = const_cast<char**>(&args[0]);
		ImGui::ListBox("prev_actions", &edit.history.selectedHistory, charArgs, 
			(int)edit.history.actions.size(), historyNum, ImVec2{ ImGui::GetWindowWidth() * 0.95f, 0 });
		if (ImGui::Button("Clear History") && edit.history.selectedHistory)
		{
			edit.history.actions.erase(edit.history.actions.begin(), edit.history.actions.begin() + edit.history.selectedHistory);
			edit.history.selectedHistory = 0;
			edit.history.selectedHistoryRecord = edit.history.selectedHistory;
		}
	}
}

void HistoryWindow::Update(Editor& edit)
{
	if (edit.history.saveAction)
	{
		if (edit.history.selectedHistory != static_cast<int>(edit.history.actions.size()) - 1)
			edit.history.actions.erase(edit.history.actions.begin() + edit.history.selectedHistory + 1, edit.history.actions.end());
		edit.history.actions.emplace_back();
		edit.history.actions.back() = std::make_unique<Action>();
		edit.SaveComponentData(edit.history.actions.back()->savedLayout);
		edit.history.actions.back()->name = edit.history.saveName;
		edit.history.saveAction = false;
		if (static_cast<int>(edit.history.actions.size()) > historyNum)
			edit.history.actions.erase(edit.history.actions.begin());
		else
		{
			++edit.history.selectedHistory;
			++edit.history.selectedHistoryRecord;
		}
	}
	
	// Only want the keys to be active when there isn't any scene playing in editor mode
	if (!edit.EditorKeyLocked())
	{
		if (edit.editor_input.CheckCombination(KEY_LCTRL, KEY_Z) && edit.history.selectedHistory)
		{
			--edit.history.selectedHistory;
		}
		if (edit.editor_input.CheckCombination(KEY_LCTRL, KEY_Y)
			&& edit.history.selectedHistory < static_cast<int>(edit.history.actions.size()) - 1)
		{
			++edit.history.selectedHistory;
		}
	}
	if (edit.history.selectedHistory != edit.history.selectedHistoryRecord)
	{
		BROADCAST_MESSAGE(EditorMessage{EDITORMSG::LOADHISTORYDATA, "", edit.history.selectedHistory});
		edit.history.selectedHistoryRecord = edit.history.selectedHistory;
	}

}