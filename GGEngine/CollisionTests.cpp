#include "stdafx.h"
#include "CollisionTests.h"
#include <array>


Vector3 ProjectPointOnPlane(const Vector3& point, const Vector3& normal, float planeDistance)
{
	/******Student:Assignment1******/
	return point - normal * (point* normal - planeDistance);
}

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b,
	float& u, float& v, float epsilon)
{
	/******Student:Assignment1******/
	auto n = b - a;
	if (n.LengthSq() == 0.f)
	{
		v = 0.f;
		u = 1.f;
		return false;
	}
	n.Normalize();
	auto nDp = n *(point - a);
	v = nDp / (b - a).Length();
	u = 1.f - v;
	if (v < -epsilon || v > 1.f + epsilon) return false;
	auto diff = point - (a + nDp * n);
	if (diff.Length() > epsilon) return false;
	return true;
}

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b, const Vector3& c,
	float& u, float& v, float& w, float epsilon)
{
	/******Student:Assignment1******/
	Vector3 v0 = point - c;
	Vector3 v1 = a - c;
	Vector3 v2 = b - c;
	float _a = v1 * (v1);
	float _b = v2 * (v1);
	float _c = v1 * (v2);
	float _d = v2 * (v2);
	float _e = v0 * (v1);
	float _f = v0 * (v2);
	float detBottom = _a * _d - _c * _b;
	if (detBottom == 0.f)
	{
		u = 0.f;
		v = 0.f;
		w = 0.f;
		return false;
	}

	u = (_e * _d - _b * _f) / detBottom;
	v = (_a * _f - _c * _e) / detBottom;
	w = 1 - u - v;
	auto checker = [=](float f)
	{
		if (f < -epsilon || f > 1.f + epsilon)
			return false;
		return true;
	};
	if (checker(u) && checker(v) && checker(w)) return true;
	return false;
}

IntersectionType::Type PointPlane(const Vector3& point, const Vector4& plane, float epsilon)
{
	auto result = point.x * plane.x + point.y * plane.y + point.z * plane.z - plane.w;
	if (result < -epsilon) return IntersectionType::Outside;
	else if (result > epsilon) return IntersectionType::Inside;
	else return IntersectionType::Coplanar;
}

bool PointSphere(const Vector3& point, const Vector3& sphereCenter, float sphereRadius)
{
	auto pointMinusSphere = point - sphereCenter;
	if (pointMinusSphere.LengthSq() <= sphereRadius * sphereRadius)
		return true;
	else
		return false;
}

bool PointAabb(const Vector3& point, const Vector3& aabbMin, const Vector3& aabbMax)
{
	auto compare = [](float pt, float min, float max)
	{
		return (min <= pt) && (max >= pt);
	};
	return compare(point.x, aabbMin.x, aabbMax.x) &&
		compare(point.y, aabbMin.y, aabbMax.y) &&
		compare(point.z, aabbMin.z, aabbMax.z);
}

bool RayPlane(const Vector3& rayStart, const Vector3& rayDir,
	const Vector4& plane, float& t, float epsilon)
{
	auto normal = Vector3{ plane.x, plane.y, plane.z };
	auto dotProd = normal * (rayDir);
	if (abs(dotProd) < epsilon) return false;
	t = (plane.w - normal * (rayStart)) / (normal * (rayDir));
	if (t < 0)
		return false;
	else
		return true;
}

bool RayTriangle(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float& t, float triExpansionEpsilon)
{
	auto v1 = triP1 - triP0;
	auto v2 = triP2 - triP1;
	auto normal = v1.Cross(v2);
	normal.Normalize();
	float dist = normal * (triP1);
	bool rayPlane = RayPlane(rayStart, rayDir,
		Vector4(normal.x, normal.y, normal.z, dist), t, triExpansionEpsilon);
	if (!rayPlane) return false;
	auto point = rayStart + rayDir * t;
	float u, v, w;
	return BarycentricCoordinates(point, triP0, triP1, triP2, u, v, w, triExpansionEpsilon);
}

