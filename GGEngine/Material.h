/*****************************************************************************/
/*!
\file CEMaterial.h
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
#pragma once
#include "GraphicsEnums.h"
#include <string.h>
#include "Vector.h"
#include "Serializable.h"

#define UNINITIALIZED_FLAG 0xFFFFFFFF
class Texture;

struct Material: public Serializable
{
	Material();
	void Reset();

	bool        HasDiffuse = { true };
	bool        HasSpecular = { false };
	bool        HasNormal = { false };
	bool        HasOpacity = { false };
	bool        HasSelfIllumination = { false };

  std::string diffuse_name;
  std::string specular_name;
  std::string normal_name;
  std::string opacity_name;
  std::string self_illumination_name;
  std::array<int, 5> textureNums;

  Texture* m_diffuse, *m_specular, *m_normal, *m_opacity_mask, *m_self_illumination;
	//GLuint      m_diffuse;
	//GLuint      m_specular;
	float       m_specular_power; // how strong is the highlight
	//GLuint      m_normal;
	float       m_normal_power; // how strong is the shadow
	//GLuint      m_opacity_mask; // which part is transparent
	//GLuint      m_self_illumination;
	float       m_illumination_power;
	Vector4     m_tint;
  Vector4     m_emissive_clr = { Vector4(.0f, .0f, .0f, 1.0f) };
	void SetupTextures();
	bool ChooseTextureGUI(const std::string&, std::string& str, unsigned arrPos);
	std::optional<int> GenerateUI() override;
	void UpdateTextureNum(std::string& textureDirectory, unsigned arrPos);
};