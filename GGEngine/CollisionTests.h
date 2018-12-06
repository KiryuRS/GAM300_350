
#pragma once

#include "Vector.h"
namespace IntersectionType
{
	enum Type { Coplanar = 0, Outside, Inside, Overlaps, NotImplemented };
	static const char* Names[] = { "Coplanar", "Outside", "Inside", "Overlaps", "NotImplemented" };
}

Vector3 ProjectPointOnPlane(const Vector3& point, const Vector3& normal, float planeDistance);

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b,
	float& u, float& v, float epsilon = 0.0f);

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b, const Vector3& c,
	float& u, float& v, float& w, float epsilon = 0.0f);

IntersectionType::Type PointPlane(const Vector3& point, const Vector4& plane, float epsilon);

bool PointSphere(const Vector3& point, const Vector3& sphereCenter, float sphereRadius);

bool PointAabb(const Vector3& point, const Vector3& aabbMin, const Vector3& aabbMax);

bool RayPlane(const Vector3& rayStart, const Vector3& rayDir,
	const Vector4& plane, float& t, float epsilon = 0.0001f);
bool RayTriangle(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float& t, float triExpansionEpsilon);

bool RaySphere(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& sphereCenter, float sphereRadius,
	float& t);

bool RayAabb(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& aabbMin, const Vector3& aabbMax, float& t);

IntersectionType::Type PlaneTriangle(const Vector4& plane,
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float epsilon);

IntersectionType::Type PlaneSphere(const Vector4& plane,
	const Vector3& sphereCenter, float sphereRadius);

IntersectionType::Type PlaneAabb(const Vector4& plane,
	const Vector3& aabbMin, const Vector3& aabbMax);
IntersectionType::Type FrustumTriangle(const Vector4 planes[6],
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float epsilon);
IntersectionType::Type FrustumSphere(const Vector4 planes[6],
	const Vector3& sphereCenter, float sphereRadius, size_t& lastAxis);

IntersectionType::Type FrustumAabb(const Vector4 planes[6],
	const Vector3& aabbMin, const Vector3& aabbMax, size_t& lastAxis);
bool SphereSphere(const Vector3& sphereCenter0, float sphereRadius0,
	const Vector3& sphereCenter1, float sphereRadius1);

bool AabbAabb(const Vector3& aabbMin0, const Vector3& aabbMax0,
	const Vector3& aabbMin1, const Vector3& aabbMax1);
bool AabbAabb(const Aabb& aabbMin0, const Aabb& aabbMax0);