bool RaySphere(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& sphereCenter, float sphereRadius,
	float& t)
{
	float a = rayDir * (rayDir);
	float b = 2.f * (rayStart * (rayDir) - sphereCenter * (rayDir));
	float c = rayStart * (rayStart) + sphereCenter * (sphereCenter) -
		sphereRadius * sphereRadius - 2 * sphereCenter * (rayStart);
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0.f)
		return false;
	else if (discriminant > 0.f)
	{
		float t0 = (-b - sqrt(discriminant)) / 2.f / a;
		float t1 = (-b + sqrt(discriminant)) / 2.f / a;
		if (t1 < 0.f) return false;
		if (t0 < 0.f)
			t = 0.f;
		else
			t = t0;
		return true;
	}
	else // == 0
	{
		t = -b / 2.f / a;
		return true;
	}
}

bool RayAabb(const Vector3& rayStart, const Vector3& rayDir,
	const Vector3& aabbMin, const Vector3& aabbMax, float& t)
{
	std::vector<float> minValues, maxValues;
	auto checkAndEmplace = [&](float aMin, float aMax, float b, float c)
	{
		if (c > 0.f)
		{
			minValues.emplace_back((aMin - b) / c);
			maxValues.emplace_back((aMax - b) / c);
		}
		else if (c < 0.f)
		{
			maxValues.emplace_back((aMin - b) / c);
			minValues.emplace_back((aMax - b) / c);
		}
		else if (b < aMin || b > aMax)
			return false;
		return true;
	};
	if (!checkAndEmplace(aabbMin.x, aabbMax.x, rayStart.x, rayDir.x)) return false;
	if (!checkAndEmplace(aabbMin.y, aabbMax.y, rayStart.y, rayDir.y)) return false;
	if (!checkAndEmplace(aabbMin.z, aabbMax.z, rayStart.z, rayDir.z)) return false;

	auto maxFunc = [](const std::vector<float>& vec)
	{
		float max = std::numeric_limits<float>::lowest();
		for (auto item : vec)
		{
			if (max < item) max = item;
		}
		return max;
	};
	auto minFunc = [](const std::vector<float>& vec)
	{
		float min = std::numeric_limits<float>::max();
		for (auto item : vec)
		{
			if (min > item) min = item;
		}
		return min;
	};
	float tMin = maxFunc(minValues);
	float tMax = minFunc(maxValues);
	if (tMin > tMax) return false;
	if (tMax < 0.f) return false;
	if (tMin < 0.f)
		t = 0.f;
	else
		t = tMin;
	return true;
}

IntersectionType::Type PlaneTriangle(const Vector4& plane,
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float epsilon)
{
	auto type0 = PointPlane(triP0, plane, epsilon);
	auto type1 = PointPlane(triP1, plane, epsilon);
	auto type2 = PointPlane(triP2, plane, epsilon);
	auto allTypesEqual = [&](IntersectionType::Type type)
	{
		if ((type0 == type || type0 == IntersectionType::Coplanar)
			&& (type1 == type || type1 == IntersectionType::Coplanar)
			&& (type2 == type || type2 == IntersectionType::Coplanar))
			return true;
		return false;
	};
	if (allTypesEqual(IntersectionType::Coplanar))
		return IntersectionType::Coplanar;
	if (allTypesEqual(IntersectionType::Inside))
		return IntersectionType::Inside;
	if (allTypesEqual(IntersectionType::Outside))
		return IntersectionType::Outside;
	return IntersectionType::Overlaps;
}

IntersectionType::Type PlaneSphere(const Vector4& plane,
	const Vector3& sphereCenter, float sphereRadius)
{
	auto planeNormal = Vector3(plane.x, plane.y, plane.z);
	auto p0 = ProjectPointOnPlane(sphereCenter, planeNormal, plane.w);
	auto scMp0 = sphereCenter - p0;
	if (scMp0.LengthSq() <= sphereRadius * sphereRadius)
		return IntersectionType::Overlaps;
	if (scMp0 * (planeNormal) > 0) return IntersectionType::Inside;
	else return IntersectionType::Outside;
}

