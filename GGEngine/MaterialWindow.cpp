#include "stdafx.h"
#include "MaterialWindow.h"

MaterialWindow::MaterialWindow(Editor& edit):
	GuiWindow(edit, windowName), materialName{ 0 }
{
	std::vector<std::string> materialLocations;
	std::string path = MATERIAL_DIRECTORY;
	for (const auto &p : fs::directory_iterator(path))
		materialLocations.emplace_back(TOOLS::ReplaceBSlashWithFSlash(p.path().string()));
	for (auto& item : materialLocations)
	{
		Serializer ss;
		ss.LoadFile(item);
		auto root = ss.FirstChild();
		if (root == nullptr)
		{
			continue;
		}
		auto fileName = TOOLS::ExtractFileName(item);
		materials[fileName].ConvertSerializedData(*root->FirstChildElement());
	}
}

void MaterialWindow::Display(Editor&)
{
	ImGui::Text("Materials");
	ImGui::InputText("", materialName.data(), numStr);
	if (ImGui::Button("Create New Material"))
	{
		CreateMaterial();
	}
	ImGui::Separator();
	for (auto& item : materials)
	{
		if (ImGui::CollapsingHeader(item.first.c_str()))
		{
			TOOLS::PushID(UITYPE::DEFAULT);
			if (ImGui::Button("Save"))
			{
				SaveMaterial(item.first);
			}
			ImGui::PopID();
			ImGui::SameLine();
			TOOLS::PushID(UITYPE::DEFAULT);
			if (ImGui::Button("Delete"))
			{
				DeleteMaterial(item.first);
				return;
			}
			ImGui::PopID();
			item.second.GenerateUI();
		}
	}
}

void MaterialWindow::CreateMaterial()
{
	if (!materialName[0])
	{
		CONSOLE_ERROR("Cannot have empty prefab name!");
		return;
	}
	materials[materialName.data()] = Material();
	Serializer Doc;
	SerializeElem &pRoot = Doc.NewElement("Root");
	SerializeElem & pElement = pRoot.InsertChild("Material");
	pElement.SetAttribute("Name", &materialName[0]);
	materials[materialName.data()].Serialize(pElement);
	std::string saveString = std::string(MATERIAL_DIRECTORY);
	saveString += &materialName[0];
	saveString += ".pfb";
	Doc.SaveFile(saveString.c_str());
	TOOLS::CreateInfoMessageBox("Material saved!");
	BROADCAST_MESSAGE(GraphicsMessage{GFXMSG::UPDATEMATERIALLIST, nullptr});
}

void MaterialWindow::SaveMaterial(const std::string & matName)
{
	Serializer Doc;
	SerializeElem &pRoot = Doc.NewElement("Root");
	SerializeElem & pElement = pRoot.InsertChild("Material");
	pElement.SetAttribute("Name", matName.c_str());
	materials[matName].Serialize(pElement);
	std::string saveString = std::string(MATERIAL_DIRECTORY);
	saveString += matName;
	saveString += ".pfb";
	Doc.SaveFile(saveString.c_str());
	TOOLS::CreateInfoMessageBox("Material saved!");
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATEMATERIALLIST, nullptr });

}

void MaterialWindow::DeleteMaterial(const std::string & str)
{
	std::string path = MATERIAL_DIRECTORY + str + ".pfb";
	fs::remove(path);
	auto iter = materials.find(str);
	materials.erase(iter);
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::UPDATEMATERIALLIST, nullptr });
}

void MaterialWindow::Update(Editor &)
{
}
