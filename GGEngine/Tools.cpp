#include "stdafx.h"
#include "Tools.h"
#include "FilesChecker.h"

std::unique_ptr<RandWrapper> RandWrapper::rw;
std::unique_ptr<FileSystemWrapper> FileSystemWrapper::fsw;

namespace TOOLS
{
	std::wstring StringToWString(const std::string& s)
	{
		int slength = (int)s.length() + 1;
		int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		std::vector<wchar_t> buf;
		buf.assign(len, wchar_t{});
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &buf[0], len);
		std::wstring r{ buf.data() };
		return r;
	}

	std::string TCharToString(TCHAR * path)
	{
		char mbbufer[MAX_PATH];
		size_t length;

		// WCHAR Conversion to char
		wcstombs_s(&length, mbbufer, MAX_PATH, path, MAX_PATH);
		std::string actualDir = { mbbufer };
		return ReplaceBSlashWithFSlash(actualDir);
	}

	std::string ReplaceBSlashWithFSlash(const std::string & path)
	{
		std::string actualDir = path;
		// Replace all instance of "\" with "/"
		for (auto& elem : actualDir)
			if (elem == static_cast<char>('\\'))
				elem = '/';
		return std::move(actualDir);
	}

	std::string ReplaceFSlashWithBSlash(const std::string & path)
	{
		std::string actualDir = path;
		// Replace all instance of "/" with "\"
		for (auto& elem : actualDir)
			if (elem == static_cast<char>('/'))
				elem = '\\';
		return std::move(actualDir);
	}

	std::string GetExtension(const std::string & filename)
	{
		std::string extension;
		size_t pos = filename.rfind(".");
		if (pos != std::string::npos)
			extension = filename.substr(pos);
		return extension;
	}

	bool ConvertToCurrentDirectory(TCHAR * str, char * destination)
	{
		bool returnBool = false;
		TCHAR savefile_directory[MAXBUFFER];
		GetCurrentDirectory(MAXBUFFER, savefile_directory);
		// Replaces the saveDirectory with the specified
		std::string actualDir{ TCharToString(str) };
		std::string ss = TCharToString(savefile_directory);
		size_t pos = actualDir.rfind(ss);

		// Throws an error if there are no save file
		if (pos == std::string::npos)
		{
			pos = actualDir.length();
			CONSOLE_ERROR("Invalid file path specified!");
		}
		else
		{
			pos += ss.length() + 1;
			returnBool = true;
		}
		// Otherwise we substr and reassign to saveDirectory
		actualDir = actualDir.substr(pos);
		strcpy_s(destination, MAXBUFFER, actualDir.c_str());
		return returnBool;
	}

	bool ConvertToCurrentDirectory(std::string& actualDir)
	{
		bool returnBool = false;
		TCHAR savefile_directory[MAXBUFFER]{ };
		GetCurrentDirectory(MAXBUFFER, savefile_directory);
		// Replaces the saveDirectory with the specified
		std::string ss = TCharToString(savefile_directory);
		size_t pos = actualDir.rfind(ss);

		// Throws an error if there are no save file
		if (pos == std::string::npos)
		{
			pos = actualDir.length();
			CONSOLE_ERROR("Cannot add from files outside of the game folder!");
		}
		else
		{
			pos += ss.length() + 1;
			returnBool = true;
		}
		// Otherwise we substr and reassign to saveDirectory
		actualDir = actualDir.substr(pos);
		return returnBool;
	}

	std::string GetAbsolutePath(const std::string & appender)
	{
		TCHAR savefile_directory[MAXBUFFER]{ };
		GetCurrentDirectory(MAXBUFFER, savefile_directory);
		// Replaces the saveDirectory with the specified
		std::string ss = TCharToString(savefile_directory);
		ss += "/" + appender;
		return ss;
	}

	std::string OnlyFileName(const std::string& actualDir)
	{
		auto pos = actualDir.rfind("\\") + 1;
		if (pos != std::string::npos)
			return actualDir.substr(pos);
		return std::string{};
	}

	std::vector<std::string> SplitString(const std::string & string, 
		const std::string & splitter)
	{
		std::vector<std::string> returnVec;
		auto iter = string.find(splitter);
		size_t prevIter = 0;
		while (iter != std::string::npos)
		{
			returnVec.emplace_back(string.substr(prevIter, iter - prevIter));
			prevIter = iter + 2;
			iter = string.find(splitter, iter + 2);
		}
		returnVec.emplace_back(string.substr(prevIter, string.size() - prevIter));
		return returnVec;
	}

	char modifiedtolower(char c)
	{
		return static_cast<char>(std::tolower(c));
	}

	std::string CreateScriptTemplate(const std::string& filename, bool* conflict)
	{
		std::string new_filename = filename.size() ? filename : "NewScript";
		// First check if there's already an existing filename
		for (auto it = fs::directory_iterator(SCRIPT_DIRECTORY); it != fs::directory_iterator(); ++it)
		{
			const fs::path& path = *it;
			std::string curr_filename = path.string();
			
			// Substr the curr_filename
			curr_filename = curr_filename.substr(curr_filename.rfind("/") + 1);
			size_t extension_pos = curr_filename.find(SCRIPT_EXTENSION);
			if (extension_pos == std::string::npos)
				continue;
			curr_filename = curr_filename.substr(0, extension_pos);

			// Checking the filename with the new_filename
			if (new_filename == curr_filename)
			{
				*conflict = true;
				size_t last_pos = new_filename.size() - 1;
				// Append a new value
				int assumed_num = std::isdigit(new_filename[last_pos]);
				if (assumed_num)
					new_filename[last_pos] = (char)++assumed_num;
				else
					new_filename.append(std::to_string(++assumed_num));
				break;
			}
		}

		// Now we create our new file
		std::string new_file = SCRIPT_DIRECTORY;
		new_file += new_filename;
		new_file += SCRIPT_EXTENSION;

		std::string template_file = TEMPLATE_DIRECTORY;
		template_file += "lua.template";
		
		// Copy the contents over from the template file to the new file
		fs::copy_file(template_file, new_file);

		return new_filename += SCRIPT_EXTENSION;
	}

	Component * FindComponent(COMPONENTTYPE comp, std::vector<std::unique_ptr<Component>>& vec)
	{
		auto iter = std::find_if(vec.begin(), vec.end(),
			[&](auto&item) {if (item->GetType() == comp) return true; return false; });
		if (iter == vec.end())
			return nullptr;
		else
			return iter->get();
	}

	bool SaveData(const std::string & directory, const std::string & filename, const std::vector<std::string>& messages)
	{
		// Sanity Checks
		if (!directory.size() ||  directory.back() != '/')
		{
			Assert(false, "Invalid format of directory save!");
			return false;
		}
		if (GetExtension(filename).empty())
		{
			Assert(false, "No extension of file found!");
			return false;
		}

		auto& fsw = FileSystemWrapper::GetFileSystemWrapper();
		fsw.full_path += '/' + directory;
		// Create the directory if it doesn't exist
		if (!fs::exists(fsw.full_path))
			fs::create_directories(fsw.full_path);

		// Creating the file
		std::ofstream outfile;
		outfile.open(fsw.full_path + filename);
		// Write every single data inside the vector of strings into the file
		for (const auto& elem : messages)
			outfile << elem;
		outfile.close();
		fsw.ClearAll();
		fsw.DestroyFileSystemWrapper();
		return true;
	}

	bool RemoveDataOrDirectory(const std::string & str)
	{
		// Check for empty str
		if (str.empty())
		{
			CONSOLE_ERROR("Attempting to remove files / folders with no path specified!");
			return false;
		}

		auto& fsw = FileSystemWrapper::GetFileSystemWrapper();
		fsw.full_path += '/' + str;

		// Check if the directory exists or the filename exists
		if (!fs::exists(fsw.full_path))
		{
			CONSOLE_ERROR("Attempting to remove files / folders but no such file or directory exists!");
			return false;
		}
		
		fs::remove(fsw.full_path);
		fsw.ClearAll();
		return true;
	}

	float RandFloat(float min, float max)
	{
		// Grab the random instance
		RandWrapper& rw_inst = RandWrapper::GetRandWrapper();
		rw_inst.dis = std::uniform_real_distribution<float>{ min, max };
		return rw_inst.dis(rw_inst.gen);
	}

	int RandInt(int min, int max)
	{
		// Grab the random instance
		RandWrapper& rw_inst = RandWrapper::GetRandWrapper();
		rw_inst.dis = std::uniform_real_distribution<float>{ 0, 1 };
		return min + (int)roundf(rw_inst.dis(rw_inst.gen) * static_cast<float>(max - min));
	}

	void Assert(bool condition, const std::string & msg)
	{
		/*
			Display a message box instead to show the error that we want to show.
			This function would mimic the C++ assertion similarly except for the fact
			that the customized version will not terminate the program instantly
		*/

		// Only display the message if the condition turns out to be false
		if (condition)
			return;

		// Temporary printouts for debugging
		SYSTEMLOG_S.PrintLog(true, DebugTypes::_from_string("DT_ASSERTION"), msg);
		
		// Call our Stack Walker Wrapper (so to append our information to the assertion screen)
		StackWalkerWrapper sww;
		sww.ShowCallstack();

		// Modify our messages to display the call stack first
		std::string msg_with_stack{ sww.GetSWBuffer() };
		msg_with_stack += "\n";
		msg_with_stack += msg;

		// We display the message (and cater for user's input on the buttons?)
		/*
			https://msdn.microsoft.com/en-us/library/windows/desktop/ms645505%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
			for more information on message box
		*/
		int msgbox_id = MessageBox(COREENGINE_S.GetWindow().GetWindowHandle(),
								   StringToWString(msg_with_stack).c_str(),
								   (LPCWSTR)L"ASSERTION ERROR",
								   MB_ICONSTOP | MB_OKCANCEL | MB_DEFBUTTON1);

		// Create our filename for assertion error
		std::string filename{ "assert_error_" };
		// Append the current time to the filename
		filename += FileSystemWrapper::GetFileSystemWrapper().GetLocalTime();
		// Once we finished using, destroy it (to prevent any unforeseen errors)
		FileSystemWrapper::DestroyFileSystemWrapper();
		// Specify our format
		filename += ".log";

		// Save our data
		std::vector<std::string> messages;
		messages.emplace_back(msg_with_stack);
		SaveData(LOGGING_DIRECTORY, filename, messages);

		// Depending on what the action from the user, handle it properly
		switch (msgbox_id)
		{
			// User clicked on OK (means to continue as per normal)
		case IDOK:
			CONSOLE_WARNING("Assertion error occured with message: ", msg);
			break;

			// User clicked CANCEL (immediately throw error to trigger the "breakpoint")
		case IDCANCEL:
			// Save whatever remaining datafiles we have
			SYSTEMLOG_S.WriteToFile(false);
			throw EngineException{ ASSERTION_ERROR, msg_with_stack, msg };
			break;
		}
	}

	void SearchAndReplace(std::string& updateString, const std::string& eraseStr,
		const std::string& replacement)
	{
		auto num = updateString.find(eraseStr);
		if (num != std::string::npos)
			updateString.replace(num, eraseStr.length(), replacement);
	}

	void PushID(UITYPE type)
	{
		auto& guiManager = COREENGINE_S.GetEditor()->GetGuiManager();
		switch (type)
		{
		case UITYPE::DEFAULT:
			ImGui::PushID(guiManager.defaultNum++);
			break;
		case UITYPE::COLLAPSINGHEADER:
			ImGui::PushID(guiManager.collapseNum++);
			break;
		case UITYPE::TREEHEADER:
			ImGui::PushID(guiManager.treeNum++);
			break;
		}
	}

	unsigned GetID(UITYPE type)
	{
		auto& guiManager = COREENGINE_S.GetEditor()->GetGuiManager();
		switch (type)
		{
		case UITYPE::DEFAULT:
			return guiManager.defaultNum++;
			break;
		case UITYPE::COLLAPSINGHEADER:
			return guiManager.collapseNum++;
			break;
		default:
		case UITYPE::TREEHEADER:
			return guiManager.treeNum++;
			break;
		}
	}

	bool BrowseFolder(std::string& destination, FILEDIALOGTYPE dialogType, TCHAR* startFolder)
	{
		CoreEngine& engine = COREENGINE_S;
		bool result = false;

		// Set the default directory to our SaveFile directory
		TCHAR savefile_directory[MAXBUFFER];
		GetCurrentDirectory(MAXBUFFER, savefile_directory);
		if (startFolder)
			lstrcat(savefile_directory, startFolder);

		LPCWSTR filter = L"All\0*.*\0"
						 L"Audio Files (.mp3, .wav, .ogg)\0*.wav;*.mp3;*.ogg\0"
						 L"Lua Files (.lua)\0*.lua\0"
						 L"Model Files (.fbx, .dae, .obj, .mdl)\0*.fbx;*.dae;*.obj;*.mdl\0"
						 L"Prefab Files (.pfb)\0*.pfb\0"
						 L"Save Files (.cfg, .lvl, .ffs)\0*.cfg;*.lvl;*.ffs\0"
						 L"Text Files (.log, .txt)\0*.txt;*.log\0"
						 L"Texture Files (.dds, .png)\0*.dds;.png\0";

		switch (dialogType)
		{
		case FILEDIALOGTYPE::OPEN:
		{
			std::wstring str = L"Browse for file";
			OPENFILENAME ofn;
			TCHAR szFileName[MAX_PATH] = L"";

			SecureZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = (LPWSTR)szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.nFilterIndex = 1;
			ofn.lpstrInitialDir = savefile_directory;
			ofn.lpstrTitle = (TCHAR*)str.data();
			ofn.nMaxFileTitle = 0;
			ofn.lpstrFile[0] = '\0';
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			engine.SetFreezeToggle(true);
			result = GetOpenFileName(&ofn);
			engine.SetFreezeToggle(false);
			if (result)
			{
				char write[MAX_PATH];
				ConvertToCurrentDirectory(szFileName, write);
				destination = write;
			}

			return result;
		}
			break;

		case FILEDIALOGTYPE::SAVEAS:
		{
			std::wstring str = L"Save file as";
			OPENFILENAME ofn;
			TCHAR szFileName[MAX_PATH] = L"";

			SecureZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = (LPWSTR)szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrInitialDir = savefile_directory;
			ofn.lpstrTitle = (TCHAR*)str.data();
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = (LPCWSTR)L".txt";

			engine.SetFreezeToggle(true);
			result = GetSaveFileName(&ofn);
			engine.SetFreezeToggle(false);
			if (result)
			{
				char write[MAX_PATH];
				ConvertToCurrentDirectory(szFileName, write);
				destination = write;
			}

			return result;
		}
			break;
		}
			
		return result;
	}

	void AddToolTip(const std::string& str)
	{
		if (ImGui::IsItemHovered() && str.size())
			ImGui::SetTooltip(str.c_str());
	}

	std::wstring ConvertToWString(const std::string& as)
	{
		std::vector<wchar_t> buf;
		buf.assign(as.size() * 2 + 2, wchar_t{});
		swprintf_s(&buf[0], as.size() * 2 + 2, L"%S", as.c_str());
		std::wstring rval = buf.data();
		return rval;
	}

	std::string ExtractFileName(const std::string & pathname)
	{
		auto dotPos = pathname.rfind(".");
		auto lastBackslash = pathname.rfind("\\");
		auto lastFrontslash = pathname.rfind("/");
		if (lastBackslash == lastFrontslash) //only way for them to be equal is if == end
		{
			auto length = pathname.size() - dotPos + 1;
			return pathname.substr(0, length);
		}
		else if (lastBackslash == std::string::npos)
		{
			auto length = dotPos - lastFrontslash - 1;
			return pathname.substr(lastFrontslash + 1, length);
		}
		else if (lastFrontslash == std::string::npos)
		{
			auto length = dotPos - lastBackslash - 1;
			return pathname.substr(lastBackslash + 1, length);
		}
		else
		{
			if (lastFrontslash > lastBackslash)
			{
				auto length = dotPos - lastFrontslash - 1;
				return pathname.substr(lastFrontslash + 1, length);
			}
			else
			{
				auto length = dotPos - lastBackslash - 1;
				return pathname.substr(lastBackslash + 1, length);
			}
		}

	}

	void CreateErrorMessageBox(const std::string& str)
	{
		std::wstring wide = ConvertToWString(str);

		// Display the default Cursor
		//ShowCursor(WINDOW_S.displayCursor = true);
		MessageBox(COREENGINE_S.GetWindow().GetWindowHandle(), wide.c_str(), NULL, MB_ICONEXCLAMATION);
	}

	void CreateInfoMessageBox(const std::string& str)
	{
		std::wstring wide = ConvertToWString(str);
		std::wstring title = ConvertToWString("Info");

		// Display the default Cursor
		//ShowCursor(WINDOW_S.displayCursor = true);
		MessageBox(COREENGINE_S.GetWindow().GetWindowHandle(), wide.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	std::optional<bool> CreateYesNoMessageBox2(const std::string & str)
	{
		std::wstring wide = ConvertToWString(str);
		std::wstring title = ConvertToWString("Info");

		// Display the default Cursor
		//ShowCursor(WINDOW_S.displayCursor = true);
		auto returnValue = MessageBox(COREENGINE_S.GetWindow().GetWindowHandle(), wide.c_str(), title.c_str(), MB_YESNOCANCEL | MB_ICONWARNING );
		switch (returnValue)
		{
		case IDCANCEL:
			return {};
		case IDYES:
			return true;
		case IDNO:
			return false;
		}
		return {};
	}
}
