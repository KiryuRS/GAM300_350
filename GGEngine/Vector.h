#pragma once

class Vector2
{
	friend  Vector2 operator+(const Vector2& l, const Vector2& r);
	friend  Vector2 operator-(const Vector2& l, const Vector2& r);
	friend  Vector2 operator*(float l, const Vector2& r);           /*  scalar product */
	friend  Vector2 operator*(const Vector2& r, float l);           /*  scalar product */
	friend  float   operator*(const Vector2& l, const Vector2& r);  /*  dot product */

public:
	Vector2();
	Vector2(float xi, float yi);
	Vector2(int xi, int yi);
	Vector2(const Vector2& v);
	Vector2(const float(&cv)[2]);

	Vector2& operator=(const Vector2& v);
	float&          operator[](unsigned int i);
	const float&    operator[](unsigned int i) const;
	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(float s);
	Vector2& operator/=(float s);
	Vector2 operator-(void) const;
	Vector2 operator/(float s) const;
	bool operator==(const Vector2 & rhs) const
	{
		return std::tie(x, y) == std::tie(rhs.x, rhs.y);
	}

	bool operator!=(const Vector2 & rhs) const
	{
		return !operator==(rhs);
	}
	bool operator==(const float(&rhs)[2]) const
	{
		return std::tie(x, y) == std::tie(rhs[0], rhs[1]);
	}
	bool operator!=(const float(&rhs)[2]) const
	{
		return !operator==(rhs);
	}


	float Length() const;
	Vector2 Unit() const;
	Vector2& Normalize();
	float GetX() const { return x; }
	float GetY() const { return y; }
	void SetX(float value) { x = value; }
	void SetY(float value) { y = value; }
	float x;
	float y;
};

class Vector3
{
	friend  Vector3 operator+(const Vector3& l, const Vector3& r);
	friend  Vector3 operator-(const Vector3& l, const Vector3& r);
	friend  Vector3 operator*(float l, const Vector3& r);           /*  scalar product */
	friend  Vector3 operator*(const Vector3& r, float l);           /*  scalar product */
	friend  Vector3 operator^(const Vector3& l, const Vector3& r);  /*  cross product */
	friend  float   operator*(const Vector3& l, const Vector3& r);  /*  dot product */
	friend  Vector3 operator/(const Vector3& l, float r); 

public:
	Vector3();
	Vector3(float xi, float yi, float zi);
	Vector3(Vector2, float zi = 0);
	Vector3(const Vector3& v);
	Vector3(const float(&cv)[3]);

	Vector3& operator=(const Vector3& v);
	float&          operator[](unsigned int i);
	const float&    operator[](unsigned int i) const;
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
	Vector3 operator-(void) const;
	bool operator==(const Vector3 & rhs) const
	{
		return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
	}

	bool operator!=(const Vector3 & rhs) const
	{
		return !operator==(rhs);
	}
	bool operator==(const float(&rhs)[3]) const
	{
		return std::tie(x, y, z) == std::tie(rhs[0], rhs[1], rhs[2]);
	}
	bool operator!=(const float(&rhs)[3]) const
	{
		return !operator==(rhs);
	}
	// FMOD_VECTOR conversion
	operator FMOD_VECTOR()
	{
		return FMOD_VECTOR{ x, y, z };
	}

	float Length() const;
	float LengthSq() const{ return x * x + y * y + z * z; }
	Vector3 Unit() const;
	Vector3& Normalize();
	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetZ() const { return z; }
	void SetX(float value) { x = value; }
	void SetY(float value) { y = value; }
	void SetZ(float value) { z = value; }
	float x;
	float y;
	float z;
	Vector3 Cross(const Vector3& vv) const
	{
		return Vector3{y * vv.z - z * vv.y,
		vv.x * z  - x * vv.z,
		x * vv.y - y * vv.x};
	}
};


class Vector4
{
	friend Vector4 operator+(const Vector4& l, const Vector4& r);
	friend Vector4 operator-(const Vector4& l, const Vector4& r);
	friend Vector4 operator*(float l, const Vector4& r);            /*  scalar product */
	friend Vector4 operator*(const Vector4& r, float);             /*  scalar product */
	friend float   operator*(const Vector4& l, const Vector4& r);   /*  dot product */

public:
	Vector4();
	Vector4(float xi, float yi, float zi, float wi);
	Vector4(const Vector3& v, float wi);
	Vector4(const Vector4& v);
	Vector4(const float(&cv)[4]);

	Vector4& operator=(const Vector4& v);
	float&          operator[](unsigned int i);
	const float&    operator[](unsigned int i) const;
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
	Vector4 operator-(void) const;
	Vector4 operator/(float s);
	//operator Color() const;
	bool operator==(const Vector4& rhs) const;
	bool operator!=(const Vector4& rhs) const;
	bool operator==(const float(&rhs)[4]) const
	{
		return std::tie(x, y, z) == std::tie(rhs[0], rhs[1], rhs[2]);
	}
	bool operator!=(const float(&rhs)[4]) const
	{
		return !operator==(rhs);
	}

	float Length() const;
	Vector4 Unit() const;
	Vector4& Normalize();
	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetZ() const { return z; }
	float GetW() const { return w; }
	void SetX(float value) { x = value; }
	void SetY(float value) { y = value; }
	void SetZ(float value) { z = value; }
	void SetW(float value) { w = value; }
	float x;
	float y;
	float z;
	float w;
	static Vector4 QuaternionInterpolate(Vector4, Vector4, float);
  static Vector4 QuaternionEuler(const Vector3 &);
};

// Printout Functions
std::ostream& operator<<(std::ostream &os, const Vector2& vv);
std::ostream& operator<<(std::ostream &os, const Vector3& vv);
std::ostream& operator<<(std::ostream &os, const Vector4& vv);

//Rotate

Vector4 AxisAngleToQuaternion(float angle, const Vector3& axis);
Vector4 LookAt(Vector3 sourcePoint, Vector3 destPoint);
Vector3 Rotate(float angle, const Vector3& axis, const Vector3& curr);
Vector3 EulerQuaternion(const Vector4& quat);
