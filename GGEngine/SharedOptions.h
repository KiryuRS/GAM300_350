#pragma once
#include "GuiWindow.h"
#include "Serializable.h"

class SharedOptions : public GuiWindow
{
	char searchPhrase[255];
	std::array<char, 256> directory;
	std::vector<std::string> xmlPaths;
	std::vector<std::pair<std::string, unsigned>> displayText;
	Serializable animGui;
	std::string mainAnim;
	std::vector<std::string> animList;
	std::vector<std::string> animNames;
	void RecursiveXmlSearch(const std::string&);
public:
	static constexpr char windowName[] = "Options";
	SharedOptions(Editor& edit);
	void Display(Editor&) override;
};