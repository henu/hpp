#ifndef HPP_VECTORS_H
#define HPP_VECTORS_H

#include "assert.h"

#include <cmath>
#include <ostream>


namespace Hpp
{



// ----------------------------------------
// ----------------------------------------
//
// Class Vector2
//
// ----------------------------------------
// ----------------------------------------

class Vector2
{

public:

	// ----------------------------------------
	// Public constants
	// ----------------------------------------

/*
	static Vector2 const ZERO;
	static Vector2 const ONE;
	static Vector2 const INFINITE;
*/

	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	// Constructor and destructor
	inline Vector2(void);

	// Miscellaneous constructors
	inline Vector2(float x, float y);

	// Copy constructor and assignment operator
	inline Vector2(Vector2 const& v);
	inline Vector2& operator=(Vector2 const& v);

	// Miscellaneous operators
	inline Vector2 operator+(Vector2 const& v) const;
	inline Vector2 operator-(Vector2 const& v) const;
	inline Vector2 operator+=(Vector2 const& v);
	inline Vector2 operator-=(Vector2 const& v);
	inline Vector2 operator*(float r) const;
	inline Vector2 operator*=(float r);
	inline Vector2 operator/(float r) const;
	inline Vector2 operator/=(float r);
	inline Vector2 operator-(void) const;

	// Function to get length of Vector
	inline float length(void) const;
	inline float lengthSquared(void) const;

	// Function to normalize Vector. The length of vector cannot be zero.
	inline void normalize(void);

	// Returns normalized version of Vector
	inline Vector2 normalized(void) const;


	// ----------------------------------------
	// Public members
	// ----------------------------------------

	float x, y;

};


// ----------------------------------------
// Global functions of Vector2
// ----------------------------------------

inline std::ostream& operator<<(std::ostream& strm, Vector2 const& v);
inline float dotProduct(Vector2 const& v1, Vector2 const& v2);



// ----------------------------------------
// ----------------------------------------
//
// Class Vector3
//
// ----------------------------------------
// ----------------------------------------

class Vector3
{

public:

	// ----------------------------------------
	// Public constants
	// ----------------------------------------

/*
	static Vector3 const ZERO;
	static Vector3 const ONE;
	static Vector3 const INFINITE;
*/

	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	// Constructor and destructor
	inline Vector3(void);

	// Miscellaneous constructors
	inline Vector3(float x, float y, float z);

	// Copy constructor and assignment operator
	inline Vector3(Vector3 const& v);
	inline Vector3& operator=(Vector3 const& v);

	// Comparison operators
	inline bool operator==(Vector3 const& v) const;
	inline bool operator!=(Vector3 const& v) const;

	// Miscellaneous operators
	inline Vector3 operator+(Vector3 const& v) const;
	inline Vector3 operator-(Vector3 const& v) const;
	inline Vector3 operator+=(Vector3 const& v);
	inline Vector3 operator-=(Vector3 const& v);
	inline Vector3 operator*(float r) const;
	inline Vector3 operator*=(float r);
	inline Vector3 operator*(Vector3 const& v) const;
	inline Vector3 operator/(float r) const;
	inline Vector3 operator/=(float r);
	inline Vector3 operator-(void) const;

	// Function to get length of Vector
	inline float length(void) const;
	inline float lengthSquared(void) const;

	// Function to normalize Vector. The length of vector cannot be zero.
	inline void normalize(void);

	// Returns normalized version of Vector
	inline Vector3 normalized(void) const;

	// Checks if vector is finite.
	inline bool isFinite(void) const;


	// ----------------------------------------
	// Public members
	// ----------------------------------------

