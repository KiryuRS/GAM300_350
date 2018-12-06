#include "stdafx.h"
#include "ResourceUpdater.h"

ResourceUpdater::ResourceUpdater():
	running(false)
{
	filesChecker.Init();
	auto offlineFiles = filesChecker.GrabAllOfflineFiles();
	if (offlineFiles.size())
		modifiedFiles.insert(modifiedFiles.end(), offlineFiles.begin(), offlineFiles.end());
}

void ResourceUpdater::Update()
{
	while (running)
	{
		// Do not "run" this update if the window is freezed
		if (COREENGINE_S.IsWindowFreezed())
			continue;

		/*

			Steps:
			1) File Checker checks for any new uncompiled files
				- Grab all of the files within the Resource folder for checking
				- This step is purely to determine which files are modified / added into the project
			2) Convert all uncompiled files into our own custom format
				- Determine what are the available extensions to capture
				- Categorize them and store them into a vector
			3) Updates all existing files already loaded in the engine
				- Distribute all relevant files into the respective "compilers"
				- All these should be done within one frame (as much as possible)

		*/

		offWndMod = false;

		// (1)
		filesChecker.Update();
		auto new_modified_files = filesChecker.GrabModifiedFiles();
		if (new_modified_files.size())
		{
			offWndMod = true;
			modifiedFiles.insert(modifiedFiles.end(), new_modified_files.begin(), new_modified_files.end());
		}
#ifdef _DEBUG
		if (!modifiedFiles.empty())
		{
			for (auto& elem : modifiedFiles)
				if (!elem->new_addition)
					CONSOLE_LN(elem->path_filename, " has been modified!");
		}
#endif

		// (2)
		for (const auto& item : modifiedFiles)
		{
			//compiler.CompileFile(item->path_filename);
			BROADCAST_MESSAGE(CompilerMessage{COMPILERMSG::COMPILEFILE , item->path_filename });
		}
		if (offWndMod)
		{
			// Create the log file whenever something is updated
			filesChecker.SaveFileLogs();
		}
		modifiedFiles.clear();

		// (3)

		// Only update every 0.5s
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
