/*****************************************************************************/
/*!
\file StaticMeshC.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

This is a static mesh container, it holds the mesh that current object is using
then pass it to the renderer to render.

serialize mesh data

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#include "stdafx.h"
#include "StaticMeshC.h"

StaticMeshC::StaticMeshC() : base(*this)
{
	
}

void StaticMeshC::AddSerializeData(LuaScript * luaScript)
{
	base.AddSerializeData(luaScript);	
	AddCallableFunction("SetStaticMesh", &StaticMeshC::SetMesh, luaScript);
	AddCallableFunction("GetStaticMesh", &StaticMeshC::GetMeshname, luaScript);
	AddCallableFunction("SetStaticMaterial", &StaticMeshC::SetStaticMaterial, luaScript);
}

bool StaticMeshC::EditorGUI(Editor&)
{
	bool modelMod = base.ChooseModelGUI();
	bool materialMod = base.ChooseMaterialGUI();
	return modelMod || materialMod;
}

void StaticMeshC::EditorSetup()
{
	base.Setup();
}

void StaticMeshC::EditorUpdate(float)
{
	base.UpdateTransformMinMax();
	base.Update();
}

void StaticMeshC::SetMesh(std::string meshName)
{
	base.animNo = 0;
	base.modelDirectory = meshName;
	base.mesh_ptr = GFX_S.GetStaticMesh(meshName);
	base.animTime = 0.f;
	base.UpdateTransformMinMax();
}

void StaticMeshC::SetStaticMaterial(std::string materialName)
{
	for (auto& item : base.materials)
		item = materialName;
}

std::string StaticMeshC::GetMeshname()
{
	return base.modelDirectory;
}
