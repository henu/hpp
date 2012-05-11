#ifndef RANDOM_H
#define RANDOM_H

#include "vector2.h"
#include "vector3.h"
#include "angle.h"
#include "assert.h"
#include "real.h"

#include <cstdlib>

namespace Hpp
{

// Returns a value in range [min, max)
inline Real random(Real min, Real max);

// Returns a value in range [min, max]
inline ssize_t randomInt(ssize_t min, ssize_t max);

inline Angle randomAngle(void);

inline Vector2 randomVector2(Real max_radius);
inline Vector3 randomVector3(Real max_radius);

inline Real random(Real min, Real max)
{
	HppAssert(max >= min, "Maximum cannot be smaller than minimum");
	return min + ((max - min) * (Real)rand() / (Real)RAND_MAX);
}

inline ssize_t randomInt(ssize_t min, ssize_t max)
{
	ssize_t range = max - min + 1;
	return min + (rand() % range);
}

inline Angle randomAngle(void)
{
	return Angle::fromDegrees(random(-180, 180));
}

inline Vector2 randomVector2(Real max_radius)
{
	Vector2 result;
	Real const max_radius_to_2 = max_radius * max_radius;
	do {
		result.x = random(-max_radius, max_radius);
		result.y = random(-max_radius, max_radius);
	} while (result.lengthTo2() > max_radius_to_2);
	return result;
}

inline Vector3 randomVector3(Real max_radius)
{
	Vector3 result;
	Real const max_radius_to_2 = max_radius * max_radius;
	do {
		result.x = random(-max_radius, max_radius);
		result.y = random(-max_radius, max_radius);
		result.z = random(-max_radius, max_radius);
	} while (result.lengthTo2() > max_radius_to_2);
	return result;
}

}

#endif
