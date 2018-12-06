/*****************************************************************************/
/*!
\file SpotLightC.h
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date September 24, 2018
\brief

This is a container for spot light data

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/
#pragma once
#include "Component.h"
#include "Light.h"

class TransformC;

class SpotLightC : public Component
{
  TransformC* transform;
  Light mLight;
  float innerAngle = { 10.0f };
  float outerAngle = { 30.0f };


public:
  SpotLightC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::SPOTLIGHT; }
  //Add these functions if you need them
  /*void EditorGUI() override;
  void Awake() override;
  void Initialize() override;
  void Update(float) override;*/
  void EditorSetup();
  void Initialize() override;
  void EditorUpdate(float);
  void Update(float dt) { EditorUpdate(dt); }
  void toggleVisible() {}
  void DestroyUpdate();
  void EditorDestroyUpdate() { DestroyUpdate(); }
};