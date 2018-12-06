#include "stdafx.h"
#include "StaticMeshRendererC.h"
#include "Texture.h"

StaticMeshRendererC::StaticMeshRendererC():
	mTint{Vector4{1.f,1.f,1.f,0.f}}, mTransparency(1.f)
{
  RequiredComponents(mTransform, mMesh);
}

void StaticMeshRendererC::AddSerializeData(LuaScript * state)
{
  AddSerializable("Transparency", mTransparency, "transparency of this object in the world", state);
  AddSerializable("Tint", mTint, "tint of this object in the world", state);
  AddSerializable("Cast Shadow", mCastShadow, "Whether the object will cast shadow", state);
  AddSerializable("Receive Shadow", mReceiveShadow, "Whether the object will receive shadow", state);
  AddSerializable("Visible", mVisible, "", state);
}

bool StaticMeshRendererC::EditorGUI(Editor&)
{
	return false;
}

void StaticMeshRendererC::Awake()
{

}

void StaticMeshRendererC::Initialize()
{
	if (mMesh)
	{
    mMesh->base.mesh_ptr = GFX_S.GetStaticMesh(mMesh->base.modelDirectory);
    mMesh->base.UpdateTransformMinMax();
	}
}

void StaticMeshRendererC::EditorSetup()
{
	Initialize();
}
void StaticMeshRendererC::Render()
{
	if (!mVisible) return;
	ModelDetails md;
	md.transformation = *mTransform->GetTransformationMatrix();
	md.model = mMesh->base.modelDirectory;
	md.uv = { 0.0f, 0.0f };
	md.opacity = mTransparency;
	md.tint = mTint;
	md.type = MODELTYPE::STATIC;
	md.materials = mMesh->base.materials;
	md.cast_shadow = mCastShadow;
	md.recv_shadow = mReceiveShadow;
	BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWMODEL, std::move(md), GetGameSpaceID() });

}
