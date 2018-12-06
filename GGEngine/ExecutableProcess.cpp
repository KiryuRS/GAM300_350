#include "stdafx.h"
#include <TlHelp32.h>			// CreateToolhelp32Snapshot, Process32First, Process32Next

int ExecutableProcess::RunProcess(const std::vector<std::string> & comm)
{
	// Capture our command and the process name
	command = comm;
	size_t pos = command[0].find(".exe");
	// Not running an exe! Should not be executed at all
	bool result = pos != std::string::npos;
	if (!result)
		return -1;

	size_t trailingSlash = command[0].rfind("/", pos);
	std::string process_name{ command[0].substr(trailingSlash + 1, pos - trailingSlash + strlen(".exe") - 1) };
	processName = process_name;
	std::string flippedCommand;
	std::string commandLine;
	size_t size = command.size();

	if (independentProcess)
	{
		/*
			If its an independent Process, we would want to do the following:
			Process A Creates Process B
			Process B Creates Process C
			Process A Kills Process B
			Process C is still active
		*/

		std::string fullPath = fs::current_path().generic_string();
		fullPath += "/";
		fullPath += "Resources/Templates/ProcCreator.exe ";
		// Flip the commands around
		flippedCommand = fullPath;
		for (int i = (int)size - 1; i >= 0; --i)
		{
			flippedCommand += command[i];
			flippedCommand += (i - 1 >= 0 ? " " : "");
		}
	}
	else
	{
		for (size_t i = 0; i != size; ++i)
		{
			commandLine += command[i];
			commandLine += (i + 1 != size ? " " : "");
		}
	}

	// Create our process
	const std::wstring wpath{ TOOLS::StringToWString(independentProcess ? flippedCommand : commandLine) };
	SecureZeroMemory(&si, sizeof(si));
	SecureZeroMemory(&pi, sizeof(pi));
	GetStartupInfo(&si);
	si.cb = sizeof(si);
	// Start the child process
	result = CreateProcess
	(
		NULL,									// No Module Name (use command line)
		const_cast<wchar_t*>(wpath.c_str()),	// Command Line
		NULL,									// Process Handle not inheritable
		NULL,									// Thread Handle not inheritable
		FALSE,									// Set handle inheritance to FALSE
		0,										// Creation flags
		NULL,									// Use parent's environment block
		NULL,									// Use parent's starting directory
		&si,									// Pointer to STARTUPINFO structure
		&pi										// Pointer to PROCESS_INFORMATION structure
	);

	// Close the handles if its an independent process
	if (independentProcess)
	{
		result = CloseHandle(pi.hProcess);
		result = CloseHandle(pi.hThread);
	}

	return result;
}

bool ExecutableProcess::CloseProcess()
{
	bool success = false;
	// Kill the current process
	IterateAllProcessesAndKill(processName);

	// Close the process and thread handles immediately
	success = CloseHandle(pi.hProcess);
	success = CloseHandle(pi.hThread);

	return success;
}

PROCSTATUS_ ExecutableProcess::Status() const
{
	PROCSTATUS_ status{};
	switch (WaitForSingleObject(pi.hProcess, 0))
	{
	case WAIT_OBJECT_0:
		// process has been terminated
		status = PROCSTATUS_TERMINATED;
		break;

	case WAIT_TIMEOUT:
		// process is still running
		status = PROCSTATUS_RUNNING;
		break;
	}
	
	return status;
}

void ExecutableProcess::IterateAllProcessesAndKill(const std::string& pName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry{ };
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	std::wstring wstring{ pName.begin(), pName.end() };
	while (hRes)
	{
		if (!wstring.compare(pEntry.szExeFile))
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

PWID ProcessesWrapper::PushProcess(const std::vector<std::string> & command, bool independentProcess)
{
	// Push a process into the map and return the PWID
	auto& newProcess = allProcesses[counter++];
	newProcess.independentProcess = independentProcess;
	bool result = newProcess.RunProcess(command);
	if (!result)
		CONSOLE_ERROR("CreateProcess() failed to execute with error code: ", GetLastError());

	// return the ID
	return counter - 1;
}

void ProcessesWrapper::KillProcess(const PWID & id)
{
	// Attempt to find the ID
	auto iter = allProcesses.find(id);
	if (iter == allProcesses.end())
	{
		// No such ID exist in the map!
		CONSOLE_ERROR("No such PWID exist in the list of processes! Please ensure you have captured all of the corresponding PWIDs");
		return;
	}

	// Determine if the process is already dead
	auto& currentProcess = iter->second;
	if (currentProcess.Status() == PROCSTATUS_TERMINATED)
	{
		// Trying to kill a dead process
		// CONSOLE_WARNING("This process has already been dead and should have been removed from the program!");
		// Remove that process from our map
		allProcesses.erase(iter);
		return;
	}

	// Kill the process
	currentProcess.CloseProcess();
}

void ProcessesWrapper::Update()
{
	for (auto&[pwid, process] : allProcesses)
	{
		// Kill the process if has already been terminated
		if (!process.independentProcess && process.Status() == PROCSTATUS_TERMINATED)
			KillProcess(pwid);
	}
}

void ProcessesWrapper::Shutdown()
{
	// Close all of the processes prematurely
	for (auto&[pwid, process] : allProcesses)
	{
		// Kill the process if has already been terminated
		if (!process.independentProcess && process.Status() != PROCSTATUS_TERMINATED)
			KillProcess(pwid);
	}
	allProcesses.clear();
}

size_t ProcessesWrapper::Size() const
{
	return allProcesses.size();
}

bool ProcessesWrapper::Empty() const
{
	return !Size();
}
