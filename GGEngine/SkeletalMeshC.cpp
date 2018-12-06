/*****************************************************************************/
/*!
\file SkeletalMeshC.cpp
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

This is a skeletal mesh container, it holds the mesh that current object is using
then pass it to the renderer to render.

serialize mesh data

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#include "stdafx.h"
#include "SkeletalMeshC.h"
#include "SkeletalMeshRendererC.h"

SkeletalMeshC::SkeletalMeshC(): base(*this)
{
	OptionalComponents(renderer);
}

void SkeletalMeshC::AddSerializeData(LuaScript * luaScript)
{
	base.AddSerializeData(luaScript);
	base.AddAnimSerializeData(luaScript);
	AddCallableFunction("SetMesh", &SkeletalMeshC::SetMesh, luaScript);
	AddCallableFunction("SetAllMaterials", &SkeletalMeshC::SetMaterial, luaScript);
  AddCallableFunction("SetMaterial", &SkeletalMeshC::SetMaterialIndexed, luaScript);
}

bool SkeletalMeshC::EditorGUI(Editor&)
{
	bool modelMod = base.ChooseModelGUI();
	bool animMod = base.ChooseAnimationGUI();
	if (animMod || modelMod) UpdateSerializables(base.animNo);
	bool materialMod = base.ChooseMaterialGUI();
	return modelMod || animMod || materialMod;
}

void SkeletalMeshC::EditorSetup()
{
	base.Setup();
}

void SkeletalMeshC::EditorUpdate(float)
{
  base.Update();
}

void SkeletalMeshC::SetMesh(std::string meshName)
{
	base.animNo = 0;
	base.modelDirectory = meshName;
	base.mesh_ptr = GFX_S.GetSkeletalMesh(meshName);
	base.animTime = 0.f;
	base.UpdateTransformMinMax();
	base.ResetMaterials();
	renderer->PlayAnimation();
}


void SkeletalMeshC::SetMaterial(std::string materialName)
{
	auto bsMatName = TOOLS::ReplaceFSlashWithBSlash(materialName);
	for (auto& item : base.materials)
		item = bsMatName;
}

void SkeletalMeshC::SetMaterialIndexed(std::string materialName, int index)
{
  if (base.materials.size() <= index)
    throw std::runtime_error("Index of material is out of range!");
  auto bsMatName = TOOLS::ReplaceFSlashWithBSlash(materialName);
  base.materials[index] = materialName;
}
