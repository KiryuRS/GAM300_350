
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Serializer.h"

struct Action
{
	Serializer savedLayout;
	std::string name;
};

struct History
{
	Serializer currentDocument;
	std::vector<std::unique_ptr<Action>> actions;
	bool saveAction;
	std::string saveName;
	int selectedHistory, selectedHistoryRecord;
	History() :saveAction(false), selectedHistory{ -1 }, selectedHistoryRecord{ -1 }	
	{}
	void AddAction(const std::string& name)
	{
		saveName = name;
		saveAction = true;
	}
	template<typename ... Args>
	void AddAction(const Args&... args)
	{
		std::stringstream ss;
		using arr = char[];
		arr{ char(), (ss << args, char())... };
		saveName = ss.str();
		saveAction = true;
	}
	void Reset()
	{
		saveAction = false;
		actions.clear();
		selectedHistory = -1;
		selectedHistoryRecord = -1;
	}
};