/******************************************************************************/
/*!
\file		Matrix4x4.h
\author		Zhou Zhihua
\par		game title: TRASHBOMINATION
\par		email: zhihua.z\@digipen.edu
\date		December 04, 2017
\brief

Matrix 4x4 for maths

All content C 2017 DigiPen(SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Vector.h"
#include <glm.hpp>

class Matrix3x3;

class Matrix4x4
{
  float arr[16];
public:
  Matrix4x4();
  Matrix4x4(const Matrix3x3&);
  Matrix4x4(const float* values, unsigned size);
  Matrix4x4(const glm::mat4&);
  Matrix4x4(
    float e00, float e10, float e20, float e30,
    float e01, float e11, float e21, float e31,
    float e02, float e12, float e22, float e32,
    float e03, float e13, float e23, float e33);
  Matrix4x4(const Matrix4x4& m);
  Matrix4x4(float x);
  Matrix4x4(const Vector4&, const Vector4&, const Vector4&, const Vector4&);
  ~Matrix4x4();

  float operator[](int num) const
  {
	  return arr[num];
  }
  float& operator[](int num)
  {
	  return arr[num];
  }

  bool operator==(const Matrix4x4& m) const
  {
	  return arr[0] == m[0] && arr[1] == m[1] && arr[2] == m[2] && arr[3] == m[3] &&
		  arr[4] == m[4] && arr[5] == m[5] && arr[6] == m[6] && arr[7] == m[7] &&
		  arr[8] == m[8] && arr[9] == m[9] && arr[10] == m[10] && arr[11] == m[11] &&
		  arr[12] == m[12] && arr[13] == m[13] && arr[14] == m[14] && arr[15] == m[15];
  }
  bool operator!=(const Matrix4x4& m) const
  {
	  return !(*this == m);
  }
  Matrix4x4&  operator=(const Matrix4x4& m);
  Matrix4x4  operator*(const Matrix4x4& r);
  Matrix4x4  operator*(const Matrix4x4& r) const;
  Vector4       operator*(const Vector4& r);
  Vector3       operator*(const Vector3& r);

  float*          Begin();
  const float*    Begin() const;
  float*          End();
  const float*    End() const;


  float&          operator()(unsigned int c, unsigned int r);
  const float&    operator()(unsigned int c, unsigned int r) const;
  const float*	  data() const;
  float*		  data();


  Vector3 GetRow3(unsigned int r) const;
  Vector4 GetRow4(unsigned int r) const;
  Vector3 GetCol3(unsigned int c) const;
  Vector4 GetCol4(unsigned int c) const;

  operator glm::mat4()
  {
	  glm::mat4 returnMat;

	  for (unsigned i = 0; i < 4; ++i)
	  {
		  for (unsigned j = 0; j < 4; ++j)
		  {
			  returnMat[i][j] = arr[i * 4 + j];
		  }
	  }
	  return returnMat;
  }

  static Matrix4x4    Translate(const Vector3& t);
  static Matrix4x4    Rotate(float theta, const Vector4& u);
  static Matrix4x4    Rotate(const Matrix4x4& mat, float theta, const Vector3& u);
  static Vector4	  Rotate(const Vector4& start, float theta, const Vector3& u);
  static Matrix4x4    RotateX(float theta);
  static Matrix4x4    RotateY(float theta);
  static Matrix4x4    RotateZ(float theta);
  static void		  GetRotationAngle(const Matrix4x4& mat, float & x, float & y, float & z);
  static Matrix4x4    Scale(const Vector3& scaleVec);
  static Matrix4x4    LookAt(const Vector3& eye, const Vector3& at, const Vector3& up);
  static Matrix4x4    LookAtRm(const Vector3& eye, const Vector3& at, const Vector3& up);
  static Matrix4x4    Ortho(float left, float right, float btm, float top, float zNear, float zFar);
  //angle in radians
  static Matrix4x4	  Perspective(float angle, float ratio, float nearPlane, float farPlane);
  static Matrix4x4 Inverse(const Matrix4x4&);
  static Matrix4x4 Transpose(const Matrix4x4& arr);
  static Matrix4x4 AffineInverse(const Matrix4x4&);

};

std::ostream& operator<<(std::ostream& os, const Matrix4x4& arr);