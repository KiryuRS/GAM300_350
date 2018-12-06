#include "stdafx.h"

bool Sprite2DC::ChooseTextureGUI()
{
	bool returnBool = false;
  auto& tList = GFX_S.GetTextureList();
  ImGui::Text("Texture");
  TOOLS::PushID(UITYPE::DEFAULT);
  if (ImGui::Combo("", &pos, tList.data(), int(tList.size()), int(tList.size())))
  {
	  returnBool = true;
    mTexture = tList[pos];
    UpdateSerializables(mTexture);
  }
  ImGui::PopID();
  return returnBool;
}

Sprite2DC::Sprite2DC()
{
  RequiredComponents(mTransform);
}

void Sprite2DC::AddSerializeData(LuaScript * state)
{
  AddSerializable("Radio", mRadio, "", state);
  AddSerializable("Horizontal", mHorizontal, "", state);
  AddSerializable("Vertical", mVertical, "", state);
  AddSerializable("Value", mValue, "", state);
  AddSerializable("Texture", mTexture, "", state).display = false;
  AddSerializable("lookAtCam", lookAtCam, "", state);
  AddSerializable("Always Front", mAlwaysFront, "", state);
  AddSerializable("Tint", mTint, "", state);

  AddSerializable("slices_Row", slices_Row, "", state);
  AddSerializable("slices_Col", slices_Col, "", state);
  AddSerializable("current_frame", current_frame, "", state);
  AddSerializable("loop", loop, "", state);
  AddSerializable("play_onStart", play_onStart, "", state);
  AddSerializable("onStart_animationName", start_animationName, "", state);
  //AddSerializable("animation_sprite", animation_sprite, "", state);
  AddSerializable("Transparency", m_transparency, "", state);
  AddSerializable("Visible", mVisible, "", state);
  AddCallableFunction("SetTexture", &Sprite2DC::SetTexture, state);
  AddCallableFunction("SetTransparency", &Sprite2DC::SetTransparency, state);
  AddCallableFunction("SetVisibility", &Sprite2DC::SetVisibility, state);
  AddCallableFunction("SetTint", &Sprite2DC::SetTint, state);
}

void Sprite2DC::Update(float dt)
{
	AnimationUpdate(dt);
}

bool Sprite2DC::EditorGUI(Editor & edit)
{
	UpdateComponentData(m_transparency);
	UpdateComponentData(mTint);
  return ChooseTextureGUI();
}

void Sprite2DC::EditorUpdate(float dt)
{
  Update(dt);
}

void Sprite2DC::UpdateMinMax(const std::vector<Mesh>& tmp)
{
	Vector3 max = tmp[0].toprightback;
	Vector3 min = tmp[0].btmleftfront;

	for (const auto & elem : tmp)
	{
		if (elem.toprightback.x > max.x)
			max.x = elem.toprightback.x;

		if (elem.toprightback.y > max.y)
			max.y = elem.toprightback.y;

		if (elem.toprightback.z > max.z)
			max.z = elem.toprightback.z;

		if (elem.btmleftfront.x < min.x)
			min.x = elem.btmleftfront.x;

		if (elem.btmleftfront.y < min.y)
			min.y = elem.btmleftfront.y;

		if (elem.btmleftfront.z < min.z)
			min.z = elem.btmleftfront.z;
	}
	mTransform->SetModelData(max, min);
}


bool Sprite2DC::PlayAnimation(const std::string & rhs)
{
  


  for (auto & elem : animation_sprite)
  {
    if (elem.animationName == rhs)
    {
      playing_Sprite = &elem;
      SpriteAnimationData & current_Sprite = *playing_Sprite;

      if (current_Sprite.customFrames)
      {
        if (!current_Sprite.frames.empty())
          current_frame = current_Sprite.frames[0];
        else
          current_frame = current_Sprite.frameStart;
      }
      else
      {
        current_frame = current_Sprite.frameStart;
      }


      sprite_timer = 0;

      current_playing = true;

      play_counter = 0;
      current_animationName.clear();
      current_animationName.push_back(rhs);
      return true;
    }
  }



  return false;
}


