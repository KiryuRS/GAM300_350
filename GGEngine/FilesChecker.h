#pragma once
#include <map>
#include <vector>
#include <Windows.h>
#include <future>


// If there are more than one location to exclude, indicate in here with a comma
#define EXCLUDE_CHECK_LOC	\
LOGGING_DIRECTORY, "Resources/TestStation/", "Resources/Templates/"

// Updating on every frame time instead of every frame
#define FILES_TIME_FRAME_INTERVAL		500

static inline const char* RESOURCE_PATH{ "Resources/" };
struct FileInfo;

/*
	Return Values:
	-1 -> First SYSTEMTIME is earlier than second SYSTEMTIME
	0  -> Both SYSTEMTIMEs are the same
	1  -> First SYSTEMTIME is later than second SYSTEMTIME
*/
LONG CompareSystemTime(const SYSTEMTIME& lhs, const SYSTEMTIME& rhs);
bool GetLastWriteTime(FileInfo& file);
constexpr unsigned GetNumBackSlashes(const char* str)
{
	unsigned counter = 0;
	for (; *str != '\0'; ++str)
		if (*str == '\\') ++counter;
	return counter;
}

// Details about a file's information
struct FileInfo
{
	std::string path_filename;
	// In the WINDOWS format - able to retrieve the individual time components [Stored in CREATE - ACCESSED - MODIFY]
	std::array<SYSTEMTIME, 3> stCAM;
	// In String Format (for printing?) [Stored in CREATE - ACCESSED - MODIFY]
	std::array<std::string, 3> strCAM;
	// Modifier Stuff
	bool lastest_update = false, exist = true, new_addition = false, in_use = false;

	// To inform the system that this file's new information has been reflected into the system
	void FileUpdated();
};

// Details about a folder's information
struct FolderInfo
{
	std::string path_filename;
	unsigned levelTier;
	bool selected;

	FolderInfo(const std::string& filename = "", unsigned level = 0)
		: path_filename{ filename }, levelTier{ level }, selected{ false }
	{ }

	bool operator==(const FolderInfo& rhs) const
	{
		return rhs.path_filename == path_filename;
	}

	bool operator==(const std::string& rhs) const
	{
		return path_filename == TOOLS::ReplaceBSlashWithFSlash(rhs);
	}

	bool operator!=(const FolderInfo& rhs) const
	{
		return !operator==(rhs);
	}

	bool operator!=(const std::string& rhs) const
	{
		return !operator==(rhs);
	}
};

class FilesChecker
{
	std::map<std::string, FileInfo> all_files;					// Capturing all kinds of files from the resource folders
	const std::vector<std::string> excluded_location{ EXCLUDE_CHECK_LOC };
	std::string offline_filename{ "allFiles.log" };

	// Path Exclusion Checking
	bool CheckIfExcluded(const std::string& path) const;
	// Compares with the files in the previous run
	std::vector< FileInfo > GrabOfflineFiles() const;
	// Contains all the files that were modified offline
	std::vector< FileInfo* > offlineFiles;
	// Contains all the folders
	std::vector<FolderInfo> allFolders;

public:
	FilesChecker();
	~FilesChecker();
	void Init();
	void Update();
	void UpdateFolders();
	/*
		path_folder determines the sub-directory to check for any files being modified.
		Default argument is set to no string (which means to check EVERY single file in resource folder)
		
		The function may return an empty vector that indicates no files have been modified (out of the engine)
	*/
	std::vector<FileInfo*> GrabModifiedFiles(const std::string& path_folder = "");
	std::vector<FileInfo*> GrabAllOfflineFiles() const;
	std::vector<FolderInfo> GrabAllFolders(std::promise<bool>& prom);
	void SaveFileLogs();
};