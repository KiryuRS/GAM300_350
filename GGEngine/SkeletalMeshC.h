/*****************************************************************************/
/*!
\file SkeletalMeshC.h
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

This is a skeletal mesh container, it holds the mesh that current object is using
then pass it to the renderer to render.

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#pragma once
#include "BaseMeshC.h"

struct SkeletalMesh;
class SkeletalMeshRendererC;

class SkeletalMeshC : public Component
{
	float m_transparency = { 1.0f };
	Vector4 tint;
	SkeletalMeshRendererC* renderer;
public:
	BaseMeshC<SkeletalMesh, SkeletalMeshC> base;
  SkeletalMeshC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::SKELETALMESH; }
  bool EditorGUI(Editor& edit) override;
  void EditorSetup() override;
  void EditorUpdate(float) override;
  void SetMesh(std::string);
  void SetMaterial(std::string);
  void SetMaterialIndexed(std::string, int);
  void Update(float) override
  {
	  base.Update();
  }
};