void Sprite2DC::AnimationUpdate(float dt)
{
  if (!current_animationName.empty())
    return;

  if (!current_playing)
    return;

  bool next_Anim = false;
  sprite_timer += dt;

  SpriteAnimationData & current_Sprite = *playing_Sprite;

  if (!current_Sprite.customFrameSpeed)
  {
    while (sprite_timer >= current_Sprite.singleSpeed)
    {
      sprite_timer -= current_Sprite.singleSpeed;

      play_counter++;

      if (current_Sprite.customFrames)
      {
        if (play_counter > current_Sprite.amountOfFrames)
        {
          next_Anim = true;

          if (loop)
          {
            current_animationName.push_back(current_animationName[0]);
          }
          current_animationName.erase(current_animationName.begin());

        }
      }
      else
      {
        if (play_counter > current_Sprite.frames.size())
        {
          next_Anim = true;


          if (loop)
          {
            current_animationName.push_back(current_animationName[0]);
            
          }
          current_animationName.erase(current_animationName.begin());


        }
      }
    }
  }
  else
  {
    while (sprite_timer >= current_Sprite.frameSpeed[play_counter])
    {
      sprite_timer -= current_Sprite.frameSpeed[play_counter];

      play_counter++;

      if (current_Sprite.customFrames)
      {
        if (play_counter > current_Sprite.amountOfFrames)
        {
          next_Anim = true;

          if (loop)
          {
            current_animationName.push_back(current_animationName[0]);
          }
          current_animationName.erase(current_animationName.begin());

        }
      }
      else
      {
        if (play_counter > current_Sprite.frames.size())
        {
          next_Anim = true;


          if (loop)
          {
            current_animationName.push_back(current_animationName[0]);

          }
          current_animationName.erase(current_animationName.begin());
        }
      }
    }
  }

  if (next_Anim)
  {

    if (!current_animationName.empty())
    {
      int itr = 0;
      for (auto & elem : animation_sprite)
      {
        if (elem.animationName == current_animationName[0])
          break;
        itr++;
      }

      playing_Sprite = &animation_sprite[itr];
      SpriteAnimationData & current_Sprite2 = *playing_Sprite;

      if (current_Sprite2.customFrames)
      {
        if (!current_Sprite2.frames.empty())
          current_frame = current_Sprite2.frames[0];
        else
          current_frame = current_Sprite2.frameStart;
      }
      else
      {
        current_frame = current_Sprite2.frameStart;
      }


      sprite_timer = 0;


      play_counter = 0;
    }
    else
      current_playing = false;

  }
}

void Sprite2DC::SetAndUpdateTexture(const std::string & str, bool update)
{
	mTexture = str;
	if (update)
		UpdateSerializables(mTexture);
}

bool Sprite2DC::CheckAnimationList(const std::string & rhs)
{
  for (auto & elem : animation_sprite)
  {
    if (elem.animationName == rhs)
    {
      return true;

    }
  }
  return false;
}

void Sprite2DC::SetTexture(std::string textureName)
{
	// Check if we only have the filename and no relative path
	if (textureName.find('/') == std::string::npos || textureName.find('\\') == std::string::npos)
	{
		// Append the relative path to it
		static const std::string relativePath{ "CompiledAssets/Sprites/" };
		textureName = relativePath + textureName;
	}

	mTexture = textureName;
}

void Sprite2DC::SetTransparency(float value)
{
	m_transparency = value;
}

void Sprite2DC::SetVisibility(bool value)
{
	mVisible = value;
}

void Sprite2DC::SetTint(Vector4 value)
{
	mTint = value;
}

void Sprite2DC::UpdateTextureNum()
{
	auto& list = GFX_S.GetTextureList();
	for (unsigned i = 0; i < list.size(); ++i)
	{
		std::string ll = list[i];
		if (ll == mTexture)
		{
			pos = i;
			return;
		}
	}
	CONSOLE_ERROR("Could not find texture ", mTexture, "!");
}

void Sprite2DC::EditorSetup()
{
	UpdateTextureNum();
}


void Sprite2DC::Render()
{
	if (!mVisible) return;
	if (lookAtCam)
	{
		auto lookAtM = Matrix4x4::LookAtRm(mTransform->GetGlobalPosition(), owner->entityList->GetActiveCamera()->GetCameraPosition(), Vector3{ 0,1.0f,0 });
		Matrix4x4::GetRotationAngle(lookAtM, mTransform->localRotation.x,
			mTransform->localRotation.y, mTransform->localRotation.z);
		mTransform->localRotation *= PIDegree;

		mTransform->UpdateSerializables(mTransform->localRotation);
	}

	SpriteDetails md;
	md.transformation = *mTransform->GetTransformationMatrix();
	md.texture = mTexture;
	md.uv = { 0.0f, 0.0f };
	md.opacity = m_transparency;
	md.tint = mTint;
	md.always_front = mAlwaysFront;
	md.SAdata.arr[0] = mValue;

	auto sm = GFX_S.GetStaticMesh("CompiledAssets/Models/skybox_plane.mdl");
	UpdateMinMax(sm->meshes);

	if (mRadio)
		md.SAtype = SpriteAnimType::Radio;
	else if (mHorizontal)
		md.SAtype = SpriteAnimType::Horizontal;
	else if (mVertical)
		md.SAtype = SpriteAnimType::Vertical;
	else
		md.SAtype = SpriteAnimType::None;

	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWSPRITE, std::move(md), GetGameSpaceID() });
}