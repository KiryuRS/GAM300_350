#include "stdafx.h"
#include "Math.h"

namespace
{
	inline bool FMODVECTORISNAN(const FMOD_VECTOR& value)
	{
		return std::isnan(value.x) || std::isnan(value.y) || std::isnan(value.z);
	}

	inline bool FMODVECTORISINF(const FMOD_VECTOR& value)
	{
		return std::isinf(value.x) || std::isinf(value.y) || std::isinf(value.z);
	}
}

FMOD_VECTOR MATH::fvSubtract(const FMOD_VECTOR & lhs, const FMOD_VECTOR & rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

FMOD_VECTOR MATH::fvAdd(const FMOD_VECTOR & lhs, const FMOD_VECTOR & rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

FMOD_VECTOR MATH::fvDivideVec(const FMOD_VECTOR & lhs, const FMOD_VECTOR & rhs)
{
	// Double check if the audio is nan (or infinite)
	FMOD_VECTOR rhsModified{ };
	rhsModified.x = abs(rhs.x);
	rhsModified.y = abs(rhs.y);
	rhsModified.y = abs(rhs.z);
	if (FMODVECTORISINF(rhsModified))
		return FMOD_VECTOR{ };
	if (FMODVECTORISNAN(rhsModified))
		return FMOD_VECTOR{ };

	return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}

FMOD_VECTOR MATH::fvDivideValue(const FMOD_VECTOR & lhs, float val)
{
	return !val ? FMOD_VECTOR{0, 0, 0} : FMOD_VECTOR{ lhs.x / val, lhs.y / val, lhs.z / val };
}

float MATH::fvDotProd(const FMOD_VECTOR & lhs, const FMOD_VECTOR & rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
