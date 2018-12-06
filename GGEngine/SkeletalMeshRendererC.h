/*****************************************************************************/
/*!
\file SkeletalMeshRendererC.cpp
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
#pragma once
#include "Component.h"

class TransformC;
class SkeletalMeshC;
class Texture;

class SkeletalMeshRendererC : public Component
{
  TransformC*     transform;
  SkeletalMeshC*  mesh;
  Texture*        texture_id;
  bool            mCastShadow = { true };
  bool            mReceiveShadow = { true };
  
  // animation related
  bool mAnimationPlaying = { true };
  unsigned mAnimationID = { 0 };
  bool looping;
  float m_transparency = { 1.0f };
  Vector4 tint;
  float earlyStopTime = 0.f;

public:
  SkeletalMeshRendererC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::SK_MESH_RENDERER; }
  //Add these functions if you need them
  bool EditorGUI(Editor& edit) override;
  void Awake() override;
  void Initialize() override;
  void Update(float) override;
  void EditorSetup();
  void EditorUpdate(float);
  void Render() override;
  void toggleVisible() {}
  bool AnimationFinished();
  float AnimationDuration(int animNum);
  void PlayAnimation();
  void SetAnimation(std::string);
  float AnimNameDuration(std::string);
  float CurrAnimDuration();
};