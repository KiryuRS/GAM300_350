#pragma once

#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>

class OpenFileDialog
{
	OPENFILENAME ofn;
	int flags;

public:
	OpenFileDialog();

	LPCWSTR Filter;
	int FilterIndex;
	TCHAR* InitialDir;
	TCHAR* Title;
	TCHAR FileName[MAX_PATH];

	bool ShowOpenDialog();
	bool ShowSaveDialog();
	// Converts the TCHAR array to std::string
	std::string ToString();
};