#ifndef HPP_QUATERNION_H
#define HPP_QUATERNION_H

#include "real.h"
#include "vector3.h"
#include "angle.h"

#include <cmath>
#include <ostream>


namespace Hpp
{

class Quaternion
{

public:

	// Constructors and destructor
	inline Quaternion(void);
	inline Quaternion(Real x, Real y, Real z, Real w);
	inline Quaternion(Quaternion const& q);

	// Assigment operator
	inline Quaternion operator=(Quaternion const& q);

	// Miscellaneous operators
	inline Quaternion operator+=(Quaternion const& q);
	inline Quaternion operator*(Quaternion const& q) const;
	inline Quaternion operator+(Quaternion const& q) const;
	inline Quaternion operator*(Real r) const;

	// Normalizing functions
	inline Quaternion normalized(void) const;
	inline void normalize(void);

	// Conjugate and inverse
	inline Quaternion conjugate(void) const;
	inline Quaternion inverse(void) const;

	inline Vector3 getXYZ(void) const { return Hpp::Vector3(x, y, z); }
	inline Vector3 getAxis(void) const;
	inline Angle getAngle(void) const;

	// Some constant quaternions
	static const Quaternion IDENTITY;
	static const Quaternion ZERO;

	Real x, y, z, w;

private:

};

inline Quaternion::Quaternion(void)
{
}

inline Quaternion::Quaternion(Real x, Real y, Real z, Real w) :
x(x),
y(y),
z(z),
w(w)
{
}

inline Quaternion::Quaternion(Quaternion const& q) :
x(q.x),
y(q.y),
z(q.z),
w(q.w)
{
}

inline Quaternion Quaternion::operator=(Quaternion const& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
}

inline Quaternion Quaternion::operator+=(Quaternion const& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

inline Quaternion Quaternion::operator*(Quaternion const& q) const
{
	return Quaternion(w*q.x + x*q.w + y*q.z - z*q.y,
	                  w*q.y - x*q.z + y*q.w + z*q.x,
	                  w*q.z + x*q.y - y*q.x + z*q.w,
	                  w*q.w - x*q.x - y*q.y - z*q.z);
}

inline Quaternion Quaternion::operator+(Quaternion const& q) const
{
	return Quaternion(x+q.x, y+q.y, z+q.z, w+q.w);
}

inline Quaternion Quaternion::operator*(Real r) const
{
	return Quaternion(x*r, y*r, z*r, w*r);
}

inline Quaternion Quaternion::normalized(void) const
{
	Real len = sqrt(x*x + y*y + z*z + w*w);
	if (len > 0.0) {
		return Quaternion(x / len,
		                  y / len,
		                  z / len,
		                  w / len);
	}
	return IDENTITY;
}

inline void Quaternion::normalize(void)
{
	Real len = sqrt(x*x + y*y + z*z + w*w);
	if (len > 0.0) {
		x /= len;
		y /= len;
		z /= len;
		w /= len;
		return;
	}
	x = IDENTITY.x;
	y = IDENTITY.y;
	z = IDENTITY.z;
	w = IDENTITY.w;
}

inline Quaternion Quaternion::conjugate(void) const
{
	return Quaternion(-x, -y, -z, w);
}

inline Quaternion Quaternion::inverse(void) const
{
	Quaternion conj = conjugate();
	Real div = x*x + y*y + z*z + w*w;
	HppAssert(div != 0.0, "Division by zero!");
	return Quaternion(conj.x / div, conj.y / div, conj.z / div, conj.w / div);
}

inline Vector3 Quaternion::getAxis(void) const
{
	Real scale = sqrt(x*x + y*y + z*z);
	HppAssert(scale != 0, "Division by zero!");
	Vector3 result;
	result.x = x / scale;
	result.y = y / scale;
	result.z = z / scale;
	return result;
}

inline Angle Quaternion::getAngle(void) const
{
	return Angle::fromRadians(::acos(w) * 2);
}

inline std::ostream& operator<<(std::ostream& strm, Quaternion const& q)
{
	strm << '{' << q.x << ", " << q.y << ", " << q.z << ", " << q.w << '}';
	return strm;
}

}

#endif