IntersectionType::Type PlaneAabb(const Vector4& plane,
	const Vector3& aabbMin, const Vector3& aabbMax)
{
	auto center = (aabbMin + aabbMax) / 2.f;
	auto extend = aabbMax - center;
	Vector3 u;
	u.x = plane.x >= 0.f ? extend.x : -extend.x;
	u.y = plane.y >= 0.f ? extend.y : -extend.y;
	u.z = plane.z >= 0.f ? extend.z : -extend.z;
	auto d = Vector3(plane.x, plane.y, plane.z) * (u);
	return PlaneSphere(plane, center, d);
}

IntersectionType::Type FrustumTriangle(const Vector4 planes[6],
	const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
	float epsilon)
{
	std::vector<IntersectionType::Type> types;
	for (unsigned i = 0; i < 6; ++i)
		types.emplace_back(PlaneTriangle(planes[i], triP0, triP1, triP2, epsilon));
	bool allInside = true;
	for (auto item : types)
	{
		if (item == IntersectionType::Outside)
			return IntersectionType::Outside;
		if (item != IntersectionType::Inside)
			allInside = false;
	}
	if (allInside)
		return IntersectionType::Inside;
	else
		return IntersectionType::Overlaps;
}

IntersectionType::Type FrustumSphere(const Vector4 planes[6],
	const Vector3& sphereCenter, float sphereRadius, size_t& lastAxis)
{
	bool overlaps = false;
	for (unsigned i = 0; i < 6; ++i)
	{
		auto type = PlaneSphere(planes[i], sphereCenter, sphereRadius);
		if (type == IntersectionType::Outside)
			return IntersectionType::Outside;
		if (type == IntersectionType::Overlaps) overlaps = true;
	}
	if (overlaps) return IntersectionType::Overlaps;
	return IntersectionType::Inside;
}

IntersectionType::Type FrustumAabb(const Vector4 planes[6],
	const Vector3& aabbMin, const Vector3& aabbMax, size_t& lastAxis)
{
	std::array<int, 6> axis{ 0,1,2,3,4,5 };
	std::swap(axis[0], axis[lastAxis]);
	bool overlaps = false;
	for (size_t i = 0; i < 6; ++i)
	{
		auto type = PlaneAabb(planes[axis[i]], aabbMin, aabbMax);
		if (type == IntersectionType::Outside)
		{
			lastAxis = i;
			return IntersectionType::Outside;
		}
		if (type == IntersectionType::Overlaps)
		{
			overlaps = true;
		}
	}
	if (overlaps) return IntersectionType::Overlaps;
	return IntersectionType::Inside;
}

bool SphereSphere(const Vector3& sphereCenter0, float sphereRadius0,
	const Vector3& sphereCenter1, float sphereRadius1)
{
	auto dist = sphereCenter0 - sphereCenter1;
	auto radSum = sphereRadius0 + sphereRadius1;
	return dist.LengthSq() <= radSum * radSum;
}

bool AabbAabb(const Vector3& aabbMin0, const Vector3& aabbMax0,
	const Vector3& aabbMin1, const Vector3& aabbMax1)
{
	auto comp = [](float aMin, float aMax, float bMin, float bMax)
	{
		return aMax >= bMin && bMax >= aMin;
	};
	bool intersecting = comp(aabbMin0.x, aabbMax0.x, aabbMin1.x, aabbMax1.x) &&
		comp(aabbMin0.y, aabbMax0.y, aabbMin1.y, aabbMax1.y) &&
		comp(aabbMin0.z, aabbMax0.z, aabbMin1.z, aabbMax1.z);
	if (intersecting) return true;

	auto lessThan = [](const Vector3&lhs, const Vector3& rhs)
	{
		if (lhs.x <= rhs.x && lhs.y <= rhs.y &&
			lhs.y <= rhs.y) return true;
		return false;
	};
	if (lessThan(aabbMin0, aabbMin1) && lessThan(aabbMax1, aabbMax0)) return true;
	if (lessThan(aabbMin1, aabbMin0) && lessThan(aabbMax0, aabbMax1)) return true;
	return false;
}


bool AabbAabb(const Aabb& aabb1, const Aabb& aabb2)
{
	return AabbAabb(aabb1.min, aabb1.max, aabb2.min, aabb2.max);
}
