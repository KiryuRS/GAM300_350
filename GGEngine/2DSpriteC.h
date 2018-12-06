/*****************************************************************************/
/*!
\file 2DSpriteC.h
\author Zhou Zhihua
\par zhihua.z\@digipen.edu
\date November 11, 2018
\brief

2D billboard 

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*****************************************************************************/

#pragma once
#include "BaseMeshC.h"

struct SpriteAnimationData
{

  std::string animationName;

  int frameStart;
  int amountOfFrames;

  float singleSpeed; // interval

  bool flip;


  // custom frame speed advance
  bool customFrameSpeed;
  std::vector<float> frameSpeed;

  // custom frame switching
  bool customFrames;
  std::vector<int> frames;



};

class Sprite2DC : public Component
{
	int pos = 0;
  TransformC*   mTransform;
  float     m_transparency = { 1.0f };
  Vector4   mTint = { Vector4(0.0f, 0.0f, 0.0f, 0.0f) };
  bool      mAlwaysFront = { false };
  bool      mRadio = { false };
  bool      mHorizontal = { false };
  bool      mVertical = { false };
  float     mValue = { 0.0f };
  bool		mVisible = true;
  SpriteAnimType       mMode = { SpriteAnimType::None };

	bool      ChooseTextureGUI();
	bool lookAtCam = false;

  /// animator Data
  	//slices x and y
  unsigned slices_Row{ 1 };
	unsigned slices_Col{ 1 };

	int current_frame{ 0 };
	float sprite_timer{ 0 };
	int play_counter{ 0 };
	std::vector<std::string> current_animationName;
  bool loop{ false };
	bool current_playing{ false };

	bool play_onStart{ false };

  std::string start_animationName{ "" };

	std::vector<SpriteAnimationData> animation_sprite;
	SpriteAnimationData* playing_Sprite;


	std::string GetCurrentAnimation()
	{
		if (!current_animationName.empty())
			return current_animationName[0];
		else
			return "no animation is playing";
	}

	bool PlayAnimation(const std::string &);

  bool CheckAnimationList(const std::string & rhs);

	void LayerNextAnimation(const std::string & animation_name)
	{

    if (current_animationName.empty())
      PlayAnimation(animation_name);
		else if(CheckAnimationList(animation_name))
		  current_animationName.push_back(animation_name);


	}

	bool If_Playing()
	{
		return current_playing;
	}


public:
  std::string mTexture = "CompiledAssets/Sprites/sample.dds";
  int       mTexPos{ -1 };
  Sprite2DC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::SPRITE2D; }
  void Update(float) override;
  bool EditorGUI(Editor& edit) override;
  void EditorUpdate(float) override;
  void UpdateMinMax(const std::vector<Mesh>&);
  void AnimationUpdate(float dt);

  void SetAndUpdateTexture(const std::string& str, bool update);

  // ==== FOR LUA SCRIPTING ====
  void SetTexture(std::string);
  void SetTransparency(float value);
  void SetVisibility(bool value);
  void SetTint(Vector4 value);
  // ==== END LUA SCRIPTING ====
  void UpdateTextureNum();
  void EditorSetup() override;
  void Render() override;
};