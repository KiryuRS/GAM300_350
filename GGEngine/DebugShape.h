#pragma once
#include "Vector.h"
#include <vector>
#include <array>

struct LineSegment
{
	Vector3 start, end;
	LineSegment():start(0,0,0),
		end(0,0,0)
	{}

	LineSegment(const Vector3& s, const Vector3& e) 
		:start(s), end(e) 
	{}
};

struct DebugShape
{
	bool onTop = false;
	std::vector<LineSegment> segments;
	Vector4 color = {1.f, 0.f, 0.f, 1.f};
};


struct Ray
{
	Vector3 start, direction;
};
struct Sphere
{
	Vector3 point;
	float radius;
};
struct Aabb
{
	Vector3 min, max;
};

struct Triangle
{
	std::array<Vector3, 3> points;
};
struct Plane
{
	Vector4 pp;
	Vector3 GetNormal() const { return Vector3(pp.x, pp.y, pp.z); }
	float GetDistance() const { return pp.w; }
};
struct Frustum
{
	std::array<Vector3, 8> points;

};