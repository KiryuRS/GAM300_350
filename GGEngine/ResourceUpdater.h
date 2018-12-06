#pragma once
#include "FilesChecker.h"
#include "Updater.h"

class ResourceUpdater
{
	FilesChecker filesChecker;
	Updater updater;
	std::vector<FileInfo*> modifiedFiles;
	std::vector<FolderInfo> allFolders;
	bool offWndMod = false;

public:

	std::atomic<bool> running;
	ResourceUpdater();
	void Update();
	inline FilesChecker* GetFCInstance()			{ return &filesChecker; }
};