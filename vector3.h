#ifndef HPP_VECTOR3_H
#define HPP_VECTOR3_H

#include "real.h"
#include "assert.h"

#include <cmath>
#include <ostream>

namespace Hpp
{

class Vector3
{

public:

	static Vector3 const ONE;
	static Vector3 const ZERO;
	static Vector3 const X;
	static Vector3 const Y;
	static Vector3 const Z;

	inline Vector3(void);
	inline Vector3(Real x, Real y, Real z);

	// Miscellaneous functions
	inline Real length(void) const;
	inline Real lengthTo2(void) const;
	inline void normalize(void);
	inline Vector3 normalized(void) const;

	// Operators between Vector3s
	inline Vector3 operator-(void) const;
	inline Vector3 operator+(Vector3 const& v) const;
	inline Vector3 operator-(Vector3 const& v) const;
	inline Vector3 operator*(Vector3 const& v) const;
	inline Vector3 operator+=(Vector3 const& v);
	inline Vector3 operator-=(Vector3 const& v);
	inline Vector3 operator*=(Vector3 const& v);

	// Operators with other types
	inline Vector3 operator*(Real r) const;
	inline Vector3 operator/(Real r) const;
	inline Vector3 operator*=(Real r);
	inline Vector3 operator/=(Real r);

	// Comparison operator for sorting
	inline bool operator<(Vector3 const& v) const;

	// Returns vector that is perpendicular (that is, 90 degrees) to this
	// one. The length of resulting vector might be smaller than this one,
	// but not greater.
	inline Vector3 perp(void) const;

	Real x, y, z;

};

inline std::ostream& operator<<(std::ostream& strm, Vector3 const& v);

inline Vector3 operator*(Real f, Vector3 const& v);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Vector3::Vector3(void)
{
}

inline Vector3::Vector3(Real x, Real y, Real z) :
x(x), y(y), z(z)
{
}

inline Real Vector3::length(void) const
{
	return sqrt(x*x + y*y + z*z);
}

inline Real Vector3::lengthTo2(void) const
{
	return x*x + y*y + z*z;
}

inline void Vector3::normalize(void)
{
	Real len = length();
	HppAssert(len != 0.0, "Division by zero!");
	x /= len;
	y /= len;
	z /= len;
}

inline Vector3 Vector3::normalized(void) const
{
	Vector3 result;
	Real len = length();
	HppAssert(len != 0.0, "Division by zero!");
	result.x = x / len;
	result.y = y / len;
	result.z = z / len;
	return result;
}

inline Vector3 Vector3::operator-(void) const
{
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(Vector3 const& v) const
{
	return Vector3(x+v.x, y+v.y, z+v.z);
}

inline Vector3 Vector3::operator-(Vector3 const& v) const
{
	return Vector3(x-v.x, y-v.y, z-v.z);
}

inline Vector3 Vector3::operator*(Vector3 const& v) const
{
	return Vector3(x*v.x, y*v.y, z*v.z);
}

inline Vector3 Vector3::operator+=(Vector3 const& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline Vector3 Vector3::operator-=(Vector3 const& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline Vector3 Vector3::operator*=(Vector3 const& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

inline Vector3 Vector3::operator*(Real r) const
{
	return Vector3(x*r, y*r, z*r);
}

inline Vector3 Vector3::operator/(Real r) const
{
	HppAssert(r != 0.0, "Division by zero!");
	return Vector3(x/r, y/r, z/r);
}

inline Vector3 Vector3::operator*=(Real r)
{
	x *= r;
	y *= r;
	z *= r;
	return *this;
}

inline Vector3 Vector3::operator/=(Real r)
{
	HppAssert(r != 0.0, "Division by zero!");
	x /= r;
	y /= r;
	z /= r;
	return *this;
}

inline bool Vector3::operator<(Vector3 const& v) const
{
	if (x < v.x) return true;
	if (x > v.x) return false;
	if (y < v.y) return true;
	if (y > v.y) return false;
	if (z < v.z) return true;
	return false;
}

inline Vector3 Vector3::perp(void) const
{
	if (fabs(x) < fabs(y)) {
		return Vector3(0, z, -y);
	} else {
		return Vector3(-z, 0, x);
	}
}

inline std::ostream& operator<<(std::ostream& strm, Vector3 const& v)
{
	strm << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return strm;
}

inline Vector3 operator*(Real f, Vector3 const& v)
{
	return Vector3(f * v.x, f * v.y, f * v.z);
}

}

#endif