	float x, y, z;

};


// ----------------------------------------
// Global functions of Vector3
// ----------------------------------------

inline float dotProduct(Vector3 const& v1, Vector3 const& v2);
inline Vector3 crossProduct(Vector3 const& v1, Vector3 const& v2);
inline std::ostream& operator<<(std::ostream& strm, Vector3 const& v);
inline Vector3 operator*(float r, Vector3 const& v);



// ----------------------------------------
// ----------------------------------------
//
// Implemented inline functions of Vector2
//
// ----------------------------------------
// ----------------------------------------


// ----------------------------------------
// Vector2::Vector2(), Vector2::~Vector2()
// ----------------------------------------

Vector2::Vector2(void)
{
}


// ----------------------------------------
// Vector2::Vector2()
// ----------------------------------------

Vector2::Vector2(float x, float y) :
x(x), y(y)
{
}


// ----------------------------------------
// Vector2::Vector2(), Vector::operator=()
// ----------------------------------------

Vector2::Vector2(Vector2 const& v) :
x(v.x), y(v.y)
{
}

Vector2& Vector2::operator=(Vector2 const& v)
{
	x = v.x;
	y = v.y;
	return *this;
}


// ----------------------------------------
// Vector2::operator+()
// ----------------------------------------

Vector2 Vector2::operator+(Vector2 const& v) const
{
	return Vector2(x + v.x, y + v.y);
}


// ----------------------------------------
// Vector2::operator-()
// ----------------------------------------

Vector2 Vector2::operator-(Vector2 const& v) const
{
	return Vector2(x - v.x, y - v.y);
}


// ----------------------------------------
// Vector2::operator+=()
// ----------------------------------------

Vector2 Vector2::operator+=(Vector2 const& v)
{
	x += v.x;
	y += v.y;
	return *this;
}


// ----------------------------------------
// Vector2::operator-=()
// ----------------------------------------

Vector2 Vector2::operator-=(Vector2 const& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}


// ----------------------------------------
// Vector2::operator*()
// ----------------------------------------

Vector2 Vector2::operator*(float r) const
{
	return Vector2(x * r, y * r);
}


// ----------------------------------------
// Vector2::operator*=()
// ----------------------------------------

Vector2 Vector2::operator*=(float r)
{
	x *= r;
	y *= r;
	return *this;
}


// ----------------------------------------
// Vector2::operator/()
// ----------------------------------------

Vector2 Vector2::operator/(float r) const
{
	HppAssert(r != 0, "Division by zero!");
	return Vector2(x / r, y / r);
}


// ----------------------------------------
// Vector2::operator/=()
// ----------------------------------------

Vector2 Vector2::operator/=(float r)
{
	HppAssert(r != 0, "Division by zero!");
	x /= r;
	y /= r;
	return *this;
}


// ----------------------------------------
// Vector2::operator-()
// ----------------------------------------

Vector2 Vector2::operator-(void) const
{
	return Vector2(-x, -y);
}


// ----------------------------------------
// operator<<()
// ----------------------------------------

std::ostream& operator<<(std::ostream& strm, Vector2 const& v)
{
	strm << '(';
	strm << v.x;
	strm << ", ";
	strm << v.y;
	strm << ')';
	return strm;
}



// ----------------------------------------
// Vector2::length()
// ----------------------------------------

inline float Vector2::length(void) const
{
	return static_cast< float >(sqrt(x*x + y*y));
}


// ----------------------------------------
// Vector2::lengthSquared()
// ----------------------------------------

inline float Vector2::lengthSquared(void) const
{
	return static_cast< float >(x*x + y*y);
}


// ----------------------------------------
// Vector2::normalize()
// ----------------------------------------

inline void Vector2::normalize(void)
{
	float len = static_cast< float >(sqrt(x*x + y*y));
	HppAssert(len != 0, "Unable to normalize zero length vector!");
	x /= len;
	y /= len;
}


// ----------------------------------------
// Vector2::normalized()
// ----------------------------------------

inline Vector2 Vector2::normalized(void) const
{
	float len = length();
	HppAssert(len != 0.0, "Cannot normalize zero vector!");
	return Vector2(x / len, y / len);
}


// ----------------------------------------
// dotProduct()
// ----------------------------------------

inline float dotProduct(Vector2 const& v1, Vector2 const& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}



// ----------------------------------------
// ----------------------------------------
//
// Implemented inline functions of Vector3
//
// ----------------------------------------
// ----------------------------------------


// ----------------------------------------
// Vector3::Vector3(), Vector3::~Vector3()
// ----------------------------------------

inline Vector3::Vector3(void)
{
}


// ----------------------------------------
// Vector3::Vector3()
// ----------------------------------------

inline Vector3::Vector3(float x, float y, float z) :
x(x), y(y), z(z)
{
}


// ----------------------------------------
// Vector3::Vector3(), Vector::operator=()
// ----------------------------------------

inline Vector3::Vector3(Vector3 const& v) :
x(v.x), y(v.y), z(v.z)
{
}

inline Vector3& Vector3::operator=(Vector3 const& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


// ----------------------------------------
// Vector3::operator+()
// ----------------------------------------

inline Vector3 Vector3::operator+(Vector3 const& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}


// ----------------------------------------
// Vector3::operator-()
// ----------------------------------------

inline Vector3 Vector3::operator-(Vector3 const& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}


// ----------------------------------------
// Vector3::operator+=()
// ----------------------------------------

inline Vector3 Vector3::operator+=(Vector3 const& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}


// ----------------------------------------
// Vector3::operator-=()
// ----------------------------------------

inline Vector3 Vector3::operator-=(Vector3 const& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}


// ----------------------------------------
// Vector3::operator*()
// ----------------------------------------

inline Vector3 Vector3::operator*(float r) const
{
	return Vector3(x * r, y * r, z * r);
}


// ----------------------------------------
// Vector3::operator*=()
// ----------------------------------------

inline Vector3 Vector3::operator*=(float r)
{
	x *= r;
	y *= r;
	z *= r;
	return *this;
}


// ----------------------------------------
// Vector3::operator/()
// ----------------------------------------

inline Vector3 Vector3::operator/(float r) const
{
	HppAssert(r != 0, "Division by zero!");
	return Vector3(x / r, y / r, z / r);
}


// ----------------------------------------
// Vector3::operator/=()
// ----------------------------------------

inline Vector3 Vector3::operator/=(float r)
{
	HppAssert(r != 0, "Division by zero");
	x /= r;
	y /= r;
	z /= r;
	return *this;
}


// ----------------------------------------
// Vector3::length()
// ----------------------------------------

inline float Vector3::length(void) const
{
	return static_cast< float >(sqrt(x*x + y*y + z*z));
}


// ----------------------------------------
// Vector3::lengthSquared()
// ----------------------------------------

inline float Vector3::lengthSquared(void) const
{
	return static_cast< float >(x*x + y*y + z*z);
}


// ----------------------------------------
// Vector3::normalize()
// ----------------------------------------

inline void Vector3::normalize(void)
{
	float len = static_cast< float >(sqrt(x*x + y*y + z*z));
	HppAssert(len != 0, "Unable to normalize zero length vector!");
	x /= len;
	y /= len;
	z /= len;
}


// ----------------------------------------
// dotProduct()
// ----------------------------------------

inline float dotProduct(Vector3 const& v1, Vector3 const& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


// ----------------------------------------
// crossProduct()
// ----------------------------------------

inline Vector3 crossProduct(Vector3 const& v1, Vector3 const& v2)
{
	return Vector3(v1.y * v2.z - v1.z * v2.y,
	               v1.z * v2.x - v1.x * v2.z,
	               v1.x * v2.y - v1.y * v2.x);
}


// ----------------------------------------
// operator<<()
// ----------------------------------------

inline std::ostream& operator<<(std::ostream& strm, Vector3 const& v)
{
	strm << '(';
	strm << v.x;
	strm << ", ";
	strm << v.y;
	strm << ", ";
	strm << v.z;
	strm << ')';
	return strm;
}


// ----------------------------------------
// operator<<()
// ----------------------------------------

inline Vector3 operator*(float r, Vector3 const& v)
{
	return Vector3(v.x*r, v.y*r, v.z*r);
}


// ----------------------------------------
// Vector3::operator==(),
// Vector3::operator!=()
// ----------------------------------------

inline bool Vector3::operator==(Vector3 const& v) const
{
	if (v.x != x ||
	    v.y != y ||
	    v.z != z) {
		return false;
	}
	return true;
}

inline bool Vector3::operator!=(Vector3 const& v) const
{
	if (v.x != x ||
	    v.y != y ||
	    v.z != z) {
		return true;
	}
	return false;
}


// ----------------------------------------
// Vector3::operator-()
// ----------------------------------------

inline Vector3 Vector3::operator-(void) const
{
	return Vector3(-x, -y, -z);
}


// ----------------------------------------
// Vector3::operator*()
// ----------------------------------------

inline Vector3 Vector3::operator*(Vector3 const& v) const
{
	return Vector3(x * v.x,
	               y * v.y,
	               z * v.z);
}


// ----------------------------------------
// Vector3::normalized()
// ----------------------------------------

inline Vector3 Vector3::normalized(void) const
{
	float len = length();
	HppAssert(len != 0.0, "Unable to normalize zero length vector!");
	return Vector3(x / len, y / len, z / len);
}


// ----------------------------------------
// Vector3::isFinite()
// ----------------------------------------

inline bool Vector3::isFinite(void) const
{
	if (!::finite(x) ||
	    !::finite(y) ||
	    !::finite(z)) {
		return false;
	}
	return true;
}

}

#endif
