#include "stdafx.h"

bool FilesChecker::CheckIfExcluded(const std::string & path) const
{
	for (const auto& elem : excluded_location)
	{
		// Check if any of the excluded path lies within path
		size_t pos = path.find(elem.substr(0, elem.length() - 1));
		if (pos != std::string::npos)
			return true;
	}

	// If it reaches here that means the path should NOT be excluded
	return false;
}

std::vector<FileInfo> FilesChecker::GrabOfflineFiles() const
{
	std::string full_path{ fs::current_path().generic_string() };
	full_path += "/" + std::string{ LOGGING_DIRECTORY } + offline_filename;
	// Read from the file
	std::ifstream infile;
	infile.open(full_path);
	std::string curr_line;
	FILETIME filetime{};
	std::vector< FileInfo > file_grabber;
	while (std::getline(infile, curr_line))
	{
		if (curr_line.find("[") != std::string::npos)
			file_grabber.emplace_back();
		// Capturing the path_filename
		if (TOOLS::GetExtension(curr_line).size())
			file_grabber.back().path_filename = curr_line;
		if (size_t pos = curr_line.find("LOW - "); pos != std::string::npos)
		{
			std::string sub{ curr_line.substr(pos + strlen("LOW - ")) };
			filetime.dwLowDateTime = std::stoul(sub);
		}
		else if (pos = curr_line.find("HIGH - "); pos != std::string::npos)
		{
			std::string sub{ curr_line.substr(pos + strlen("HIGH - ")) };
			filetime.dwHighDateTime = std::stoul(sub);
			FileTimeToSystemTime(&filetime, &file_grabber.back().stCAM[2]);
		}
	}
	infile.close();
	
	return file_grabber;
}

std::vector<FolderInfo> FilesChecker::GrabAllFolders(std::promise<bool>& prom)
{
	UpdateFolders();
	prom.set_value(true);
	return allFolders;
}

FilesChecker::FilesChecker()
	: all_files{ }
{ }

FilesChecker::~FilesChecker()
{
	SaveFileLogs();
}

void FilesChecker::Init()
{
	auto vec{ std::move(GrabOfflineFiles()) };
	std::string full_path{ fs::current_path().generic_string() };
	full_path += "/" + std::string{ LOGGING_DIRECTORY } + offline_filename;
	bool file_exist = fs::exists(full_path);

	for (auto& path : fs::recursive_directory_iterator(RESOURCE_PATH))
	{
		// First, convert the path's slashes to the correct one
		std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(path.path().string());
		// Skip checking if its part of the exluded directory
		if (CheckIfExcluded(alt_path))
			continue;
		// Check if its a file (and not a directory)
		if (fs::is_directory(path.symlink_status()))
			continue;

		// Store the information into the FileInfo
		auto& file_info = all_files[alt_path];
		file_info.path_filename = alt_path;
		GetLastWriteTime(file_info);
		file_info.lastest_update = true;

		// Compare it with the offline files (only if the log file exist)
		if (file_exist)
		{
			auto iter = std::find_if(vec.begin(), vec.end(), [&file_info](const FileInfo& rhs)
			{
				return file_info.path_filename == rhs.path_filename;
			});

			// Add it into the new_files if its a new set of file or modified ones
			if (iter == vec.end())
			{
				offlineFiles.emplace_back(nullptr);
				offlineFiles.back() = &all_files[alt_path];
			}
			else
			{
				// Compare the timing
				LONG result = CompareSystemTime(file_info.stCAM[2], iter->stCAM[2]);
				if (result > 0)
				{
					offlineFiles.emplace_back(nullptr);
					offlineFiles.back() = &all_files[alt_path];
				}
			}
		}
	}
}

void FilesChecker::Update()
{
	for (auto iter = all_files.begin(); iter != all_files.end();)
	{
		auto&[filename, file_info] = *iter;
		if (!file_info.exist)
		{
			std::cout << "File deleted from the system!\n" << file_info.path_filename << "\n";
			iter = all_files.erase(iter);
			continue;
		}
		++iter;
	}

	// Checking for all existing files (to update their modified time)
	for (auto&[filename, file_info] : all_files)
	{
		// "Update" our file_information to the latest "build" if the file isn't in use anymore
		if (!file_info.lastest_update && !file_info.in_use)
			file_info.FileUpdated();

		// Default set all of the exist state to false
		file_info.exist = false;
	}

	// Grab all instances of the files in the folder
	for (auto path : fs::recursive_directory_iterator(RESOURCE_PATH))
	{
		// First, convert the path's slashes to the correct one
		std::string alt_path = TOOLS::ReplaceBSlashWithFSlash(path.path().string());
		// Skip checking if its in the log directory
		if (CheckIfExcluded(alt_path))
			continue;
		// Check if its a file (and not a directory)
		if (fs::is_directory(path.symlink_status()))
			continue;

		// Create an instance of the FileInfo
		FileInfo new_file{};
		new_file.path_filename = alt_path;
		bool success = GetLastWriteTime(new_file);

		// First determine if the file exist inside our map
		auto iter = std::find_if(all_files.begin(), all_files.end(), [&new_file](const std::pair<std::string, FileInfo>& elem)
		{
			auto&[filename, file_info] = elem;
			return filename == new_file.path_filename;
		});
		// If it's not inside the all_files, we add it (and declare that is not updated)
		if (iter == all_files.end())
		{
#ifdef _DEBUG
			std::cout << "One additional file has been added!\n" << alt_path << "\n";
#endif
			new_file.new_addition = true;
			all_files.insert(std::pair<std::string, FileInfo>(alt_path, new_file));
		}
		// Otherwise we check for the modified timing
		else
		{
			auto&[filename, file_info] = *iter;
			// First, confirm that the file still exist
			file_info.exist = true;

			// If "success" fails, we assume that it is currently in use
			file_info.in_use = !success ? true : false;
			// Compare the date of the two files (based on the modified files)
			LONG result = CompareSystemTime(file_info.stCAM[2], new_file.stCAM[2]);
			if (result < 0 || !success)
			{
				// Indicate that the file is not updated and will be updated in the next frame
				file_info.lastest_update = false;
			}
		}

	}
}

