/******************************************************************************/
/*!
\file		Matrix4x4.cpp
\author		Zhou Zhihua
\par		game title: TRASHBOMINATION
\par		email: zhihua.z\@digipen.edu
\date		December 04, 2017
\brief

Matrix 4x4 for maths

All content C 2017 DigiPen(SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Matrix4x4.h"
#include <cmath>

Matrix4x4::Matrix4x4()
  : arr{ 
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1}
{}

Matrix4x4::Matrix4x4(const Matrix3x3 & mm):
	arr{}
{
	for (unsigned i = 0; i < 3; ++i)
	{
		for (unsigned j = 0; j < 3; ++j)
		{
			arr[i * 4 + j] = mm.Begin()[i * 3 + j];
		}
		arr[i * 4 + 4] = 0;
	}
	arr[15] = 1.f;
}

Matrix4x4::Matrix4x4(const float * values, unsigned size)
	: arr{ }
{
	for (unsigned i = 0; i != size; ++i)
		arr[i] = values[i];
}

Matrix4x4::Matrix4x4(const glm::mat4 & mm)
{
	for (unsigned i = 0; i < 4; ++i)
	{
		for (unsigned j = 0; j < 4; ++j)
		{
			arr[i * 4 + j] = mm[i][j];
		}
	}
}

Matrix4x4::Matrix4x4(
  float e00, float e10, float e20, float e30,
  float e01, float e11, float e21, float e31,
  float e02, float e12, float e22, float e32,
  float e03, float e13, float e23, float e33)
  : arr { e00, e10, e20, e30,
  e01, e11, e21, e31,
  e02, e12, e22, e32,
  e03, e13, e23, e33 }
{
}

Matrix4x4::Matrix4x4(const Matrix4x4& m)
  : arr{ 
  m.arr[0], m.arr[1], m.arr[2], m.arr[3],
  m.arr[4], m.arr[5], m.arr[6], m.arr[7],
  m.arr[8], m.arr[9], m.arr[10], m.arr[11],
  m.arr[12], m.arr[13], m.arr[14], m.arr[15] }
{
}


Matrix4x4::Matrix4x4(float x)
  : arr{
  x, 0, 0, 0,
  0, x, 0, 0,
  0, 0, x, 0,
  0, 0, 0, x }
{
}

Matrix4x4::Matrix4x4(const Vector4 & a, const Vector4 &b, const Vector4 &c, const Vector4 &d)
{
	arr[0] = a.x;
	arr[1] = a.y;
	arr[2] = a.z;
	arr[3] = a.w;
	arr[4] = b.x;
	arr[5] = b.y;
	arr[6] = b.z;
	arr[7] = b.w;
	arr[8] = c.x;
	arr[9] = c.y;
	arr[10] = c.z;
	arr[11] = c.w;
	arr[12] = d.x;
	arr[13] = d.y;
	arr[14] = d.z;
	arr[15] = d.w;
}

Matrix4x4::~Matrix4x4()
{
}

Matrix4x4 & Matrix4x4::operator=(const Matrix4x4 & m)
{
  for (int i = 0; i < 16; ++i)
  {
    arr[i] = m.arr[i];
  }

  return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & r)
{
	return Matrix4x4(
		GetRow4(0) * r.GetCol4(0),
		GetRow4(1) * r.GetCol4(0),
		GetRow4(2) * r.GetCol4(0),
		GetRow4(3) * r.GetCol4(0),
		GetRow4(0) * r.GetCol4(1),
		GetRow4(1) * r.GetCol4(1),
		GetRow4(2) * r.GetCol4(1),
		GetRow4(3) * r.GetCol4(1),
		GetRow4(0) * r.GetCol4(2),
		GetRow4(1) * r.GetCol4(2),
		GetRow4(2) * r.GetCol4(2),
		GetRow4(3) * r.GetCol4(2),
		GetRow4(0) * r.GetCol4(3),
		GetRow4(1) * r.GetCol4(3),
		GetRow4(2) * r.GetCol4(3),
		GetRow4(3) * r.GetCol4(3));
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & r) const
{
	return Matrix4x4(
		GetRow4(0) * r.GetCol4(0),
		GetRow4(1) * r.GetCol4(0),
		GetRow4(2) * r.GetCol4(0),
		GetRow4(3) * r.GetCol4(0),
		GetRow4(0) * r.GetCol4(1),
		GetRow4(1) * r.GetCol4(1),
		GetRow4(2) * r.GetCol4(1),
		GetRow4(3) * r.GetCol4(1),
		GetRow4(0) * r.GetCol4(2),
		GetRow4(1) * r.GetCol4(2),
		GetRow4(2) * r.GetCol4(2),
		GetRow4(3) * r.GetCol4(2),
		GetRow4(0) * r.GetCol4(3),
		GetRow4(1) * r.GetCol4(3),
		GetRow4(2) * r.GetCol4(3),
		GetRow4(3) * r.GetCol4(3));
}

Vector4 Matrix4x4::operator*(const Vector4 & r)
{
  return Vector4{
    GetRow4(0) * r,
    GetRow4(1) * r,
    GetRow4(2) * r,
    GetRow4(3) * r
  };
}

Vector3 Matrix4x4::operator*(const Vector3& r)
{
	return Vector3{
		GetRow3(0) * r,
		GetRow3(1) * r,
		GetRow3(2) * r
	};
}

float * Matrix4x4::Begin()
{
  return &arr[0];
}

const float * Matrix4x4::Begin() const
{
  return &arr[0];
}

float * Matrix4x4::End()
{
  return &arr[16];
}

const float * Matrix4x4::End() const
{
  return &arr[16];
}

float & Matrix4x4::operator()(unsigned int c, unsigned int r)
{
  return arr[r * 4 + c];
}

const float & Matrix4x4::operator()(unsigned int c, unsigned int r) const
{
  return arr[r * 4 + c];
}

const float * Matrix4x4::data() const
{
	return &arr[0];
}

float * Matrix4x4::data()
{
	return &arr[0];
}

Vector3 Matrix4x4::GetRow3(unsigned int r) const
{
  return Vector3(
    arr[r],
    arr[r + 4],
    arr[r + 4 * 2]
  );
}

Vector4 Matrix4x4::GetRow4(unsigned int r) const
{
  return Vector4(
    arr[r],
    arr[r + 4],
    arr[r + 8],
    arr[r + 12]
  );
}

Vector3 Matrix4x4::GetCol3(unsigned int c) const
{
  return Vector3(
    arr[c * 4],
    arr[c * 4 + 1],
    arr[c * 4 + 2]
  );
}

Vector4 Matrix4x4::GetCol4(unsigned int c) const
{
  return Vector4(
    arr[c * 4],
    arr[c * 4 + 1],
    arr[c * 4 + 2],
    arr[c * 4 + 3]
  );
}

Matrix4x4 Matrix4x4::Translate(const Vector3 & t)
{
  Matrix4x4 temp;

  temp.arr[12] = t.x;
  temp.arr[13] = t.y;
  temp.arr[14] = t.z;

  return temp;
}

Matrix4x4 Matrix4x4::Rotate(float theta, const Vector4 & u)
{
  float cosine = cosf(theta);
  float sine = sinf(theta);
  float uxsq = u.x * u.x;
  float uysq = u.y * u.y;
  float uzsq = u.z * u.z;
  float uxuy = u.x * u.y;
  float uxuz = u.x * u.z;
  float uyuz = u.y * u.z;

  return Matrix4x4{
    uxsq + cosine*(1 - uxsq),
    uxuy*(1 - cosine) + u.z *sine,
    uxuz*(1 - cosine) - u.y *sine,
    0,
    uxuy*(1 - cosine) - u.z *sine,
    uysq + cosine*(1 - uysq),
    uyuz*(1 - cosine) + u.x *sine,
    0,
    uxuz*(1 - cosine) + u.y *sine,
    uyuz*(1 - cosine) - u.x *sine,
    uzsq + cosine * (1 - uzsq),
    0,
    0,
    0,
    0,
    1
  };
}

Matrix4x4 Matrix4x4::Rotate(const Matrix4x4 & mat, float theta, const Vector3 & u)
{
	return Matrix4x4::Rotate(theta, Vector4(u.x, u.y, u.z, 0.f)) * mat;
}

Vector4 Matrix4x4::Rotate(const Vector4& start, float theta, const Vector3& u)
{
	Vector4 norm_start{ start.Unit() };
	return Vector4{ Rotate(theta, Vector4(u.x, u.y, u.z, 0.f)) * start }.Normalize();
}

Matrix4x4 Matrix4x4::Scale(const Vector3 & scaleVec)
{
  return Matrix4x4{
    scaleVec.x, 0, 0, 0,
    0, scaleVec.y, 0, 0,
    0, 0, scaleVec.z, 0,
    0, 0, 0, 1
  };
}

Matrix4x4 Matrix4x4::LookAt(const Vector3 & eye, const Vector3 & at, const Vector3 & up)
{
  Vector3 w = (eye - at).Unit();
  Vector3 u = (up ^ w).Unit();
  Vector3 v = w ^ u;

  Matrix4x4 viewMat(
    u.x, v.x, w.x, 0,
    u.y, v.y, w.y, 0,
    u.z, v.z, w.z, 0,
    -u * eye, -v * eye, -w * eye, 1
  );

  return viewMat;
}

Matrix4x4 Matrix4x4::LookAtRm(const Vector3 & eye, const Vector3 & at, const Vector3 & up)
{
  Vector3 w = (at - eye).Unit();
  Vector3 u = (up ^ w).Unit();
  Vector3 v = w ^ u;

  Matrix4x4 viewMat(
    u.x, u.y, u.z, -u * eye,
    v.x, v.y, v.z, -v * eye,
    w.x, w.y, w.z, -w * eye,
    0, 0, 0, 1
  );

  return viewMat;
}

Matrix4x4 Matrix4x4::Inverse(const Matrix4x4 & m)
{
	Matrix4x4 inv, invOut;
	double det;
	int i;
	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return Matrix4x4();

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * (float)det;
	return invOut;
}

Matrix4x4 Matrix4x4::Transpose(const Matrix4x4 & arr)
{
	Matrix4x4 returnMatrix(arr[0], arr[4], arr[8], arr[12],
		arr[1], arr[5], arr[9], arr[13],
		arr[2], arr[6], arr[10], arr[14],
		arr[3], arr[7], arr[11], arr[15]);
	return returnMatrix;
}

Matrix4x4 Matrix4x4::AffineInverse(const Matrix4x4 & m)
{
	auto m3 = Matrix3x3(m[0], m[1], m[2], m[4], m[5], m[6],
		m[8], m[9], m[10]);
	auto m3i = m3.Inverse();
	auto translation = Vector3(m[12], m[13], m[14]);
	auto newTrans = -(m3i * translation);

	return Matrix4x4(m[0], m[1], m[2], 0.f, m[3], m[4], m[5],
		0.f, m[6], m[7], m[8], 0.f, newTrans.x, newTrans.y, newTrans.z, 1.f);
}


Matrix4x4 Matrix4x4::RotateX(float theta)
{
	float cosine = cosf(theta);
	float sine = sinf(theta);

	return Matrix4x4{
		1, 0,		0,		0,
		0, cosine,	sine,	0,
		0, -sine,	cosine, 0,
		0, 0,		0,		1
	};
}


Matrix4x4 Matrix4x4::RotateY(float theta)
{
	float cosine = cosf(theta);
	float sine = sinf(theta);

	return Matrix4x4{
		cosine, 0, -sine,	0,
		0,		1, 0,		0,
		sine,	0, cosine,	0,
		0,		0, 0,		1
	};
}

Matrix4x4 Matrix4x4::RotateZ(float theta)
{
	float cosine = cosf(theta);
	float sine = sinf(theta);

	return Matrix4x4{
		cosine,	sine,	0,	0,
		-sine,	cosine,	0,	0,
		0,		0,		1,	0,
		0,		0,		0,	1
	};
}

void Matrix4x4::GetRotationAngle(const Matrix4x4& mat, float & x, float & y, float & z)
{

	//x = -atanf(mat.arr[4 * 1 + 2] / mat.arr[4 * 2 + 2]);
	//y = asinf(mat.arr[4 * 0 + 2]);
	//z = -atanf(mat.arr[4 * 0 + 1] / mat.arr[4 * 0 + 0]);

	//float sqrtfXYZ = sqrtf((mat.arr[4 * 2 + 1] - mat.arr[4 * 1 + 2])*(mat.arr[4 * 2 + 1] - mat.arr[4 * 1 + 2])
	//					+ (mat.arr[4 * 0 + 2] - mat.arr[4 * 2 + 0])*(mat.arr[4 * 0 + 2] - mat.arr[4 * 2 + 0])
	//					+ (mat.arr[4 * 1 + 0] - mat.arr[4 * 0 + 1])*(mat.arr[4 * 1 + 0] - mat.arr[4 * 0 + 1]));
	//
	//float angle = acosf((mat.arr[0] + mat.arr[5] + mat.arr[10] - 1) / 2.0f);
	//
	//std::cout << angle << std::endl;
	//
	//if (sqrtfXYZ != 0 && angle != 0)
	//{
	//	Vector3 tmp
	//	{
	//		(mat.arr[4 * 2 + 1] - mat.arr[4 * 1 + 2]) / sqrtfXYZ,
	//		(mat.arr[4 * 0 + 2] - mat.arr[4 * 2 + 0]) / sqrtfXYZ,
	//		(mat.arr[4 * 1 + 0] - mat.arr[4 * 0 + 1]) / sqrtfXYZ
	//	};
	//	tmp = tmp.Normalize() * angle;
	//	x = tmp.x;
	//	y = tmp.y;
	//	z = tmp.z;
	//
	//	std::cout << tmp << std::endl;
	//}

	float single = sqrtf(mat.arr[0] * mat.arr[0] + mat.arr[1] * mat.arr[1]);
	bool singularTest = single < 1e-6;

	if (!singularTest)
	{
		x = atan2(mat.arr[4 * 1 + 2], mat.arr[4 * 2 + 2]);
		y = atan2(-mat.arr[2], single);
		z = atan2(mat.arr[1], mat.arr[0]);
	}
	else
	{
		x = atan2(-mat.arr[4 * 2 + 1], mat.arr[5]);
		y = atan2(-mat.arr[2], single);
		z = 0;
	}
}


Matrix4x4 Matrix4x4::Ortho(float left, float right, float btm, float top, float zNear, float zFar)
{
	UNREFERENCED_PARAMETER(left);
	UNREFERENCED_PARAMETER(btm);
	Matrix4x4 mm;
	mm[0] = 1.f / right;
	mm[5] = 1.f / top;
	mm[10] = -2.f / (zFar - zNear);
	mm[14] = -(zFar + zNear) / (zFar - zNear);
	mm[15] = 1.f;
	return mm;
}

Matrix4x4 Matrix4x4::Perspective(float angle, float ratio, float nearPlane, float farPlane)
{
	auto const tanHalfFovy = tanf(angle / 2.f);

	Matrix4x4 mm;
	//2n / w
	mm[0] = 1 / tanHalfFovy / ratio;
	mm[5] = 1 / tanHalfFovy;
	mm[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
	mm[11] = -1.f;
	mm[14] = -(farPlane * nearPlane * 2.f) / (farPlane - nearPlane);
	mm[15] = 0;
	return mm;
}

std::ostream& operator<<(std::ostream& os, const Matrix4x4& arr)
{
	os << "( " << arr[0] << ", " << arr[4] << ", " << arr[8] << ", " << arr[12] << ")\n"
		<< "( " << arr[1] << ", " << arr[5] << ", " << arr[9] << ", " << arr[13] << ")\n"
		<< "( " << arr[2] << ", " << arr[6] << ", " << arr[10] << ", " << arr[14] << ")\n"
		<< "( " << arr[3] << ", " << arr[7] << ", " << arr[11] << ", " << arr[15] << ")\n";
	return os;
}