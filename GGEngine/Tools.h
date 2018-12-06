#pragma once
#include <sstream>
#include <random>
#include <fstream>
#include <strsafe.h>
#include <tchar.h>
#include "StackWalkerWrapper.h"

class Component;
class Input;
enum class COMPONENTTYPE;

// Anonymous namespace to prevent outside class from using it
namespace
{
	// Singleton class
	struct RandWrapper final
	{
		std::random_device rd;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dis;
		static std::unique_ptr<RandWrapper> rw;
		RandWrapper()
			: rd{}, gen{ rd() }, dis{ 0,1 }
		{ }
		static RandWrapper& GetRandWrapper()
		{
			// Check if there's already an instance
			if (!rw)
				rw = std::make_unique<RandWrapper>();
			return *rw;
		}
	};

	// Singleton class as well
	class FileSystemWrapper final
	{
		fs::path path_dir;
		std::tm time_info;
		__time64_t raw_time;
		char buffer[80];

	public:
		std::string full_path;

		static std::unique_ptr<FileSystemWrapper> fsw;
		FileSystemWrapper()
			: path_dir{ fs::current_path() },
			  full_path{ path_dir.generic_string() },
			  buffer{ 0 }
		{ }
		static FileSystemWrapper& GetFileSystemWrapper()
		{
			if (!fsw)
				fsw = std::make_unique<FileSystemWrapper>();
			return *fsw;
		}
		static void DestroyFileSystemWrapper()
		{
			if (fsw)
				fsw.reset();
		}
		std::string GetLocalTime()
		{
			time(&raw_time);
			_localtime64_s(&time_info, &raw_time);
			strftime(buffer, sizeof(buffer), "%d-%m-%Y %I%M%S", &time_info);
			std::string ret_time{ buffer };
			// Reset our time_info, raw_time and buffer
			time_info = std::tm{};
			raw_time = __time64_t{};
			SecureZeroMemory(buffer, sizeof(buffer));

			return ret_time;
		}
		void ClearAll()
		{
			full_path.clear();
			full_path = path_dir.generic_string();
		}
	};
}

enum class UITYPE { DEFAULT, COLLAPSINGHEADER, TREEHEADER };
enum class FILEDIALOGTYPE { OPEN, SAVEAS };

/*
	Include all misc. calculations and conversions that may be inappropiate to be
	in the class itself. Examples could be:
	- Random Generator
	- Graphics Pipeline Conversions
	- Misc. Conversions
	- Assertion
	- File System Wrapper functions
	- etc.
*/
namespace TOOLS
{
	/*
		Custom std::copy because MSC is being a bitch
	*/
	template <typename InputIt, typename OutputIt>
	OutputIt MyCopy(InputIt begin, InputIt end, OutputIt start)
	{
		while (begin != end)
			*start++ = *begin++;
		return start;

	}
	std::wstring StringToWString(const std::string& s);
	std::string TCharToString(TCHAR* path);
	// '\\' -> '/'
	std::string ReplaceBSlashWithFSlash(const std::string& path);
	// '/' -> '\\'
	std::string ReplaceFSlashWithBSlash(const std::string& path);
	//returns ".ext"
	std::string GetExtension(const std::string& filename);
	bool ConvertToCurrentDirectory(TCHAR* str, char* destination);

	/*
		Saves the messages into a filename and place it in the directory as specified.
		Example of usage:
		std::vector<std::string> messages{ <some code> };
		SaveData("SaveData/Log/", "logging.out", messages);

		*Note that directory must end with the back slash.
	*/
	bool SaveData(const std::string& directory, const std::string& filename, const std::vector<std::string>& messages);
	/*
		Removes a directory or file specified
		Examples of usage:
		RemoveDataorDirectory("SaveData/Log/");
		RemoveDataOrDirectory("SaveData/Log/logging.out");
		RemoveDataOrDirectory("logging.out");
		*Note that str must end with the back slash (if its a directory)
	*/
	bool RemoveDataOrDirectory(const std::string& str);

	// Generate a distribution between <min> and <max> inclusive, treated as float
	float RandFloat(float min, float max);
	// Generate a distribution between <min> and <max> inclusive, treated as int
	int RandInt(int min, int max);

	// Mimic the actual assertion system (but not actually using it)
	void Assert(bool condition, const std::string& msg);

	// Variadic Assertion printing (assertion will activate if condition is false)
	template<typename ... Args>
	void Assert(bool condition, const Args& ... printouts)
	{
		std::stringstream ss;
		// C++17 Fold Expression
		(ss << ... << printouts);
		Assert(condition, ss.str());
	}

	template<typename T>
	void PrintError(const std::string& errorLocation, const T& errorItem)
	{
		CONSOLE_ERROR(errorLocation, ": Error code ", errorItem, "\n");
	}
	//searches for a substring and replaces it
	void SearchAndReplace(std::string& updateString, const std::string& eraseStr,
		const std::string& replacement);
	void PushID(UITYPE);
	unsigned GetID(UITYPE type);
	bool BrowseFolder(std::string& destination, FILEDIALOGTYPE dialogType, TCHAR* startFolder = nullptr);
	void AddToolTip(const std::string& str);	
	void CreateErrorMessageBox(const std::string& str);
	void CreateInfoMessageBox(const std::string& str);
	std::optional<bool> CreateYesNoMessageBox2(const std::string& str);
	template<typename ... Args>
	void CreateInfoMessageBox(Args... args)
	{
		std::stringstream ss;
		// C++17 fold expressions
		(ss << ... << args);
		CreateInfoMessageBox(ss.str());
	}
	template<typename ... Args>
	void CreateErrorMessageBox(Args... args)
	{
		std::stringstream ss;
		// C++17 fold expressions
		(ss << ... << args);
		CreateErrorMessageBox(ss.str());
	}
	template<typename ... Args>
	std::optional<bool> CreateYesNoMessageBox(Args&&... args)
	{
		std::stringstream ss;
		// C++17 fold expressions
		(ss << ... << args);
		return CreateYesNoMessageBox2(ss.str());
	}
	std::wstring ConvertToWString(const std::string& as);
	std::string ExtractFileName(const std::string&);
	template<unsigned sz>
	void StringToTChar(const std::string& str, TCHAR(&arr)[sz])
	{
		std::copy(str.begin(), str.end(), arr);
		arr[str.size()] = 0;
	}
	bool ConvertToCurrentDirectory(std::string& actualDir);
	std::string GetAbsolutePath(const std::string& appender);
	std::string OnlyFileName(const std::string& actualDir);
	std::vector<std::string> SplitString(const std::string& string, const std::string& splitter);
	char modifiedtolower(char c);
	/*
		Creates a script with the given template (filename to NOT include the extension!)
		conflict would determine if the given filename has conflicting naming issue - does not affect the creation
	*/
	std::string CreateScriptTemplate(const std::string& filename = std::string{ "" }, bool* conflict = nullptr);
	template<typename T>
	T* FindComponent(std::vector<std::unique_ptr<Component>>& vec)
	{
		auto iter = std::find_if(vec.begin(), vec.end(),
			[](auto&item) {if (item->GetType() == T::GetType()) return true; return false; });
		if (iter == vec.end())
			return nullptr;
		else
			return static_cast<T*>(iter->get());
	}

	Component* FindComponent(COMPONENTTYPE comp, std::vector<std::unique_ptr<Component>>& vec);
}