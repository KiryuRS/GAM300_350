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
#include "DirectionalLightC.h"

DirectionalLightC::DirectionalLightC()
{
}

void DirectionalLightC::AddSerializeData(LuaScript * luaScript)
{
  // By calling AddSerializable on a variable,
  // you get the property being displayed in ImGui
  AddSerializable("Light_Diffuse", mLight.diffuse, "", luaScript);
  AddSerializable("Light_Specular", mLight.specular, "", luaScript);
  AddSerializable("Light_Ambient", mLight.ambient, "", luaScript);
  AddSerializable("Light_Direction", mLight.dir, "", luaScript);
  AddSerializable("Intensity", mLight.intensity, "", luaScript);
}

void DirectionalLightC::Initialize()
{
	EditorSetup();
}

void DirectionalLightC::EditorSetup()
{
  GFX_S.RegisterLight(&mLight, GetGameSpaceID());
  mLight.light_type = DIRECTIONAL_LIGHT;
}

void DirectionalLightC::EditorUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

void DirectionalLightC::DestroyUpdate()
{
	GFX_S.UnregisterLight(&mLight, GetGameSpaceID());
}
