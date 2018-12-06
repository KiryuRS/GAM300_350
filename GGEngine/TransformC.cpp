#include "stdafx.h"
#include "TransformC.h"
#include "Matrix3x3.h"
#include "ext.hpp"

#define DegreeToPI 3.1415926535f/180.0f

TransformC* EditorTransformUpdate::transformHost = nullptr;
Vector3 EditorTransformUpdate::localPosition = { 0,0,0 };
Vector3 EditorTransformUpdate::localRotation = { 0,0,0 };
Vector3 EditorTransformUpdate::localScale = { 1.0f,1.0f,1.0f };


TransformC::TransformC() :
	position(0, 0, 0),
	localPosition(0, 0, 0),
	localRotation(0, 0, 0),
	localScale(1.0f, 1.0f, 1.0f),
	transformMatrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f),
	ModelMax{10.0f,10.0f ,10.0f },
	ModelMin{ -10.0f,-10.0f ,-10.0f },
	extends{10.0f,10.0f,10.0f}
{
	
}

void TransformC::AddSerializeData(LuaScript* state)
{
  // AddSerializable("GlobalPosition", position, "", state);
	AddSerializable("Position", localPosition, "", state);
	AddSerializable("Rotation", localRotation, "", state);
	AddSerializable("Scale", localScale, "", state);
	AddCallableFunction("Move_Object", &TransformC::MoveObject, {}, state);
	AddCallableFunction("Rotate_Object", &TransformC::RotateObject, {}, state);
	AddCallableFunction("Scale_Object", &TransformC::ScaleObject, {}, state);
	AddCallableFunction("Set_Position", &TransformC::SetPosition, {}, state);
	AddCallableFunction("Set_Rotation", &TransformC::SetRotation, {}, state);
	AddCallableFunction("Set_Scale", &TransformC::SetScale, {}, state);
	AddCallableFunction("Look_At", &TransformC::LookAt, {"Target"}, state);
	AddCallableFunction("GetGlobalPosition", &TransformC::GetGlobalPosition, state);
}

void TransformC::UpdateLocal()
{
  UpdateSerializables(localPosition);
  UpdateSerializables(localRotation);
  UpdateSerializables(localScale);
}

void TransformC::SetPosition(Vector3 pos)
{
	localPosition = pos;
}

void TransformC::UpdateRotation()
{

	/* to keep the rotation value as 0 to 359*/
	while (localRotation.x > 359.0f)
	{
		localRotation.x -= 360.0f;
	}

	while (localRotation.x < 0)
	{
		localRotation.x += 360.0f;
	}

	while (localRotation.y > 359.0f)
	{
		localRotation.y -= 360.0f;
	}

	while (localRotation.y < 0)
	{
		localRotation.y += 360.0f;
	}

	while (localRotation.z > 359.0f)
	{
		localRotation.z -= 360.0f;
	}

	while (localRotation.z < 0)
	{
		localRotation.z += 360.0f;
	}


	return;
}


void TransformC::UpdateTransformationMatrix()
{
  translateMatrix = Matrix4x4::Translate(localPosition);
  scaleMatrix = Matrix4x4::Scale(localScale);
  UpdateRotation();

  
  Vector3 rot = localRotation * DegreeToPI;


  rotationMatrix = Matrix4x4{ Matrix3x3::GetQuatMatrix(Vector4::QuaternionEuler(rot)) };

  localTransformMatrix = rotationMatrix * scaleMatrix;

  localTransformMatrix = translateMatrix * localTransformMatrix;

  transformMatrix = GetParentMatrix(owner->parent, localTransformMatrix);

  position = Vector3{ transformMatrix.GetCol3(3) };
  
}

Matrix4x4 TransformC::GetParentMatrix(Entity* parent, const Matrix4x4 & childT)
{
	if (parent == nullptr)
		return childT;

	Matrix4x4 *current = parent->GetComponent<TransformC>()->GetLocalTransformationMatrix();

	return GetParentMatrix(parent->parent, (*current)*childT);

}

Matrix4x4 TransformC::GetParentScaleMatrix(Entity* parent, const Matrix4x4 & childT)
{
	if (parent == nullptr)
		return childT;

	Matrix4x4 *current = parent->GetComponent<TransformC>()->GetScaleMatrix();

	return GetParentScaleMatrix(parent->parent, (*current)*childT);

}

Matrix4x4 TransformC::GetParentRotationMatrix(Entity* parent, const Matrix4x4 & childT)
{
	if (parent == nullptr)
		return childT;

	Matrix4x4 *current = parent->GetComponent<TransformC>()->GetRotationMatrix();

	return GetParentRotationMatrix(parent->parent, (*current)*childT);

}

Vector3 TransformC::GetParentScale(Entity* parent, const Vector3 &childT)
{
	if (parent == nullptr)
		return childT;

	Vector3& current = parent->GetComponent<TransformC>()->localScale;

	return GetParentScale(parent->parent, Vector3{ current.x*childT.x,current.y*childT.y, current.z*childT.z });

}

void TransformC::LookAt(Vector3 pos)
{
	auto matrix = Matrix4x4::LookAtRm(position, pos, Vector3(0, 1, 0));

  if (owner->parent)
  {
    TransformC* parentTrans = owner->parent->GetComponent<TransformC>();
    Matrix4x4 localRotate = Matrix4x4::Inverse(TransformC::GetParentRotationMatrix(owner->parent->parent, *parentTrans->GetRotationMatrix())) * matrix;

    Matrix4x4::GetRotationAngle(localRotate, localRotation.x, localRotation.y, localRotation.z);

  }
  else
    Matrix4x4::GetRotationAngle(matrix, localRotation.x, localRotation.y, localRotation.z);

  localRotation *= PIDegree;

  UpdateSerializables(localRotation);
}

void TransformC::Awake()
{
	UpdateTransformationMatrix();
}


void TransformC::Initialize()
{
	UpdateTransformationMatrix();
}