void FilesChecker::UpdateFolders()
{
	allFolders.clear();
	std::vector<FolderInfo> tmp;

	for (auto& path : fs::recursive_directory_iterator(RESOURCE_PATH))
	{
		std::string alt_path = path.path().string();
		if (CheckIfExcluded(alt_path))
			continue;
		// We would only want to capture the folders
		if (!fs::is_directory(path.symlink_status()))
			continue;
		// Making use of emplace_back's functionality
		tmp.emplace_back(alt_path, GetNumBackSlashes(alt_path.c_str()));
	}

	allFolders = std::move(tmp);
}

std::vector<FileInfo*> FilesChecker::GrabModifiedFiles(const std::string & path_folder)
{
	bool all_path = !path_folder.empty();
	std::vector<FileInfo*> ret_files;

	for (auto&[filename, file_info] : all_files)
	{
		// We only want to find the specific path (if there's one)
		if (!all_path && filename.find(path_folder) == std::string::npos)
			continue;

		// Check the lastest_update
		if (!file_info.lastest_update)
		{
			ret_files.emplace_back();
			ret_files.back() = &file_info;
		}
	}

	return ret_files;
}

std::vector<FileInfo*> FilesChecker::GrabAllOfflineFiles() const
{
	return offlineFiles;
}

void FilesChecker::SaveFileLogs()
{
	// We would want to capture the information of all of the files just before we close the program
	std::vector<std::string> file_append;
	unsigned long counter = 0;

	for (auto&[filename, fileInfo] : all_files)
	{
		// Grab the last write time
		GetLastWriteTime(fileInfo);
		FILETIME fileTime{};
		SystemTimeToFileTime(&fileInfo.stCAM[2], &fileTime);

		// Save the information into the file
		file_append.emplace_back("[" + std::to_string(counter++) + "]\n");
		file_append.emplace_back(fileInfo.path_filename + "\n");
		file_append.emplace_back("{FILEITME}\n");
		file_append.emplace_back("\tLOW - " + std::to_string((DWORD)fileTime.dwLowDateTime) + "\n");
		file_append.emplace_back("\tHIGH - " + std::to_string((DWORD)fileTime.dwHighDateTime) + " \n");
		file_append.emplace_back("\n");
	}

	std::string full_path{ fs::current_path().generic_string() };
	full_path += "/" + std::string{ LOGGING_DIRECTORY } + offline_filename;
	std::ofstream outfile;
	outfile.open(full_path);
	for (const auto& elem : file_append)
		outfile << elem;
	outfile.close();
}

void FileInfo::FileUpdated()
{
	bool update = GetLastWriteTime(*this);
	if (update)
	{
		lastest_update = true;
		new_addition = false;
	}
}

LONG CompareSystemTime(const SYSTEMTIME & lhs, const SYSTEMTIME & rhs)
{
	FILETIME ftlhs{}, ftrhs{};
	SystemTimeToFileTime(&lhs, &ftlhs);
	SystemTimeToFileTime(&rhs, &ftrhs);
	return CompareFileTime(&ftlhs, &ftrhs);
}

bool GetLastWriteTime(FileInfo & file)
{
	// Read, Write, Modify
	std::array<FILETIME, 3> ftCAM;
	SYSTEMTIME stUTC, stLocal;
	DWORD dwRet;
	HANDLE hFile;
	std::wstring wide_str = TOOLS::ConvertToWString(file.path_filename);

	// Attempt to read the file first
	hFile = CreateFile(wide_str.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Unable to read the file with CreateFile() function!" << std::endl;
		return false;
	}

	// Retrieve the file times for the file
	// https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-getfiletime
	if (!GetFileTime(hFile, &ftCAM[0], &ftCAM[1], &ftCAM[2]))
	{
		std::cout << "Something went wrong here! Unable to call for GetFileTime()" << std::endl;
		CloseHandle(hFile);
		return false;
	}

	// Store all 3 information into FileInfo
	for (unsigned i = 0; i != 3; ++i)
	{
		// Convert the last-write time to local time
		FileTimeToSystemTime(&ftCAM[i], &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		// Build a string showing the date and time
		TCHAR lpszString[MAX_PATH];
		SecureZeroMemory(lpszString, MAX_PATH);
		dwRet = StringCchPrintf(lpszString, MAX_PATH, TEXT("%02d/%02d/%d  %02d:%02d"), stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute);
		if (dwRet != S_OK)
		{
			std::cout << "Something went wrong here! Unable to call StringCchPrintf()" << std::endl;
			CloseHandle(hFile);
			return false;
		}

		// Save the information into our file
		file.stCAM[i] = stLocal;
		file.strCAM[i] = TOOLS::TCharToString(lpszString);
	}

	CloseHandle(hFile);
	return true;
}
