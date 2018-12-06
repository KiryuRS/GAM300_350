#pragma once
#include <vector>
#include "EnumParser.h"

// Class for Logging the file (Could be potentially error logging or debug logging)
class SystemLog
{
	std::vector<std::string> string_log;
	const std::string directory;
	const std::string filename;
	std::string ConsoleMessageConvertor(DebugTypes _pt);

	bool scriptActive;

public:
	void WriteToFile(bool end = true);
	bool FileFiltering();
	const auto& GetLog() const { return string_log; }

	SystemLog();
	~SystemLog();

	/*
		Parameters:
		- newline	: to determine if the printout should append a new line
		- str		: the first argument to pass it in
		- rest		: rest of the arguments to append
		*Note that str and rest must support operator<<
	*/
	template <typename First, typename ... Args>
	void PrintLog(bool newline, DebugTypes _pt, First&& str, Args&& ... rest)
	{
		std::string debug_printout = ConsoleMessageConvertor(_pt);

		std::stringstream ss;
		// Check for end of script and print accordingly
		if (scriptActive)
		{
			const std::string& scriptStr = "END SCRIPT\n";
			std::cout << scriptStr;
			string_log.emplace_back(scriptStr);
			scriptActive = false;
		}
		// Retrieve the string type based on the printtype
		ss << std::boolalpha << debug_printout << str;

		(ss << ... << rest);

		// Get our final string format
		const std::string& final_str = ss.str();
		const std::string& newLineStr = newline ? "\n" : "";

		// Grab the date
		__time64_t rawTime;
		std::tm timeInfo;
		static constexpr size_t bufSize = 90;
		char buffer[bufSize]{ };

		time(&rawTime);
		_localtime64_s(&timeInfo, &rawTime);

		strftime(buffer, bufSize, "{ %d-%m-%Y %I:%M:%S }  ", &timeInfo);
		const std::string buf{ buffer };

		// Print it out to the console
		std::cout << buf + final_str << newLineStr;

		// Append to our list
		string_log.emplace_back(buf + final_str + newLineStr);
	}

	template <typename First, typename ... Args>
	void PrintScriptLog(First&& str, Args&& ... rest)
	{
		std::stringstream ss;
		/*
			For script printing, we would want to ultimately print in the following format:
			BEGIN SCRIPT
			// script printouts
			END SCRIPT

			BEGIN SCRIPT marks the start of the script when it first detects the script printing
			END script marks the end of script and is defined when other _pt types interfers
		*/
		if (!scriptActive)
			ss << "BEGIN SCRIPT\n";
		ss << str;
		(ss << ... << rest);
		const std::string& scriptStr = ss.str();
		// Do not have to print out the date
		std::cout << scriptStr << std::endl;
		string_log.emplace_back(scriptStr);
		scriptActive = true;
	}
};
