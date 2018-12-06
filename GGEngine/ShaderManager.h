/*****************************************************************************/
/*!
\file CEShaderManager.h
\author Zhou Zhihua 
\par zhihua.z\@digipen.edu
\date August 31, 2017
\brief

Provides management of shaders

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include <vector>
#include <map>
#include <string>
#include <glew.h>//from http://glew.sourceforge.net/
#include <glm.hpp>
#include "GraphicsEnums.h"
#include "Shader.h"
#pragma once


class ShaderManager
{
public:

  ShaderManager();

  bool Init();
  void Clean();
  void Recompile();

  bool CreateShader(CurrentShader cs);

  Shader& Get(CurrentShader cs);

private:
	std::map<CurrentShader, Shader> m_shaders;
	std::string shaderpath;
};