/*****************************************************************************/
/*!
\file StaticMeshC.h
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 8, 2018
\brief

This is a static mesh container, it holds the mesh that current object is using
then pass it to the renderer to render.

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once
#include "BaseMeshC.h"

struct StaticMesh;
class StaticMeshC : public Component
{
public:
	BaseMeshC<StaticMesh, StaticMeshC> base;
	StaticMeshC();
	void AddSerializeData(LuaScript* state = nullptr) override;
	static constexpr auto GetType() { return COMPONENTTYPE::STATICMESH; }
	bool EditorGUI(Editor& edit) override;
	void EditorSetup() override;
  void EditorUpdate(float) override;
  void Update(float) override
  {
	  base.Update();
  }
  void SetMesh(std::string meshName);
  void SetStaticMaterial(std::string materialName);
  std::string GetMeshname();
};