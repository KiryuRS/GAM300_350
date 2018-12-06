#pragma once
#include <filesystem>
#include "GuiWindow.h"
#include "ResourceObject.h"

class ResourceWindow :public GuiWindow
{
	std::vector<ResourceObject>				ResourceList;
	std::vector<FolderInfo>					allFolders;
	bool									ResourceLoaded = false;
	bool									currChanged;
	std::string								currDirectory;
	ResourceObject*							currentItem;
	std::vector<FolderInfo>::iterator		afIter;

	const std::vector<std::string> excluded_location{ EXCLUDE_CHECK_LOC };
	// Path Exclusion Checking
	bool CheckIfExcluded(const std::string& path) const;

public:
	static constexpr char windowName[] = "Resources";
	ResourceWindow(Editor& edit) :GuiWindow(edit, windowName), 
		currentItem{ nullptr }, currDirectory{"Resources"}, currChanged{ false }
	{
		LoadResources(fs::directory_entry{ fs::path(currDirectory) });
	}
	void Initialize(Editor&) override;
	void Display(Editor&) override;
	void LoadResources(fs::directory_entry Path);
	void Update(Editor&) override;
	void GrabFolderInformation();
};