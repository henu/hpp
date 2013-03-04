#ifndef HPP_ANGLE_H
#define HPP_ANGLE_H

#include "constants.h"
#include "assert.h"
#include "real.h"

#include <string>
#include <cmath>

namespace Hpp
{

class Angle
{

public:

	static Angle const ZERO;

	inline Angle(void);
	inline Angle(float degrees);

	// Getters
	inline float getRadians(void) const;
	inline float getDegrees(void) const;

	// Conversion functions
	std::string toRadiansStr(void) const;
	std::string toDegreesStr(void) const;

	// Constructing function from radians
	inline static Angle fromRadians(float radians);

	// Copy constructor and assignment operators
	inline Angle(Angle const& a);
	inline Angle operator=(Angle const& a);

	// Comparison operators
	inline bool operator==(Angle const& a) const;
	inline bool operator!=(Angle const& a) const;
	inline bool operator<(Angle const& a) const;
	inline bool operator>(Angle const& a) const;
	inline bool operator<=(Angle const& a) const;
	inline bool operator>=(Angle const& a) const;

	// Operators with another Angle objects
	inline Angle operator+(Angle const& a) const;
	inline Angle operator-(Angle const& a) const;
	inline Angle operator+=(Angle const& a);
	inline Angle operator-=(Angle const& a);
	inline float operator/(Angle const& a) const;

	// Operators with other types
	inline Angle operator/(float f) const;
	inline Angle operator*(float f) const;
	inline Angle operator/=(float f);
	inline Angle operator*=(float f);

	// Other operators
	inline Angle operator-(void) const;

	// Trigonometric functions
	inline Real sin(void) const;
	inline Real cos(void) const;
	inline Real tan(void) const;

private:

	float rad;

};

inline Angle operator*(float f, Angle const& a);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Angle::Angle(void)
{
}

inline Angle::Angle(float degrees) :
rad(degrees / 180.0 * HPP_PI)
{
}

inline float Angle::getRadians(void) const
{
	return rad;
}

inline float Angle::getDegrees(void) const
{
	return rad / HPP_PI * 180.0;
}

inline Angle::Angle(Angle const& a) :
rad(a.rad)
{
}

inline Angle Angle::operator=(Angle const& a)
{
	rad = a.rad;
	return *this;
}

inline Angle Angle::fromRadians(float radians)
{
	Angle a;
	a.rad = radians;
	return a;
}

inline bool Angle::operator==(Angle const& a) const
{
	return rad == a.rad;
}

inline bool Angle::operator!=(Angle const& a) const
{
	return rad != a.rad;
}

inline bool Angle::operator<(Angle const& a) const
{
	return rad < a.rad;
}

inline bool Angle::operator>(Angle const& a) const
{
	return rad > a.rad;
}

inline bool Angle::operator<=(Angle const& a) const
{
	return rad <= a.rad;
}

inline bool Angle::operator>=(Angle const& a) const
{
	return rad >= a.rad;
}

inline Angle Angle::operator+(Angle const& a) const
{
	Angle result;
	result.rad = rad + a.rad;
	return result;
}

inline Angle Angle::operator-(Angle const& a) const
{
	Angle result;
	result.rad = rad - a.rad;
	return result;
}

inline Angle Angle::operator+=(Angle const& a)
{
	rad += a.rad;
	return *this;
}

inline Angle Angle::operator-=(Angle const& a)
{
	rad -= a.rad;
	return *this;
}

inline float Angle::operator/(Angle const& a) const
{
	HppAssert(a.rad != 0.0, "Division by zero!");
	return rad / a.rad;
}

inline Angle Angle::operator/(float f) const
{
	HppAssert(f != 0.0, "Division by zero!");
	Angle result;
	result.rad = rad / f;
	return result;
}

inline Angle Angle::operator*(float f) const
{
	Angle result;
	result.rad = rad * f;
	return result;
}

inline Angle Angle::operator/=(float f)
{
	HppAssert(f != 0.0, "Division by zero!");
	rad /= f;
	return *this;
}

inline Angle Angle::operator*=(float f)
{
	rad *= f;
	return *this;
}

inline Angle Angle::operator-(void) const
{
	Angle result;
	result.rad = -rad;
	return result;
}

inline Real Angle::sin(void) const
{
	return ::sin(rad);
}

inline Real Angle::cos(void) const
{
	return ::cos(rad);
}

inline Real Angle::tan(void) const
{
	return ::tan(rad);
}

inline Angle operator*(float f, Angle const& a)
{
	return a * f;
}

}

#endif
