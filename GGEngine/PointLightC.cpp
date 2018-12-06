/*****************************************************************************/
/*!
\file PointLightC.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

This is a container for point light data

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#include "stdafx.h"
#include "PointLightC.h"

PointLightC::PointLightC()
{
  RequiredComponents(transform);
}
void PointLightC::AddSerializeData(LuaScript * luaScript)
{
  // By calling AddSerializable on a variable,
  // you get the property being displayed in ImGui
  AddSerializable("Light_Diffuse", mLight.diffuse, "", luaScript);
  AddSerializable("Light_Specular", mLight.specular, "", luaScript);
  AddSerializable("Light_Ambient", mLight.ambient, "", luaScript);
  AddSerializable("Light_Intensity", mLight.intensity, "", luaScript);
  AddSerializable("Light_Radius", mLight.radius, "", luaScript);
}

void PointLightC::Initialize()
{
	EditorSetup();
}

void PointLightC::EditorSetup()
{
  GFX_S.RegisterLight(&mLight, GetGameSpaceID());
  mLight.light_type = POINT_LIGHT;
}

void PointLightC::EditorUpdate(float dt)
{
	UNREFERENCED_PARAMETER(dt);
  mLight.pos = transform->GetGlobalPosition();
}

void PointLightC::DestroyUpdate()
{
	GFX_S.UnregisterLight(&mLight, GetGameSpaceID());
}
