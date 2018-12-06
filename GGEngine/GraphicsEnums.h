/*****************************************************************************/
/*!
\file GraphicsEnum.h
\author Zhou Zhihua, zhihua.z, 390001016
\par zhihua.z\@digipen.edu
\date August 12, 2017
\brief

Shared enums and constants of engine

Copyright (C) 2017 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once


#define FLOATS_PER_VERTEX     8
#define FLOATS_PER_POSITION   3
#define FLOATS_PER_NORMAL     3
#define FLOATS_PER_UV         2

#define CE_R_MASK             0xFF000000
#define CE_G_MASK             0x00FF0000
#define CE_B_MASK             0x0000FF00
#define CE_A_MASK             0x000000FF
#define CE_POWER_FOUR         0xFFFFFFFF
#define CE_POWER_THREE        0x00FFFFFF
#define CE_POWER_TWO          0x0000FFFF
#define CE_POWER_ONE          0x000000FF

enum PrimitiveType
{
	CE_LINE = 0,
	CE_TRIANGLE
};

enum DrawMode
{
	DM_TRIANGLE = 0,
	DM_WIREFRAME
};

enum CurrentShader
{
	S_PLAIN = 0,
	S_CUSTOM,
	S_TEXTURE,
	S_IMGUI,
	S_BOUNDING_BOX,
	S_PARTICLE,
	S_PARTICLE_INDEPENDENT,
	S_FONT,
	S_LINE,
  S_DEPTH,
  S_SKYBOX,
  S_BLUR,
  S_BLEND,
  S_EXTRACT,
  S_HDR,
  S_GAMMA,
  S_SPRITE2D
};

enum PARTICLE_BLENDING_MODE
{
	PARTICLE_B_NONE = 0,
	PARTICLE_B_ADDITIVE
};

enum PrimitiveType;
enum DrawMode;
