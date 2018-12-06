#include "stdafx.h"
#include "DebugDraw.h"

DebugShape& DebugDraw::DrawPoint(const Vector3& point)
{
	return DrawSphere(Sphere{ point, 0.1f });
}

DebugShape& DebugDraw::DrawLine(const LineSegment& line)
{
	DebugShape& shape = GetNewShape();
	shape.segments.emplace_back(line.start, line.end);
	return shape;
}

DebugShape& DebugDraw::AddDisk(DebugShape& shape, Vector3 center,
	Vector3 normal, float radius)
{
	Vector3 u, v, w;
	u = normal;
	u.Normalize();
	if (normal.x == 1.f)
		v = normal.Cross(Vector3(0, 1.f, 0));
	else
		v = normal.Cross(Vector3(1.f, 0, 0));
	v.Normalize();
	w = u.Cross(v);

	static constexpr int nusegments = 30;
	float theta = 2.f * M_PI / nusegments;
	float currTheta = theta;
	auto calcNewPoint = [&]()
	{
		return center + radius * (sin(currTheta) * w + cos(currTheta) * v);
	};
	for (int i = 0; i < nusegments; ++i)
	{
		Vector3 p0 = calcNewPoint();
		currTheta += theta;
		Vector3 p1 = calcNewPoint();
		shape.segments.emplace_back(p0, p1);
	}
	return shape;
}

void DebugDraw::Draw()
{
	{
		std::lock_guard<std::mutex> m(shapes_m);
		for (const auto& item : shapes)
		{
			if (item.onTop)
			{
				for (auto ss : item.segments)
				{
					BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWTOPLINE, LineDetails{ss.start, ss.end, item.color} });
				}
			}
			else
			{
				for (auto ss : item.segments)
				{
					BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWLINE, LineDetails{ss.start, ss.end, item.color} });
				}
			}
		}
	}
	shapes.clear();
}

void DebugDraw::Clear()
{
	shapes.clear();
}

DebugShape& DebugDraw::DrawRay(const Ray& ray, float t)
{
	DebugShape& shape = GetNewShape();
	auto lineEnd = ray.start + ray.direction * t;
	shape.segments.emplace_back(ray.start, lineEnd);
	//arrow head
	auto v = ray.direction.Cross(Vector3(1.f, 0, 0));
	if (ray.direction.x == 1.f)
		v = ray.direction.Cross(Vector3(0, 1.f, 0));
	auto w = ray.direction.Cross(v);

	float radius = 0.2f;
	auto center = ray.start + ray.direction * t * 0.9f;
	static constexpr int nusegments = 10;
	float theta = 2.f * M_PI / nusegments;
	float currTheta = theta;
	auto calcNewPoint = [&]()
	{
		return center + radius * (sin(currTheta) * w + cos(currTheta) * v);
	};
	for (int i = 0; i < nusegments; ++i)
	{
		Vector3 p0 = calcNewPoint();
		currTheta += theta;
		shape.segments.emplace_back(p0, lineEnd);
	}
	AddDisk(shape, center, ray.direction, radius);
	return shape;
}

DebugShape& DebugDraw::DrawSphere(const Sphere& sphere)
{
	DebugShape& shape = GetNewShape();
	AddDisk(shape, sphere.point, Vector3(1.f, 0, 0), sphere.radius);
	AddDisk(shape, sphere.point, Vector3(0, 1.f, 0), sphere.radius);
	AddDisk(shape, sphere.point, Vector3(0, 0, 1.f), sphere.radius);

	//horizon disk
	auto camPos = GFX_S.GetCameraPosition();
	auto eMCs = sphere.point - camPos;
	float d = eMCs.Length();
	auto l = sqrtf(d*d - sphere.radius * sphere.radius);
	eMCs.Normalize();
	float radius = sphere.radius * l / d;
	float z = sqrtf(sphere.radius * sphere.radius - radius * radius);
	auto center = sphere.point - z * eMCs;
	AddDisk(shape, center, -eMCs, radius);
	return shape;
}

