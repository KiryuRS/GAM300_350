#include "stdafx.h"
#include "SkeletalMeshRendererC.h"
#include "Texture.h"
#include "SkeletalMesh.h"

SkeletalMeshRendererC::SkeletalMeshRendererC():
	tint{ Vector4{1.f,1.f,1.f,0.f} }, m_transparency(1.f),
	looping(true)
{
  RequiredComponents(transform, mesh);
}

void SkeletalMeshRendererC::AddSerializeData(LuaScript * state)
{
	AddSerializable("Transparency", m_transparency, "transparency of this object in the world", state);
	AddSerializable("Tint", tint, "tint of this object in the world", state);
  AddSerializable("Cast Shadow", mCastShadow, "Whether the object will cast shadow", state);
  AddSerializable("Receive Shadow", mReceiveShadow, "Whether the object will receive shadow", state);
	AddSerializable("Loop", looping, "", state);
	AddSerializable("EarlyStopTime", earlyStopTime, "How much earlier to stop the animation", state);
	AddCallableFunction("AnimationDuration", &SkeletalMeshRendererC::AnimationDuration, state);
	AddCallableFunction("AnimNameDuration", &SkeletalMeshRendererC::AnimNameDuration, state);
	AddCallableFunction("AnimationFinished", &SkeletalMeshRendererC::AnimationFinished, state);
	AddCallableFunction("PlayAnimation", &SkeletalMeshRendererC::PlayAnimation, state);
	AddCallableFunction("SetAnimation", &SkeletalMeshRendererC::SetAnimation, state);
	AddCallableFunction("CurrAnimDuration", &SkeletalMeshRendererC::CurrAnimDuration, state);
}

bool SkeletalMeshRendererC::EditorGUI(Editor&)
{
	if (ImGui::Button("Replay Animation"))
	{
		PlayAnimation();
	}
	return false;
}

void SkeletalMeshRendererC::Awake()
{
	if (mesh)
	{
		mesh->base.mesh_ptr = GFX_S.GetSkeletalMesh(mesh->base.modelDirectory.c_str());
		mesh->base.UpdateTransformMinMax();
	}
}

void SkeletalMeshRendererC::Initialize()
{
}

void SkeletalMeshRendererC::Update(float dt)
{
	//mesh->base.UpdateTransformMinMax();
	SkeletalMesh* m = mesh->base.mesh_ptr;
	if (m->animations.size())
	{
		if (mAnimationPlaying)
			mesh->base.animTime += (float)m->animations[mesh->base.animNo].ticks_per_sec * dt;
		if (mesh->base.animTime > m->animations[mesh->base.animNo].duration - earlyStopTime)
		{
			if (looping)
				mesh->base.animTime -= (float)m->animations[mesh->base.animNo].duration - earlyStopTime;
			else
			{
				mAnimationPlaying = false;
				mesh->base.animTime = m->animations[mesh->base.animNo].duration - earlyStopTime - 0.01f;
			}
		}
	}
}

void SkeletalMeshRendererC::EditorSetup()
{
	Awake();
}

void SkeletalMeshRendererC::EditorUpdate(float dt)
{
	Update(dt);
}

void SkeletalMeshRendererC::Render()
{
	ModelDetails md;
	md.transformation = *transform->GetTransformationMatrix();
	md.model = mesh->base.modelDirectory;
	md.uv = { 0.0f, 0.0f };
	md.opacity = m_transparency;
	md.tint = tint;
	md.type = MODELTYPE::SKELETAL;
	md.animNum = mesh->base.animNo;
	md.animTime = mesh->base.animTime;
	md.materials = mesh->base.materials;
	md.cast_shadow = mCastShadow;
	md.recv_shadow = mReceiveShadow;
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWMODEL, std::move(md), GetGameSpaceID() });
}

bool SkeletalMeshRendererC::AnimationFinished()
{
	if (mesh)
	{
		if (mesh->base.mesh_ptr->animations.size() == 0)
			return true;
		if (!looping && !mAnimationPlaying)
			return true;
	}
	return false;
}

float SkeletalMeshRendererC::AnimationDuration(int animNum)
{
	if (mesh)
	{
		if (mesh->base.mesh_ptr)
		{
			if (mesh->base.mesh_ptr->animations.size() <= animNum)
			{
				std::stringstream ss;
				ss << "Cannot play animation " << animNum << " in "
					<< mesh->base.modelDirectory << "!";
				throw std::runtime_error(ss.str());
			}
			return mesh->base.mesh_ptr->animations[animNum].duration - earlyStopTime;
		}
	}
	return 0.0f;
}

void SkeletalMeshRendererC::PlayAnimation()
{
	mAnimationPlaying = true;
	mesh->base.animTime = 0.f;
}

void SkeletalMeshRendererC::SetAnimation(std::string animName)
{
	auto meshPtr = mesh->base.mesh_ptr;
	if (!meshPtr)
	{
		throw std::runtime_error("No mesh has been set yet!");
	}
	int num = 0;
	for (auto& item : meshPtr->animations)
	{
		if (item.name == animName)
		{
			mesh->base.animNo = num;
      mesh->base.animTime = 0.f;
			return;
		}
		++num;
	}
	std::stringstream ss;
	ss << "Could not find animation " << animName << "!";
	throw std::runtime_error(ss.str());
}

float SkeletalMeshRendererC::AnimNameDuration(std::string animName)
{
	auto meshPtr = mesh->base.mesh_ptr;
	if (!meshPtr)
	{
		throw std::runtime_error("No mesh has been set yet!");
	}
	int num = 0;
	for (int i = 0; i < meshPtr->animations.size(); ++i)
	{
		if (meshPtr->animations[i].name == animName)
		{
			return AnimationDuration(i);
		}
		++num;
	}
	std::stringstream ss;
	ss << "Could not find animation " << animName << "!";
	throw std::runtime_error(ss.str());
}

float SkeletalMeshRendererC::CurrAnimDuration()
{
	if (mesh)
	{
		if (mesh->base.mesh_ptr)
		{
			if (mesh->base.mesh_ptr->animations.size() <= mesh->base.animNo)
			{
				std::stringstream ss;
				ss << "Cannot play animation " << mesh->base.animNo << " in "
					<< mesh->base.modelDirectory << "!";
				throw std::runtime_error(ss.str());
			}
			return mesh->base.mesh_ptr->animations[mesh->base.animNo].duration - earlyStopTime;
		}
	}
	return 0.0f;
}
