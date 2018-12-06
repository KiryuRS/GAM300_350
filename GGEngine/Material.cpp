/*****************************************************************************/
/*!
\file CEMaterial.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Defines material of a game object

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include "Material.h"

#pragma warning (disable : 4244)
#pragma warning (disable : 4245)

Material::Material()
	:
	m_specular_power(1.0f),
	m_normal_power(1.0f),
	m_illumination_power(1.0f),
	m_tint(Vector4(0.0f, 0.0f, 0.0f, 0.0f)),
	HasDiffuse(true),
	HasSpecular(false),
	HasNormal(false),
	HasOpacity(false),
	HasSelfIllumination(false)
{
	AddSerializable("HasDiffuse", HasDiffuse);
	AddSerializable("HasSpecular", HasSpecular);
	AddSerializable("HasNormal", HasNormal);
	AddSerializable("HasOpacity", HasOpacity);
	AddSerializable("HasSelfIllumination", HasSelfIllumination);
	AddSerializable("diffuse_name", diffuse_name).display = false;
	AddSerializable("specular_name", specular_name).display = false;
	AddSerializable("normal_name", normal_name).display = false;
	AddSerializable("opacity_name", opacity_name).display = false;
	AddSerializable("self_illumination_name", self_illumination_name).display = false;
  AddSerializable("Emissive_Color", m_emissive_clr);
	AddSerializable("m_specular_power", m_specular_power);
	AddSerializable("m_normal_power", m_normal_power);
	AddSerializable("m_illumination_power", m_illumination_power);
	//AddSerializable("Tint", m_tint);
	UpdateSerializables();
}

void Material::Reset()
{
}

void Material::SetupTextures()
{
	if (diffuse_name.size())
		m_diffuse = GFX_S.GetTexture(diffuse_name);
	else
		m_diffuse = GFX_S.GetTexture("CompiledAssets/Sprites/sample.dds");
	if (specular_name.size())
		m_specular = GFX_S.GetTexture(specular_name);
	else
		m_specular = GFX_S.GetTexture("CompiledAssets/Sprites/sample.dds");

	if (normal_name.size())
		m_normal = GFX_S.GetTexture(normal_name);
	else
		m_normal = GFX_S.GetTexture("CompiledAssets/Sprites/sample.dds");

	if (opacity_name.size())
		m_opacity_mask = GFX_S.GetTexture(opacity_name);
	else
		m_opacity_mask = GFX_S.GetTexture("CompiledAssets/Sprites/sample.dds");

	if (self_illumination_name.size())
		m_self_illumination = GFX_S.GetTexture(self_illumination_name);
	else
		m_self_illumination = GFX_S.GetTexture("CompiledAssets/Sprites/sample.dds");
}

bool Material::ChooseTextureGUI(const std::string& name, std::string & str, unsigned arrPos)
{
	auto& tList = GFX_S.GetTextureList();
	int prevNum = textureNums[arrPos];
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	TOOLS::PushID(UITYPE::DEFAULT);
	ImGui::Combo("", &textureNums[arrPos], tList.data(), int(tList.size()), int(tList.size()));
	ImGui::PopID();
	if (prevNum != textureNums[arrPos])
	{
		str = tList[textureNums[arrPos]];
		UpdateSerializables(str);
		return true;
	}
	return false;
}

std::optional<int> Material::GenerateUI()
{
	UpdateTextureNum(diffuse_name, 0);
	UpdateTextureNum(specular_name, 1);
	UpdateTextureNum(normal_name, 2);
	UpdateTextureNum(opacity_name, 3);
	UpdateTextureNum(self_illumination_name, 4);
	ChooseTextureGUI("diffuse_name", diffuse_name, 0);
	ChooseTextureGUI("specular_name", specular_name, 1);
	ChooseTextureGUI("normal_name", normal_name, 2);
	ChooseTextureGUI("opacity_name", opacity_name, 3);
	ChooseTextureGUI("self_illumination_name", self_illumination_name, 4);
	Serializable::GenerateUI();
	return std::optional<int>();
}

void Material::UpdateTextureNum(std::string & textureDirectory, unsigned arrPos)
{
	auto& tList = GFX_S.GetTextureList();
	if (!textureDirectory.size())
		textureDirectory = tList[0];
	for (unsigned i = 0; i < tList.size(); ++i)
	{
		std::string ll = tList[i];
		if (ll == textureDirectory)
		{
			textureNums[arrPos] = i;
			return;
		}
	}
	CONSOLE_ERROR("Could not find texture ", textureDirectory, "!");
}
