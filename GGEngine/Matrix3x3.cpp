/******************************************************************************/
/*!
\file		Matrix3x3.cpp
\author		Zhou Zhihua
\par		game title: TRASHBOMINATION
\par		email: zhihua.z\@digipen.edu
\date		December 04, 2017
\brief

Matrix 3x3 for maths

All content C 2017 DigiPen(SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Matrix3x3.h"

Matrix3x3::Matrix3x3()
  : arr{
  1, 0, 0,
  0, 1, 0,
  0, 0, 1}
{
}

Matrix3x3::Matrix3x3(float e00, float e10, float e20, float e01, float e11, float e21, float e02, float e12, float e22)
  : arr{ e00, e10, e20,
  e01, e11, e21,
  e02, e12, e22}
{
}

Matrix3x3::Matrix3x3(const Matrix3x3 & m)
  : arr{
  m.arr[0], m.arr[1], m.arr[2],
  m.arr[3], m.arr[4], m.arr[5],
  m.arr[6], m.arr[7], m.arr[8]}
{
}

Matrix3x3::Matrix3x3(float x)
  : arr{
  x, 0, 0,
  0, x, 0,
  0, 0, x}
{
}

Matrix3x3::~Matrix3x3()
{
}

Matrix3x3 & Matrix3x3::operator=(const Matrix3x3 & m)
{
  for (int i = 0; i < 9; ++i)
  {
    arr[i] = m.arr[i];
  }

  return *this;
}

Matrix3x3 & Matrix3x3::operator-=(const Matrix3x3 & m)
{
	for (int i = 0; i < 9; ++i)
	{
		arr[i] -= m.arr[i];
	}

	return *this;
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 & r)
{
  return Matrix3x3(
    GetRow3(0) * r.GetCol3(0),
    GetRow3(1) * r.GetCol3(0),
    GetRow3(2) * r.GetCol3(0),
    GetRow3(0) * r.GetCol3(1),
    GetRow3(1) * r.GetCol3(1),
    GetRow3(2) * r.GetCol3(1),
    GetRow3(0) * r.GetCol3(2),
    GetRow3(1) * r.GetCol3(2),
    GetRow3(2) * r.GetCol3(2));
}

Vector3 Matrix3x3::operator*(const Vector3 & r) const
{
  return Vector3{
    GetRow3(0) * r,
    GetRow3(1) * r,
    GetRow3(2) * r
  };
}

float * Matrix3x3::Begin()
{
  return &arr[0];
}

const float * Matrix3x3::Begin() const
{
  return &arr[0];
}

float * Matrix3x3::End()
{
  return &arr[9];
}

const float * Matrix3x3::End() const
{
  return &arr[9];
}

const float * Matrix3x3::data() const
{
	return &arr[0];
}

float * Matrix3x3::data()
{
	return &arr[0];
}

float & Matrix3x3::operator()(unsigned int c, unsigned int r)
{
  return arr[r * 3 + c];
}

const float & Matrix3x3::operator()(unsigned int c, unsigned int r) const
{
  return arr[r * 3 + c];
}

Vector3 Matrix3x3::GetRow3(unsigned int r) const
{
  return Vector3(
    arr[r],
    arr[r + 3],
    arr[r + 3 * 2]
  );
}

Vector3 Matrix3x3::GetCol3(unsigned int c) const
{
  return Vector3(
    arr[c * 3],
    arr[c * 3 + 1],
    arr[c * 3 + 2]
  );
}

Matrix3x3 Matrix3x3::Inverse() const
{
  float invdet = 1.0f / Determinant();

  Matrix3x3 cofactor = Matrix3x3{ 
    arr[4] * arr[8] - arr[5] * arr[7],
    arr[5] * arr[6] - arr[3] * arr[8],
    arr[3] * arr[7] - arr[4] * arr[6],
    arr[2] * arr[7] - arr[1] * arr[8],
    arr[0] * arr[8] - arr[2] * arr[6],
    arr[1] * arr[6] - arr[0] * arr[7],
    arr[1] * arr[5] - arr[2] * arr[4],
    arr[2] * arr[3] - arr[0] * arr[5],
    arr[0] * arr[4] - arr[1] * arr[3]
  };

  return (cofactor * invdet).Transpose();
}

float Matrix3x3::Determinant() const
{
  return arr[0] * (arr[4] * arr[8] - arr[5] * arr[7])
    - arr[1] * (arr[3] * arr[8] - arr[5] * arr[6])
    + arr[2] * (arr[3] * arr[7] - arr[4] * arr[6]);
}

Matrix3x3 Matrix3x3::Transpose() const
{
  return Matrix3x3(arr[0], arr[3], arr[6], arr[1], arr[4], arr[7], arr[2], arr[5], arr[8]);
}

Matrix3x3 Matrix3x3::Translate(const Vector2 & t)
{
  Matrix3x3 temp;

  temp.arr[6] = t.x;
  temp.arr[7] = t.y;

  return temp;
}

Matrix3x3 Matrix3x3::RotateX(float theta)
{
  float cosine = cosf(theta);
  float sine = sinf(theta);

  return Matrix3x3(
	1, 0, 0,
    0, cosine, sine,
    0, -sine, cosine
  );
}

Matrix3x3 Matrix3x3::RotateY(float theta)
{
	float cosine = cosf(theta);
	float sine = sinf(theta);

	return Matrix3x3(
		cosine, 0, -sine,
		0,	1, 0,
		sine, 0 , cosine

	);
}

Matrix3x3 Matrix3x3::RotateZ(float theta)
{
	float cosine = cosf(theta);
	float sine = sinf(theta);

	return Matrix3x3(
		cosine, sine, 0,
		-sine, cosine, 0,
		0, 0, 1
	);
}

Matrix3x3 Matrix3x3::Scale(const Vector2 & scaleVec)
{
  return Matrix3x3{
    scaleVec.x, 0, 0,
    0, scaleVec.y, 0,
    0, 0, 1
  };
}

Matrix3x3 Matrix3x3::GetQuatMatrix(const Vector4 & vv)
{
	Matrix3x3 resMatrix;
	//resMatrix.Begin()[0] = 1.f - 2.f * (vv.y * vv.y + vv.z * vv.z);
	//resMatrix.Begin()[3] = 2.f * (vv.x * vv.y - vv.z * vv.w);
	//resMatrix.Begin()[6] = 2.f * (vv.x * vv.z + vv.y * vv.w);
	//resMatrix.Begin()[1] = 2.f * (vv.x * vv.y + vv.z * vv.w);
	//resMatrix.Begin()[4] = 1.f - 2.f * (vv.x * vv.x + vv.z * vv.z);
	//resMatrix.Begin()[7] = 2.f * (vv.y * vv.z - vv.x * vv.w);
	//resMatrix.Begin()[2] = 2.f * (vv.x * vv.z - vv.y * vv.w);
	//resMatrix.Begin()[5] = 2.f * (vv.y * vv.z + vv.x * vv.w);
	//resMatrix.Begin()[8] = 1.f - 2.f * (vv.x * vv.x + vv.y * vv.y);


  resMatrix.Begin()[0] = vv.w * vv.w + vv.x * vv.x - vv.y * vv.y - vv.z * vv.z;
  resMatrix.Begin()[3] = 2.0f * (vv.x * vv.y - vv.w * vv.z);
  resMatrix.Begin()[6] = 2.0f * (vv.x * vv.z + vv.w * vv.y);
  resMatrix.Begin()[1] = 2.0f * (vv.x * vv.y + vv.w * vv.z);
  resMatrix.Begin()[4] = vv.w * vv.w - vv.x * vv.x + vv.y * vv.y - vv.z * vv.z;
  resMatrix.Begin()[7] = 2.0f * (vv.y * vv.z - vv.w * vv.x);
  resMatrix.Begin()[2] = 2.0f * (vv.x * vv.z - vv.w * vv.y);
  resMatrix.Begin()[5] = 2.0f * (vv.y * vv.z + vv.w * vv.x);
  resMatrix.Begin()[8] = vv.w * vv.w - vv.x * vv.x - vv.y * vv.y + vv.z * vv.z;

	return resMatrix;
}