DebugShape& DebugDraw::DrawAabb(const Aabb& aabb)
{
	DebugShape& shape = GetNewShape();
	shape.segments.emplace_back(aabb.min, Vector3(aabb.max.x, aabb.min.y, aabb.min.z));
	shape.segments.emplace_back(aabb.min, Vector3(aabb.min.x, aabb.max.y, aabb.min.z));
	shape.segments.emplace_back(aabb.min, Vector3(aabb.min.x, aabb.min.y, aabb.max.z));
	shape.segments.emplace_back(aabb.max, Vector3(aabb.min.x, aabb.max.y, aabb.max.z));
	shape.segments.emplace_back(aabb.max, Vector3(aabb.max.x, aabb.min.y, aabb.max.z));
	shape.segments.emplace_back(aabb.max, Vector3(aabb.max.x, aabb.max.y, aabb.min.z));
	shape.segments.emplace_back(Vector3(aabb.max.x, aabb.max.y, aabb.min.z),
		Vector3(aabb.max.x, aabb.min.y, aabb.min.z));
	shape.segments.emplace_back(Vector3(aabb.max.x, aabb.min.y, aabb.max.z),
		Vector3(aabb.max.x, aabb.min.y, aabb.min.z));
	shape.segments.emplace_back(Vector3(aabb.min.x, aabb.max.y, aabb.max.z),
		Vector3(aabb.min.x, aabb.max.y, aabb.min.z));
	shape.segments.emplace_back(Vector3(aabb.max.x, aabb.max.y, aabb.min.z),
		Vector3(aabb.min.x, aabb.max.y, aabb.min.z));
	shape.segments.emplace_back(Vector3(aabb.max.x, aabb.min.y, aabb.max.z),
		Vector3(aabb.min.x, aabb.min.y, aabb.max.z));
	shape.segments.emplace_back(Vector3(aabb.min.x, aabb.max.y, aabb.max.z),
		Vector3(aabb.min.x, aabb.min.y, aabb.max.z));
	return shape;
}

DebugShape& DebugDraw::DrawTriangle(const Triangle& triangle)
{
	DebugShape& shape = GetNewShape();
	shape.segments.emplace_back(triangle.points[0], triangle.points[1]);
	shape.segments.emplace_back(triangle.points[1], triangle.points[2]);
	shape.segments.emplace_back(triangle.points[2], triangle.points[0]);
	return shape;
}

DebugShape& DebugDraw::DrawPlane(const Plane& plane, float sizeX, float sizeY)
{
	auto normal = plane.GetNormal();
	auto center = plane.GetDistance() * plane.GetNormal();
	auto u = normal.Cross(Vector3(1, 0, 0));
	if (normal.x == 1.f)
		u = normal.Cross(Vector3(0, 1, 0));
	auto v = normal.Cross(u);
	DebugShape& shape = DrawRay(Ray{ center, normal }, 2.f);
	auto p0 = center + u * sizeX / 2.f;
	auto p1 = center + v * sizeY / 2.f;
	auto p2 = center - u * sizeX / 2.f;
	auto p3 = center - v * sizeY / 2.f;
	shape.segments.emplace_back(p0, p1);
	shape.segments.emplace_back(p1, p2);
	shape.segments.emplace_back(p2, p3);
	shape.segments.emplace_back(p3, p0);
	return shape;
}

DebugShape& DebugDraw::DrawQuad(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	DebugShape& shape = GetNewShape();
	shape.segments.emplace_back(p0, p1);
	shape.segments.emplace_back(p1, p2);
	shape.segments.emplace_back(p2, p3);
	shape.segments.emplace_back(p3, p0);
	return shape;
}

DebugShape& DebugDraw::DrawFrustum(const Frustum& frustum)
{
	DebugShape& shape = GetNewShape();
	shape.segments.emplace_back(frustum.points[0], frustum.points[1]);
	shape.segments.emplace_back(frustum.points[1], frustum.points[2]);
	shape.segments.emplace_back(frustum.points[2], frustum.points[3]);
	shape.segments.emplace_back(frustum.points[3], frustum.points[0]);

	shape.segments.emplace_back(frustum.points[4], frustum.points[5]);
	shape.segments.emplace_back(frustum.points[5], frustum.points[6]);
	shape.segments.emplace_back(frustum.points[6], frustum.points[7]);
	shape.segments.emplace_back(frustum.points[7], frustum.points[4]);

	shape.segments.emplace_back(frustum.points[0], frustum.points[4]);
	shape.segments.emplace_back(frustum.points[1], frustum.points[5]);
	shape.segments.emplace_back(frustum.points[2], frustum.points[6]);
	shape.segments.emplace_back(frustum.points[3], frustum.points[7]);
	return shape;
}
