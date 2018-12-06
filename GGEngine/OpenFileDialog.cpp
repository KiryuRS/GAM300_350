#include "stdafx.h"

OpenFileDialog::OpenFileDialog(void)
	: FileName{}, Filter{ nullptr }, FilterIndex{ 1 }, InitialDir{ nullptr },
	  Title{ nullptr }, flags{ 0 }
{ }

bool OpenFileDialog::ShowOpenDialog()
{
	flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Clears the buffer
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = FileName;
	ofn.hwndOwner = NULL;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = FilterIndex;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = InitialDir;
	ofn.Flags = flags;
	ofn.lpstrTitle = Title;

	CoreEngine& engine = COREENGINE_S;
	// Inform the engine that we are "freezing the window"
	engine.SetFreezeToggle(true);
	// GetOpenFileName returns a boolean to determine if the open file dialog is successful
	bool check = GetOpenFileName(&ofn);
	// Once done we will then inform the engine to resume normal operation
	engine.SetFreezeToggle(false);

	return check;
}

bool OpenFileDialog::ShowSaveDialog()
{
	flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Clears the buffer
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = FileName;
	ofn.hwndOwner = NULL;

	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = FilterIndex;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = InitialDir;
	ofn.Flags = flags;
	ofn.lpstrTitle = Title;
	ofn.lpstrDefExt = (LPCWSTR)L"ffs";

	CoreEngine& engine = COREENGINE_S;
	// Inform the engine that we are "freezing the window"
	engine.SetFreezeToggle(true);
	// GetSaveFileName returns a boolean to determine if the open file dialog is successful
	bool check = GetSaveFileName(&ofn);
	// Once done we will then inform the engine to resume normal operation
	engine.SetFreezeToggle(false);

	return check;
}

std::string OpenFileDialog::ToString()
{
	char mbbufer[MAX_PATH];
	size_t length;

	// WCHAR Conversion to char
	wcstombs_s(&length, mbbufer, MAX_PATH, ofn.lpstrFile, MAX_PATH);

	return { mbbufer };
}
