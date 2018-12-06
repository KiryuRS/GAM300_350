#include "stdafx.h"
#include "Vector.h"

Vector2::Vector2()
	: x{ 0.0f }, y{ 0.0f }
{ }

Vector2::Vector2(float xi, float yi)
	: x{ xi }, y{ yi }
{ }

Vector2::Vector2(int xi, int yi)
	: x{ (float)xi }, y{ (float)yi }
{ }

Vector2::Vector2(const Vector2 & v)
	: x{ v.x }, y{ v.y }
{ }

Vector2::Vector2(const float(&cv)[2])
	: x{ cv[0] }, y{ cv[1] } 
{ }

Vector2 & Vector2::operator=(const Vector2 & v)
{
	x = v.x;
	y = v.y;
	return *this;
}

float & Vector2::operator[](unsigned int i)
{
	switch (i)
	{
	case 0:
		return x;
	}
	return y;
}

const float & Vector2::operator[](unsigned int i) const
{
	switch (i)
	{
	case 0:
		return x;
	}
	return y;
}

Vector2 & Vector2::operator+=(const Vector2 & v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2 & Vector2::operator-=(const Vector2 & v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2 & Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2 & Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

Vector2 Vector2::operator-(void) const
{
	return Vector2{ -x, -y };
}

Vector2 Vector2::operator/(float s) const
{
  return Vector2(x / s, y / s);
}

float Vector2::Length() const
{
	return sqrtf(x*x + y*y);
}

Vector2 Vector2::Unit() const
{
	return Vector2{ x,y } /= Length();
}

Vector2 & Vector2::Normalize()
{
	if (Length() == 0.f)
		TOOLS::CreateErrorMessageBox("Length cannot be 0 when normalizing!");

	return *this /= Length();
}

Vector2 operator+(const Vector2 & l, const Vector2 & r)
{
	return Vector2(l.x + r.x, l.y + r.y);
}

Vector2 operator-(const Vector2 & l, const Vector2 & r)
{
	return Vector2(l.x - r.x, l.y - r.y);
}

Vector2 operator*(float l, const Vector2 & r)
{
	return Vector2(l * r.x, l * r.y);
}

Vector2 operator*(const Vector2 & r, float l)
{
  return l * r;
}

Vector3::Vector3()
	: x{ 0.0f }, y{ 0.0f }, z{ 0.0f }
{ }

Vector3::Vector3(float xi, float yi, float zi)
	: x{ xi }, y{ yi }, z{ zi }
{ }

Vector3::Vector3(Vector2 vv, float zi)
	: x{ vv.x }, y{ vv.y }, z{ zi }
{ }

Vector3::Vector3(const Vector3& v)
	: x{ v.x }, y{ v.y }, z{ v.z }
{ }

Vector3::Vector3(const float(&cv)[3])
	: x{ cv[0] }, y{ cv[1] }, z{ cv[2] }
{ }

Vector3& Vector3::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

float& Vector3::operator[](unsigned int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	}
	return z;
}

const float& Vector3::operator[](unsigned int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	}
	return z;
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vector3 Vector3::operator-() const
{
	Vector3 vv = *this;
	vv *= -1;
	return vv;
}

float Vector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}


Vector3 Vector3::Unit() const
{
	auto len = Length();
	// @todo: deteremine the reason why the len is 0
	if (!len)
		return *this;
	// TOOLS::Assert(len != 0.f, "Length cannot be 0 when normalizing!");
	Vector3 vv = *this;
	vv /= len;
	return vv;
}

Vector3& Vector3::Normalize()
{
	auto len = Length();
	// @todo: deteremine the reason why the len is 0
	if (!len)
		return *this;
	// TOOLS::Assert(len != 0.f, "Length cannot be 0 when normalizing!");
	*this /= len;
	return *this;
}

float operator*(const Vector2 & l, const Vector2 & r)
{
	return l.x * r.x + l.y + r.y;
}

