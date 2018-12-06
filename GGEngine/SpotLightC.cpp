/*****************************************************************************/
/*!
\file DirectionalLightC.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 24, 2018
\brief

This is a container for directional light data

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#include "stdafx.h"
#include "SpotLightC.h"

#define TO_PI (3.141592653f / 180.0f)

SpotLightC::SpotLightC()
{
  RequiredComponents(transform);
}

void SpotLightC::AddSerializeData(LuaScript * luaScript)
{
  // By calling AddSerializable on a variable,
  // you get the property being displayed in ImGui
  AddSerializable("Light_Diffuse", mLight.diffuse, "", luaScript);
  AddSerializable("Light_Specular", mLight.specular, "", luaScript);
  AddSerializable("Light_Ambient", mLight.ambient, "", luaScript);
  AddSerializable("Light_Direction", mLight.dir, "", luaScript);
  AddSerializable("Inner_Angle", innerAngle, "", luaScript);
  AddSerializable("Outer_Angle", outerAngle, "", luaScript);
  AddSerializable("Falloff", mLight.falloff, "", luaScript);
}

void SpotLightC::EditorSetup()
{
  GFX_S.RegisterLight(&mLight, GetGameSpaceID());
  mLight.light_type = SPOT_LIGHT;
}

void SpotLightC::Initialize()
{
	EditorSetup();
}

void SpotLightC::EditorUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);
  mLight.pos = transform->GetGlobalPosition();
  mLight.inner_cos = cosf(innerAngle * TO_PI);
  mLight.outer_cos = cosf(outerAngle * TO_PI);
}

void SpotLightC::DestroyUpdate()
{
	GFX_S.UnregisterLight(&mLight, GetGameSpaceID());
}
