/******************************************************************************/
/*!
\file		Matrix3x3.h
\author		Zhou Zhihua
\par		game title: TRASHBOMINATION
\par		email: zhihua.z\@digipen.edu
\date		December 04, 2017
\brief

Matrix 3x3 for maths

All content C 2017 DigiPen(SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Vector.h"

class Matrix3x3
{
  float arr[9];
public:
  Matrix3x3();
  Matrix3x3(
    float e00, float e10, float e20,
    float e01, float e11, float e21,
    float e02, float e12, float e22);
  Matrix3x3(const Matrix3x3& m);
  Matrix3x3(float x);
  ~Matrix3x3();


  Matrix3x3&  operator=(const Matrix3x3& m);
  Matrix3x3& operator-=(const Matrix3x3& m);
  Matrix3x3  operator*(const Matrix3x3& r);
  Vector3       operator*(const Vector3& r) const;

  float*          Begin();
  const float*    Begin() const;
  float*          End();
  const float*    End() const;
  const float*	  data() const;
  float*		  data();
  
  float&          operator()(unsigned int c, unsigned int r);
  const float&    operator()(unsigned int c, unsigned int r) const;
  
  Vector3 GetRow3(unsigned int r) const;
  Vector3 GetCol3(unsigned int c) const;

  Matrix3x3 Inverse() const;
  float Determinant() const;
  Matrix3x3 Transpose() const;


  static Matrix3x3    Translate(const Vector2& t);
  static Matrix3x3    RotateX(float theta);
  static Matrix3x3    RotateY(float theta);
  static Matrix3x3    RotateZ(float theta);
  static Matrix3x3    Scale(const Vector2& scaleVec);
  static Matrix3x3 GetQuatMatrix(const Vector4&vv);
};
