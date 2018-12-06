#include "stdafx.h"
#include "SystemLog.h"

std::string SystemLog::ConsoleMessageConvertor(DebugTypes _pt)
{
	std::string retval;
	switch (_pt)
	{
	case DebugTypes::DT_DEFAULT:
		retval = ":: ";
		break;

	case DebugTypes::DT_WARNING:
		retval = "WARNING:: ";
		break;

	case DebugTypes::DT_ERROR:
		retval = "ERROR:: ";
		break;

	case DebugTypes::DT_ASSERTION:
		retval = "ASSERTION ERROR:: ";
		break;

	case DebugTypes::DT_SYSTEM:
		retval = "SYSTEM:: ";
		break;
	}

	return retval;
}

void SystemLog::WriteToFile(bool end)
{
	// Grab the date
	__time64_t rawTime;
	std::tm timeInfo;
	char buffer[80];

	time(&rawTime);
	_localtime64_s(&timeInfo, &rawTime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeInfo);
	std::string buf{ buffer };

	// Append some information to our string_log
	string_log.insert(string_log.begin(), { "DATE: " + buf + "\n", "=============== START LOGGING ===============\n" });
	// Determines if the logger should be printing until the end
	if (end)
		string_log.emplace_back("=============== END LOGGING ===============\n");

	// Call up our Save Data function
	TOOLS::SaveData(directory, filename, string_log);
	// Check if its not till the end, we destroy the whole of the string_log
	if (!end)
		string_log.clear();
}

bool SystemLog::FileFiltering()
{
	// Easier scoping
#if FULLC17
	namespace fs = std::filesystem;
#else
	namespace fs = std::experimental::filesystem;
#endif

	// Get the current path of the directory
	fs::path dir = fs::current_path();
	// Append to the full path for deletion
	std::string full_path{ dir.generic_string() + directory + "/" + filename };

	return fs::remove(full_path);
}

SystemLog::SystemLog()
	: directory{ LOGGING_DIRECTORY },
	  filename{ "logfile.txt" }, scriptActive{ false }
{ }

SystemLog::~SystemLog()
{
	// Removes the current file logging before we write to a new one
	FileFiltering();

	// Write our logging to the output file
	WriteToFile();
}

