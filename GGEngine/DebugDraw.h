#pragma once
#include "DebugShape.h"
#include<mutex>


class DebugDraw
{
	std::vector<DebugShape> shapes;
	std::mutex shapes_m;
public:
	inline DebugShape& GetNewShape()
	{
		std::lock_guard<std::mutex> m(shapes_m);
		return shapes.emplace_back();
	}
	DebugShape& DrawPoint(const Vector3& point);
	DebugShape& DrawLine(const LineSegment& line);
	DebugShape& DrawRay(const Ray& ray, float t);
	DebugShape& DrawSphere(const Sphere& sphere);
	DebugShape& DrawAabb(const Aabb& aabb);
	DebugShape& DrawTriangle(const Triangle& triangle);
	DebugShape& DrawPlane(const Plane& plane, float sizeX, float sizeY);
	DebugShape& DrawQuad(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);
	DebugShape& DrawFrustum(const Frustum& frustum);
	DebugShape& AddDisk(DebugShape&, Vector3 center, Vector3 normal, float radius);
	void Draw();
	void Clear();
};