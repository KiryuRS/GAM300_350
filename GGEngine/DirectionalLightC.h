/*****************************************************************************/
/*!
\file DirectionalLightC.h
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
#pragma once
#include "Component.h"
#include "Light.h"

class TransformC;

class DirectionalLightC : public Component
{
  Light mLight;

public:
  DirectionalLightC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::DIRECTIONALLIGHT; }
  //Add these functions if you need them
  /*void EditorGUI() override;
  void Initialize() override;
  void Update(float) override;*/
  void Initialize() override;
  void EditorSetup();
  void EditorUpdate(float);
  void toggleVisible() {}
  void DestroyUpdate();
  void Update(float dt) { EditorUpdate(dt); }
};