#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix4x4.h"

#define PIDegree 180.0f/3.1415926535f
#define DegreePI 3.1415926535f/180.0f

class TransformC :
	public Component
{

	
	Matrix4x4 transformMatrix;
	Matrix4x4 rotationMatrix;
	Matrix4x4 scaleMatrix;
	Matrix4x4 translateMatrix;
	Matrix4x4 localTransformMatrix;
  Vector3 position;



  Vector3 extends;

	void UpdateRotation();			// to set the new forward , side and up vector



	//Matrix3x3 ParentRotationMatrix(Entity* parent,Matrix3x3 input);
	//Vector3 ParentPosition(Entity* parent, Vector3 input);
	//Vector3 ParentScale(Entity* parent, Vector3 input);

	//Matrix3x3 ParentRotationMatrix(Entity* parent, Matrix3x3 input);
	//Vector3 ParentPosition(Entity* parent, Vector3 input);
	//Vector3 ParentScale(Entity* parent, Vector3 input);

public:
	// For ImGuiGizmos
	float gizmos_matrix[16]
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	Vector3 gizmosPosition;
	Vector3 gizmosRotation;
	Vector3 gizmosScale;

	Vector3 localPosition;
	Vector3 localRotation;
	Vector3 localScale;
	Vector3 ModelMax;
	Vector3 ModelMin;
	TransformC();
	void AddSerializeData(LuaScript* state = nullptr) override;
	void Initialize() override;
	void UpdateLocal();

	// Setter
	void MoveObject(Vector3 amt)
	{
		//position += amt;
		localPosition += amt;
	}

	void MoveForward(float amt)
	{
		localPosition.z += amt;

	}

	void MoveSide(float amt)
	{
		localPosition.x += amt;

	}

	void MoveUp(float amt)
	{
		localPosition.y += amt;
	
	}

	void RotateObject(Vector3 rot)
	{
		localRotation += rot;


		UpdateRotation();
	}

	void ScaleObject(Vector3 sca)
	{
		localScale += sca;
	}

	void SetPosition(Vector3 pos);

	void SetGlobalPosition(Vector3 pos)
	{
		if (owner->parent)
		{
			TransformC* parentTrans = owner->parent->GetComponent<TransformC>();

			Matrix4x4 localScaleM = Matrix4x4::Inverse(TransformC::GetParentScaleMatrix(owner->parent->parent, *parentTrans->GetScaleMatrix()));


			Vector3 parentToChildPos = pos - parentTrans->GetGlobalPosition();

			localPosition = localScaleM * Matrix4x4::Inverse(TransformC::GetParentRotationMatrix(owner->parent->parent, *parentTrans->GetRotationMatrix()))  * parentToChildPos;

			UpdateLocal();
		}
		else
			localPosition = pos;

	}

	void SetRotation(Vector3 rot)
	{
		localRotation = rot;
		UpdateRotation();
	}

	void SetGlobalRotation(Vector3 rot)
	{
		//localRotation = rot;
		//UpdateRotation();


		//rotationMatrix = Matrix4x4::RotateX(rot.x) * Matrix4x4::RotateY(rot.y) * Matrix4x4::RotateZ(rot.z);

    //std::cout << rot << " s " << std::endl;

		if (owner->parent)
		{
      rot *= DegreePI;
      rotationMatrix = Matrix4x4{ Matrix3x3::GetQuatMatrix(Vector4::QuaternionEuler(rot)) };

			TransformC* parentTrans = owner->parent->GetComponent<TransformC>();
			Matrix4x4 localRotate = Matrix4x4::Inverse(TransformC::GetParentRotationMatrix(owner->parent->parent, *parentTrans->GetRotationMatrix())) * rotationMatrix;

			Matrix4x4::GetRotationAngle(localRotate, localRotation.x, localRotation.y, localRotation.z);

			localRotation *= PIDegree;

			

		}
		else
		{
			localRotation = rot;
      //std::cout << localRotation << std::endl;
		}

    //UpdateSerializables(localRotation);
	}

	void SetScale(Vector3 sca)
	{
		localScale = sca;
	}

	// Getter
	Vector3 GetGlobalPosition()
	{
		return position;


	}



	Vector3 GetPosition()
	{
		return localPosition;


	}

	Vector3 GetRotation()
	{
		return localRotation;

	}

	Vector3 GetScale()
	{
		return localScale;
	}

	Vector3 GetGlobalScale()
	{
		return GetParentScale(owner->parent, localScale);
	}



	Vector3 GetForwardVec()
	{
		return rotationMatrix * Vector3{ 0,0,1.0f };
	}

	Vector3 GetSideVec()
	{
		return rotationMatrix * Vector3{ 1.0f, 0 ,0 };
	}

	Vector3 GetUpVec()
	{
		return rotationMatrix * Vector3{ 0 , 1.0f , 0 };
	}

  void SetModelData(Vector3 max, Vector3 min)
  {
    ModelMax = max;
    ModelMin = min;
  }

  void GetModelData(Vector3& max, Vector3& min)
  {
    max = ModelMax;
    min = ModelMin;
  }

  void GetModelData(Vector3* max, Vector3* min)
  {
    *max = ModelMax;
    *min = ModelMin;
  }

  Matrix4x4* GetRotationMatrix()
  {
    return &rotationMatrix;
  }

  const Matrix4x4* GetRotationMatrix() const
  {
    return &rotationMatrix;
  }

  Matrix4x4 GetInverseRotationMatrix()
  {
    return Matrix4x4::Inverse(rotationMatrix);
  }

  Matrix4x4 GetInverseScaleMatrix()
  {
    return Matrix4x4::Inverse(scaleMatrix);
  }


  Matrix4x4* GetScaleMatrix()
  {
    return &scaleMatrix;
  }

  const Matrix4x4* GetScaleMatrix() const
  {
    return &scaleMatrix;
  }

  Matrix4x4* GetTransformationMatrix()
  {
	  return &transformMatrix;
  }

  const Matrix4x4* GetTransformationMatrix() const
  {
	  return &transformMatrix;
  }

  Matrix4x4* GetLocalTransformationMatrix()
  {
    return &localTransformMatrix;
  }

  const Matrix4x4* GetLocalTransformationMatrix() const
  {
    return &localTransformMatrix;
  }

  void UpdateTransformationMatrix();

  Matrix4x4 GetParentMatrix(Entity* parent, const Matrix4x4 &);
  static Matrix4x4 GetParentScaleMatrix(Entity* parent, const Matrix4x4 &);
  static Matrix4x4 GetParentRotationMatrix(Entity* parent, const Matrix4x4 &);
  Vector3 GetParentScale(Entity* parent, const Vector3 &);

  void LookAt(Vector3 pos);
  void Awake() override;

	static constexpr auto GetType() { return COMPONENTTYPE::TRANSFORM; }
	inline bool MinMaxSet() const
	{
		if (ModelMin == Vector3(-0.15f, -0.15f, -0.15f) && 
			ModelMax == Vector3(0.15f, 0.15f, 0.15f))
			return false;
		return true;
	}
};

struct EditorTransformUpdate
{
	static TransformC* transformHost;
	static Vector3 localPosition;
	static Vector3 localRotation;
	static Vector3 localScale;

};




