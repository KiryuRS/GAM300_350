#pragma once
#include <math.h>
#include "Vector.h"
/*
Trivially, this file will include all (if not, most) of the mathemtics calculation
that would be required for the engine to run.
Do note that vector and matrix are supposed to be classes that will be in
the SAME folder but not in this file
*/

#ifndef M_PI
#define M_PI							3.14159265358979323846f
#endif

#define CONVERT_DEG_TO_RAD(deg)			deg / 180.f * M_PI
#define CONVERT_RAD_TO_DEG(rad)			rad / M_PI * 180.f

namespace MATH
{
	inline float cosf_d(float degree)				{ return cosf(CONVERT_DEG_TO_RAD(degree)); }
	inline float sinf_d(float degree)				{ return sinf(CONVERT_DEG_TO_RAD(degree)); }
	inline float tanf_d(float degree)				{ return tanf(CONVERT_DEG_TO_RAD(degree)); }
	inline int ffloor(float value)					{ return (int)floor(value); }
	inline int fceil(float value)					{ return (int)ceil(value); }

	/*
		Linear Interpolation.
		Ensure that time is within [0, 1]
		pos0 is the "starting" position
		pos1 is the "ending" position

		T can be any of the following:
		- float
		- Vector2
		- Vector3
		- Vector4
	*/
	template <typename T, typename = std::enable_if_t< std::is_same<T, float>::value || std::is_same<T, Vector2>::value || std::is_same<T, Vector3>::value || std::is_same<T, Vector4>::value >>
	T Lerp(const T& pos0, const T& pos1, float time)
	{
		return pos0 * (1.f - time) + pos1 * time;
	}

	/*
		Clamps the value given within min to max (if its out of bounds)
	*/
	template <typename T>
	inline T Clamp(const T& minimum, const T& maximum, const T& value)
	{
		T _value = min(value, maximum);
		return max(_value, minimum);
	}

	/*
		Wraps input value to the range[min, max]
	*/
	template <typename T>
	inline T Wrap(const T& value, const T& _min, const T& _max)
	{
		_max -= _min;
		value = std::fmod(value - _min, _max);
		if (value < T{ 0 })
			value += _max;
		return _min + value;
	}

	template <typename T, auto N, typename = std::enable_if_t< std::is_same<T, Vector2>::value || std::is_same<T, Vector3>::value || std::is_same<T, Vector4>::value >>
	void VectorToFloat(const T& vector, float(&c_vector)[N])
	{
		for (auto i = 0; i != N; ++i)
			c_vector[i] = vector[i];
	}

	template <typename T, size_t N, typename = std::enable_if_t< std::is_same<T, Vector2>::value || std::is_same<T, Vector3>::value || std::is_same<T, Vector4>::value >>
	void FloatToVector(const float (&c_vector)[N], T& vector)
	{
		for (size_t i = 0; i != N; ++i)
			vector[i] = c_vector[i];
	}

	// Performing FMOD_VECTOR - FMOD_VECTOR
	FMOD_VECTOR fvSubtract(const FMOD_VECTOR& lhs, const FMOD_VECTOR& rhs);
	// Performing FMOD_VECTOR + FMOD_VECTOR
	FMOD_VECTOR fvAdd(const FMOD_VECTOR& lhs, const FMOD_VECTOR& rhs);
	// Performing FMOD_VECTOR / FMOD_VECTOR
	FMOD_VECTOR fvDivideVec(const FMOD_VECTOR& lhs, const FMOD_VECTOR& rhs);
	// Performing FMOD_VECTOR / value
	FMOD_VECTOR fvDivideValue(const FMOD_VECTOR& lhs, float val);
	// Performing FMOD_VECTOR * FMOD_VECTOR (Dot Product)
	float fvDotProd(const FMOD_VECTOR& lhs, const FMOD_VECTOR& rhs);
}