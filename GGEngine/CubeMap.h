/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: cube_map.h

Purpose: interface of the cube map
Language: c++, Visual Studio 2017
Platform: Windows, SDK version 10.0.17134.0, Platform toolset Visual Studio 2017

(v141)
Project: GAM350
Author: Zhihua Zhou, zhihua.z, 270000013
Creation date: July 7, 2018
End Header --------------------------------------------------------*/
#pragma once

#include "glm.hpp"
#include "glew.h"

struct CubeMap
{
  CubeMap();
  ~CubeMap();

  GLuint fbo[6];
  GLuint depthrenderbuffer[6];
  GLuint cm_render_textures[6];

};