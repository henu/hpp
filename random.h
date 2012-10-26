#ifndef RANDOM_H
#define RANDOM_H

#include "color.h"
#include "vector2.h"
#include "vector3.h"
#include "angle.h"
#include "assert.h"
#include "real.h"
#include "bytev.h"
#include "unicodestring.h"

#include <cstdlib>
#include <fstream>
#include <algorithm>

namespace Hpp
{

// Returns a value in range [min, max)
inline Real random(Real min, Real max);

// Returns a value in range [min, max]
inline ssize_t randomInt(ssize_t min, ssize_t max);

// Returns a value in range [0, 2^n)
inline size_t randomNBitInt(size_t n);

// Returns random angle between [-180 - 180) degrees.
inline Angle randomAngle(void);

inline Vector2 randomVector2(Real max_radius);
inline Vector3 randomVector3(Real max_radius);

inline Color randomColor(bool randomize_alpha = false);

// Returns data from secure random source
inline ByteV randomSecureData(size_t size);

inline UnicodeString randomString(size_t size, UnicodeString const& chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_., ");

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

inline size_t randomNBitInt(size_t n)
{
	size_t result;
	HppAssert(n > 1, "n must be at least 2!");

	size_t rand_max_bits;
	if (RAND_MAX == 0x7fffffff) {
		rand_max_bits = 31;
	} else {
		rand_max_bits = 15;
	}

	size_t bits_left = n;
	result = 0;
	while (bits_left > 0) {
		size_t bits_get = std::min(rand_max_bits, bits_left);
		bits_left -= bits_get;
		result <<= bits_get;
		result += rand() % (1 << bits_get);
	}

	return result;
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

inline Color randomColor(bool randomize_alpha)
{
	Color result;
	if (randomize_alpha) {
		result = Color(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX));
	} else {
		result = Color(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX));
	}
	return result;
}

inline ByteV randomSecureData(size_t size)
{
	ByteV result;

	#ifndef WIN32

	std::ifstream rnd("/dev/random");
	if (!rnd.is_open()) {
		throw Exception("Unable to open /dev/random!");
	}

	result.assign(size, 0);
	rnd.read((char*)&result[0], size);

	rnd.close();

	#else

	HppAssert(false, "Not implemented for windows yet!");

	#endif

	return result;
}

inline UnicodeString randomString(size_t size, UnicodeString const& chars)
{
	HppAssert(!chars.empty(), "No chars given!");
	UnicodeString result;
	result.reserve(size);
	while (result.size() < size) {
		result += chars[randomInt(0, chars.size() - 1)];
	}
	return result;
}

}

#endif
