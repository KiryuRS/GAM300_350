#include "stdafx.h"
#include "ResourceObject.h"


ResourceObject::ResourceObject(const std::string& str) :
	pathname(str), texture{ nullptr }, sortID{0},
	loaded{ true }, needsLoading{ false }
{
	static auto defaultSound = GFX_S.GetTexture("Resources/Sprites/Editor/speaker.dds");
	static auto defaultOther = GFX_S.GetTexture("Resources/Sprites/Editor/unknown.dds");
	static auto defaultText = GFX_S.GetTexture("Resources/Sprites/Editor/txtfile.dds");
	static auto defaultFolder = GFX_S.GetTexture("Resources/Sprites/Editor/folder.dds");
	static auto defaultLevel = GFX_S.GetTexture("Resources/Sprites/Editor/level.dds");
	static auto defaultPrefab = GFX_S.GetTexture("Resources/Sprites/Editor/prefab.dds");
	static auto defaultScript = GFX_S.GetTexture("Resources/Sprites/Editor/lua_logo.dds");

	auto pos = str.rfind("\\") + 1;
	if(pos != std::string::npos)
		filename = str.substr(pos);
	// generate texture depending on extension
	pos = filename.rfind(".");
	if(pos != std::string::npos)
		extension = filename.substr(pos);

	std::string imagePath;
	if (!extension.size())
	{
		texture = defaultFolder;
	}
	else if (extension == ".png" || extension == ".jpg" || extension == ".dds")
	{
		texture = GFX_S.GetTexture(str);
	}
	else if (extension == ".ogg" || extension == ".wav" || extension == ".mp3")
	{
		texture = defaultSound;
	}
	else if (extension == ".txt" || extension == ".log")
	{
		texture = defaultText;
	}
	else if (extension == SAVEFILETYPE)
	{
		texture = defaultLevel;
	}
	else if (extension == ".pfb")
	{
		texture = defaultPrefab;
	}
	else if (extension == ".lua")
	{
		texture = defaultScript;
	}
	else
	{
		sortID = 1;
		texture = defaultOther;
	}
}

std::string ResourceObject::CreateButton(float size, 
	Editor& editor, int pushID)
{
	ImGui::BeginChild(pushID, { size* 1.5f, size * 1.8f }, false, 16 );
	if (ImGui::ImageButton((void*)((char*)0 + texture->GetID()), { size, size }))
	{
		if (filename.find(".") == std::string::npos)
		{
			ImGui::EndChild();
			return pathname;
		}
		else
		{
			ButtonClick(extension, editor);
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(filename.c_str());
	static constexpr auto textSize = 10;
	std::string newFileName = filename;
	if (newFileName.size() > textSize)
		newFileName = newFileName.substr(0, textSize - 3) + "...";
	ImGui::TextWrapped(newFileName.c_str());
	ImGui::EndChild();
	return "";
}

void ResourceObject::ButtonClick(const std::string & _extension, 
	Editor& editor)
{
	if (_extension == SAVEFILETYPE)
	{
		pathname = TOOLS::ReplaceBSlashWithFSlash(pathname);
		BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::LOADLEVEL, pathname });
	}
	else if (_extension == ".pfb")
	{
		editor.CreateEntity(pathname);
	}
	else if (_extension == SCRIPT_EXTENSION)
	{
		editor.OpenIDE(TOOLS::ReplaceBSlashWithFSlash(pathname));
	}
}
