#include "stdafx.h"
#include "ResourceWindow.h"
#include "ResourceObject.h"
// Disabling the integral to void* upsize
#pragma warning( push )
#pragma warning( disable : 4312 )

void ResourceWindow::GrabFolderInformation()
{
	CoreEngine& engine = COREENGINE_S;
	std::promise<bool> prom;
	std::future<bool> future{ prom.get_future() };
	// Wait for the filesCheckerInstance
	FilesChecker* fChecker = engine.resourceLoader->GetFCInstance();
	while (!(fChecker = engine.resourceLoader->GetFCInstance()));
	std::vector<FolderInfo> folders{ std::move(fChecker->GrabAllFolders(prom)) };
	future.get();
	allFolders.insert(allFolders.end(), folders.begin(), folders.end());
	afIter = allFolders.begin();
}

bool ResourceWindow::CheckIfExcluded(const std::string & path) const
{
	for (const auto& elem : excluded_location)
	{
		const std::string& modifiedElem = TOOLS::ReplaceFSlashWithBSlash(elem);
		// Check if any of the excluded path lies within path
		size_t pos = path.find(modifiedElem.substr(0, modifiedElem.length() - 1));
		if (pos != std::string::npos)
			return true;
	}

	// If it reaches here that means the path should NOT be excluded
	return false;
}

void ResourceWindow::Initialize(Editor &)
{
	// Add the first element as Resources
	allFolders.insert(allFolders.begin(), FolderInfo{ "Resources" });
	afIter = allFolders.begin();

	CoreEngine& engine = COREENGINE_S;
	// Schedule a thread to retrieve the folder details
	engine.scheduler->QueueThread([&, this]()->void { this->GrabFolderInformation(); });
}

void ResourceWindow::Display(Editor& edit)
{
	static constexpr float miniIconSize = 13.f;
	// Do a filter
	static ImGuiTextFilter filter;

	// Refresh
	static Texture* refreshTexture = GFX_S.GetTexture("Resources/Sprites/Editor/refresh.dds");
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::ImageButton((ImTextureID)refreshTexture->GetID(), { miniIconSize, miniIconSize }))
		LoadResources(fs::directory_entry{ fs::path(currDirectory) });
	ImGui::PopID();
	TCHAR savefile_directory[MAXBUFFER];
	GetCurrentDirectory(MAXBUFFER, savefile_directory);

	ImGui::SameLine();
	// Up arrow
	static Texture *upArrow = GFX_S.GetTexture("Resources/Sprites/Editor/uparrow.dds");
	TOOLS::PushID(UITYPE::DEFAULT);
	if (ImGui::ImageButton((ImTextureID)upArrow->GetID(), { miniIconSize, miniIconSize }))
	{
		filter.Clear();
		auto pos = currDirectory.rfind("\\");
		if (pos != std::string::npos)
		{
			currDirectory = currDirectory.substr(0, pos);
			currChanged = true;
			LoadResources(fs::directory_entry{ fs::path(currDirectory) });
		}
	}
	ImGui::PopID();
	ImGui::SameLine();
	// Folder name
	ImGui::TextUnformatted(currDirectory.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.65f);
	ImGui::Text("Search:");
	ImGui::SameLine();
	filter.Draw("", ImGui::GetWindowContentRegionWidth() * 0.25f);
	ImGui::SameLine();
	if (ImGui::Button("Clear"))
		filter.Clear();
	ImGui::Separator();
	//crashing!!! gonna hide for now
	// Inner Child (List Hierarchy of the files)
	//{
	//	// Determine which folder is selected
	//	if (currChanged)
	//	{
	//		afIter->selected = false;
	//		afIter = std::find(allFolders.begin(), allFolders.end(), currDirectory);
	//		//@kenneth dereference error was here. Added if check for end
	//		currChanged = false;
	//		if (afIter != allFolders.end())
	//		{
	//			afIter->selected = true;
	//		}
	//	}
	//	ImGui::BeginChild("FolderList", ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.15f, 0 }, false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_HorizontalScrollbar);
	//	
	//	// Increase spacing to differentiate leaves from expanded contents.
	//	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	//	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);
	//	// Draw the first tree node
	//	ImGui::PopStyleVar();
	//	ImGui::EndChild();
	//}

	// ImGui::SameLine();

	// Inner Child (Creating all the icons within the window)
	{
		unsigned pushID = 200;
		static constexpr float buttonSize = 50;
		ImGui::BeginChild("FileDisplay", ImVec2{ 0,0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
		unsigned lineTotal = static_cast<unsigned>(ImGui::GetWindowContentRegionWidth() / buttonSize / 1.65f);
		unsigned lineCount = lineTotal;
		for (auto& file : ResourceList)
		{
			if (!filter.PassFilter(file.filename.c_str()))
				continue;

			++lineCount;
			if (lineCount >= lineTotal)
				lineCount = 0;
			else
				ImGui::SameLine();
			std::string newDirectory = file.CreateButton(buttonSize, edit, pushID++);
			if (newDirectory.size())
			{
				currDirectory = newDirectory;
				currChanged = true;
				LoadResources(fs::directory_entry{ fs::path(newDirectory) });
				break;
			}
		}
		ImGui::EndChild();
	}
}

void ResourceWindow::LoadResources(fs::directory_entry Path)
{
	currentItem = nullptr;
	ResourceList.clear();
	for (auto& p : fs::directory_iterator(Path))
	{
		const std::string& str = p.path().string();
		if (CheckIfExcluded(str))
			continue;
		ResourceList.emplace_back(str);
	}

	std::sort(ResourceList.begin(), ResourceList.end(),
		[](const ResourceObject& obj1, const ResourceObject& obj2)
	{
		return obj1.sortID < obj2.sortID;
	});
}

void ResourceWindow::Update(Editor &)
{
	//check for any file modifications

	//window refreshes when modifications are made
}

#pragma warning( pop )
