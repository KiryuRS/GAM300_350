/*****************************************************************************/
#pragma once
#include "Component.h"

class TransformC;
class StaticMeshC;
class Texture;

class StaticMeshRendererC : public Component
{
  TransformC*   mTransform;
  StaticMeshC*  mMesh;
  float         mTransparency = { 1.0f };
  Vector4       mTint;
  bool          mCastShadow = { true };
  bool          mReceiveShadow = { true };
  bool			mVisible = true;
public:

  StaticMeshRendererC();
  void          AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::ST_MESH_RENDERER; }
  bool          EditorGUI(Editor& edit) override;
  void          Awake() override;
  void          Initialize() override;
  void          EditorSetup();
  void			Render() override;
};