Vector3 operator+(const Vector3& l, const Vector3& r)
{
	return Vector3(l.x + r.x, l.y + r.y, l.z + r.z);
}

Vector3 operator-(const Vector3& l, const Vector3& r)
{
	return Vector3(l.x - r.x, l.y - r.y, l.z - r.z);
}

Vector3 operator*(float l, const Vector3& r)
{
	return Vector3(l * r.x, l * r.y, l * r.z);
}

Vector3 operator*(const Vector3 & r, float l)
{
  return l * r;
}

float operator*(const Vector3& l, const Vector3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

Vector3 operator/(const Vector3 & l, float r)
{
  return Vector3(l.x / r, l.y / r, l.z / r);
}

Vector3 operator^(const Vector3& l, const Vector3& r)
{
	Vector3 vv;
	vv.x = l.y * r.z - l.z * r.y;
	vv.y = l.z * r.x - l.x * r.z;
	vv.z = l.x * r.y - l.y * r.x;
	return vv;
}

Vector4::Vector4() : x{ 0 }, y{ 0 }, z{ 0 }, w{ 1 }
{

}

Vector4::Vector4(float xi, float yi, float zi, float wi)
	: x{ xi }, y{ yi }, z{ zi }, w{ wi }
{

}

Vector4::Vector4(const Vector3 & v, float wi)
  : x{ v.x }, y{ v.y }, z{ v.z }, w{ wi }
{
}

Vector4::Vector4(const Vector4& v)
	: x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w }
{

}

Vector4::Vector4(const float(&cv)[4])
	: x{ cv[0] }, y{ cv[1] }, z{ cv[2] }, w{ cv[3] }
{ }

Vector4& Vector4::operator=(const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

float& Vector4::operator[](unsigned int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}
	return w;
}

const float& Vector4::operator[](unsigned int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}
	return w;
}

Vector4& Vector4::operator+=(const Vector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vector4& Vector4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Vector4& Vector4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

Vector4 Vector4:: operator/(float s)
{
	Vector4 vv = *this;
	vv /= s;
	return vv;
}

Vector4 Vector4::operator-() const
{
	Vector4 vv = *this;
	vv *= -1;
	return vv;
}

float Vector4::Length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

Vector4 Vector4::Unit() const
{
	auto len = Length();
	Vector4 vv = *this;
	return vv /= len;
}

Vector4& Vector4::Normalize()
{
	if (Length() == 0.f)
		TOOLS::CreateErrorMessageBox("Length cannot be 0 when normalizing!");

	auto len = Length();
	return (*this) /= len;
}

Vector4 Vector4::QuaternionInterpolate(Vector4 pStart, Vector4 pEnd, float pFactor)
{
	// calc cosine theta
	float cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

	// adjust signs (if necessary)
	Vector4 end = pEnd;
	if (cosom < static_cast<float>(0.0f))
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	// Calculate coefficients
	float sclp, sclq;
	if ((static_cast<float>(1.0f) - cosom) > static_cast<float>(0.0001f)) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float omega, sinom;
		omega = std::acosf(cosom); // extract theta from dot product's cos theta
		sinom = std::sinf(omega);
		sclp = std::sinf((1.f - pFactor) * omega) / sinom;
		sclq = std::sinf(pFactor * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = 1.f - pFactor;
		sclq = pFactor;
	}
	Vector4 pOut;
	pOut.x = sclp * pStart.x + sclq * end.x;
	pOut.y = sclp * pStart.y + sclq * end.y;
	pOut.z = sclp * pStart.z + sclq * end.z;
	pOut.w = sclp * pStart.w + sclq * end.w;
	return pOut;
}

Vector4 Vector4::QuaternionEuler(const Vector3 & rot)
{
  float cr = std::cosf(rot.x * 0.5f);
  float sr = std::sinf(rot.x * 0.5f);

  float cy = std::cosf(rot.z * 0.5f);
  float sy = std::sinf(rot.z * 0.5f);

  float cp = std::cosf(rot.y * 0.5f);
  float sp = std::sinf(rot.y * 0.5f);



  return Vector4{ cy * sr * cp - sy * cr * sp,
                  cy * cr * sp + sy * sr * cp,
                  sy * cr * cp - cy * sr * sp,
                  cy * cr * cp + sy * sr * sp }.Normalize();

}


Vector4 operator+(const Vector4& l, const Vector4& r)
{
	return Vector4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
}


Vector4 operator-(const Vector4& l, const Vector4& r)
{
	return Vector4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);
}

