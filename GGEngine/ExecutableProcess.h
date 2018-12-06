#pragma once
#include <string>
#include <Windows.h>
#include <vector>

enum PROCSTATUS_
{
	PROCSTATUS_RUNNING,
	PROCSTATUS_TERMINATED,
};

// Executable Process code for opening an exe
// Possible to expand this segment further with Windows API but this will suffice for now
struct ExecutableProcess final
{
	STARTUPINFO					si;
	PROCESS_INFORMATION			pi;
	std::vector<std::string>	command;
	std::string					processName;
	bool						independentProcess = false;

	int RunProcess(const std::vector<std::string>& command);
	bool CloseProcess();
	PROCSTATUS_ Status() const;
	
private:
	void IterateAllProcessesAndKill(const std::string& processName);
	LPSTARTUPINFO			lsi;
	LPPROCESS_INFORMATION	lpi;
};

using PWID = __int32;
// Wrapper function for the ExecutableProcess
class ProcessesWrapper
{
	std::map<PWID, ExecutableProcess> allProcesses;
	PWID counter = 0;

public:

	ProcessesWrapper() = default;
	/*
		Similar to Linux version of "exec" as done in CS 180
		E.G.
			notepad.exe README.txt
			will be
			command[0] = "notepad.exe"
			command[1] = "README.txt"
	*/
	PWID PushProcess(const std::vector<std::string>& command, bool independentProcess = false);
	void KillProcess(const PWID& id);
	void Update();
	void Shutdown();
	size_t Size() const;
	bool Empty() const;
};