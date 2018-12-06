 #include "stdafx.h"
#include "BaseMeshLoader.h"


SharedOptions::SharedOptions(Editor& edit) : GuiWindow(edit, windowName),
searchPhrase{ { 0 } }
{
	RecursiveXmlSearch("SaveFile");
	animGui.AddSerializable("Model", mainAnim);
	animGui.AddSerializable("Animation list", animList);
	animGui.AddSerializable("Animation names", animNames, 
		"Names for the animation states in order when serialized.");

}


void SharedOptions::Display(Editor& edit)
{
	// Displaying the frame skips for pause -> next frame
	ImGui::Text("Frame Skip Count:");
	ImGui::SliderInt("Frame Skipping", &edit.frame_skips, 1, 60);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.f);
		ImGui::TextUnformatted("Frame Skips for up to 1 second [1 second = 60 frames]");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::Separator();

	/*if (ImGui::Button("Recompile shaders"))
	{
		MODEL_S.GetView().RecompileShaders();
	}
	ImGui::SameLine(0, -20.f);
	if (ImGui::Button("Reload Textures"))
	{

	}ImGui::TextUnformatted("Search Phrase:");

	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", &searchPhrase[0], 255);
	ImGui::PopID();
	if (ImGui::Button("Search for xml"))
	{
		displayText.clear();
		for (auto& item : xmlPaths)
		{
			std::ifstream inFile;
			inFile.open(item);
			std::string line;
			unsigned lineNum = 0;
			if (inFile.is_open())
			{
				while (std::getline(inFile, line))
				{
					if (line.find(searchPhrase) != std::string::npos)
					{
						displayText.emplace_back();
						displayText.back().first = item;
						displayText.back().second = lineNum;
						break;
					}
					++lineNum;
				}
				inFile.close();
			}
		}
		if (!displayText.size())
			displayText.emplace_back("Could not find any xml file containing the searched phrase.", 0);
	}
	for (auto&item : displayText)
	{
		ImGui::TextWrapped(item.first.c_str());
		std::stringstream _ss;
		_ss << "Line number: " << item.second;
		ImGui::TextWrapped(_ss.str().c_str());
	}
	ImGui::Separator();*/
	ImGui::TextUnformatted("Compile Model:");
	static std::array<char, 255> modelDirectory = { 0 };
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", modelDirectory.data(), 255);
	ImGui::PopID();
	if (ImGui::Button("Compile"))
	{
		BROADCAST_MESSAGE(CompilerMessage{ COMPILERMSG::COMPILEFILE, modelDirectory.data() });
	}
	ImGui::Separator();
	ImGui::TextUnformatted("Shader Options:");
	if (ImGui::Button("Recompile Shaders"))
		BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::RECOMPILESHADERS, nullptr });
	if (ImGui::Button("Print Display Information"))
	{
		std::stringstream ss;
		ss << glGetString(GL_VENDOR) << "\n"
			<< glGetString(GL_RENDERER) << "\n"
			<< glGetString(GL_VERSION) << "\n"
			<< glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
		TOOLS::CreateInfoMessageBox(ss.str());
	}
	ImGui::Separator();

	static float translateAdjust = SCALINGFACTOR;
	ImGui::Text("Scale Translations");
	ImGui::InputFloat("", &translateAdjust);
	if (ImGui::Button("Apply"))
	{
		for (auto& item : edit.entityList->entityList)
		{
			auto xform = item->GetComponent<TransformC>();
			if (xform)
			{
				xform->localPosition *= translateAdjust;
				xform->UpdateSerializables(xform->localPosition);
			}
		}
		BROADCAST_MESSAGE(EditorMessage{ EDITORMSG::SAVEHISTORY, "Applied translation scaling" });
	}
	ImGui::Separator();
	ImGui::Text("Combine animations");
	animGui.GenerateUI();
	ImGui::Text("Save directory");
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::InputText("", directory.data(), 256);
	ImGui::PopID();
	if (ImGui::Button("Save Combined Animation"))
	{
		animGui.UpdateComponentData();
		static MeshCompiler mainSkeleton;
		static std::vector<MeshCompiler> skeletalMeshes;
		skeletalMeshes.clear();
		Serializer serial;
		serial.LoadFile(mainAnim);
		mainSkeleton.ConvertSerializedData(*serial.FirstChild());
		mainSkeleton.animations.clear();
		skeletalMeshes.resize(animList.size());
		for (size_t i = 0; i < animList.size(); ++i)
		{
			Serializer serial2;
			serial2.LoadFile(animList[i]);
			skeletalMeshes[i].ConvertSerializedData(*serial2.FirstChild());
			if (skeletalMeshes[i].animations.size() == 1 && skeletalMeshes[i].animations[0].name == "")
				skeletalMeshes[i].animations[0].name = animNames[i];
			mainSkeleton.animations.insert(mainSkeleton.animations.end(),
				skeletalMeshes[i].animations.begin(), skeletalMeshes[i].animations.end());
		}
		mainSkeleton.UpdateSerializables();
		mainSkeleton.UpdateSerialMeshInfo();
		mainSkeleton.UpdateSerialAnimInfo();
		Serializer outputFile;
		auto& node = outputFile.NewElement("ModelFile");
		mainSkeleton.Serialize(node);
		outputFile.SaveFile(directory.data());
		GraphicsMessage msg;
		msg.type = GFXMSG::COMPILEDFILE;
		std::get<ModelDetails>(msg.data).model = directory.data();
		BROADCAST_MESSAGE(msg);
	}
	ImGui::Separator();
	if (GFX_S.smeshLoader->loadingMesh)
	{
		std::stringstream ss;
		ss << "Loading Static Mesh: " << GFX_S.smeshLoader->loadingMesh.value();
		ImGui::TextWrapped(ss.str().c_str());
	}
	if (GFX_S.skmeshLoader->loadingMesh)
	{
		std::stringstream ss;
		ss << "Loading Skeletal Mesh: " << GFX_S.skmeshLoader->loadingMesh.value();
		ImGui::TextWrapped(ss.str().c_str());
	}
	if (GFX_S.texLoader->loadingMesh)
	{
		std::stringstream ss;
		ss << "Loading Texture: " << GFX_S.texLoader->loadingMesh.value();
		ImGui::TextWrapped(ss.str().c_str());
	}
}

void SharedOptions::RecursiveXmlSearch(const std::string & folderName)
{
	for (auto & p : std::experimental::filesystem::directory_iterator(folderName))
	{
		auto str = p.path().string();
		auto extension = TOOLS::GetExtension(str);
		if (extension == SAVEFILETYPE)
			xmlPaths.emplace_back(p.path().string());
		else if (!extension.size())
			RecursiveXmlSearch(str);
	}
}