Vector4 operator*(float l, const Vector4& r)
{
	return Vector4(l * r.x, l * r.y, l * r.z, l * r.w);
}
Vector4 operator*(const Vector4& r, float l)
{
	return Vector4(l * r.x, l * r.y, l * r.z, l * r.w);
}


float operator*(const Vector4& l, const Vector4& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

//Vector4::operator Color() const
//{
//	return Color(char(x * 255), char(y * 255), char(z * 255), char(w * 255));
//}

bool Vector4::operator==(const Vector4 & rhs) const
{
	return std::tie(x, y, z, w) == std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}

bool Vector4::operator!=(const Vector4 & rhs) const
{
	return !operator==(rhs);
}

std::ostream & operator<<(std::ostream & os, const Vector2 & vv)
{
	return os << "x=" << vv.x << " y=" << vv.y;
}

std::ostream & operator<<(std::ostream &os, const Vector3& vv)
{
	return os << "x=" << vv.x << " y=" << vv.y << " z=" << vv.z;
}

std::ostream & operator<<(std::ostream & os, const Vector4 & vv)
{
	return os << "x=" << vv.x << " y=" << vv.y << " z=" << vv.z << " w=" << vv.w;
}

Vector4 LookAt(Vector3 sourcePoint, Vector3 destPoint)
{
	Vector3 forwardVector = destPoint - sourcePoint;
	forwardVector.Normalize();

	float dot = Vector3(0, 0, 1) * forwardVector;

	if (abs(dot - (-1.0f)) < 0.000001f)
	{
		return Vector4(0, 1, 0, 3.1415926535897932f);
	}
	if (abs(dot - (1.0f)) < 0.000001f)
	{
		return AxisAngleToQuaternion(0.f, Vector3(1,0,0));
	}

	float rotAngle = acos(dot);
	Vector3 rotAxis = Vector3(0, 0, 1).Cross(forwardVector);
	rotAxis.Normalize();
	return AxisAngleToQuaternion(rotAngle, rotAxis);
}

Vector4 AxisAngleToQuaternion(float angle, const Vector3 & axis)
{
	float s = sinf(angle / 2);
	Vector4 quat;
	quat.x = axis.x * s;
	quat.y = axis.y * s;
	quat.z = axis.z * s;
	quat.w = cosf(angle / 2);
	return quat;
}

Vector3 Rotate(float angle, const Vector3& axis, const Vector3& curr)
{
	auto axisCopy = axis;
	axisCopy.Normalize();
	auto quat = AxisAngleToQuaternion(angle, axisCopy);
	Vector3 quatVec{ quat.x, quat.y, quat.z };
	auto scalar = quat.w;
	return 2.f * quatVec * curr * quatVec
		+ (scalar * scalar - quatVec * quatVec) * curr
		+ 2.0f * scalar * quatVec.Cross(curr);
}

Vector3 EulerQuaternion(const Vector4 & quat)
{
	auto x = atan2(quat.x * quat.z + quat.y * quat.w, quat.x * quat.w - quat.y * quat.z);
	auto y = acos(-quat.x* quat.x - quat.y * quat.y + quat.z* quat.z + quat.w * quat.w);
	auto z = atan2(quat.x * quat.z - quat.y * quat.w, quat.y * quat.z + quat.x * quat.z);
	if (x == x && y == y && z == z)
		return Vector3(x, y, z);
	else
		return Vector3();
}
