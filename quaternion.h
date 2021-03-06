#ifndef HPP_QUATERNION_H
#define HPP_QUATERNION_H

#include "cast.h"
#include "json.h"
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

	// Constructors
	inline Quaternion(void);
	inline Quaternion(Real x, Real y, Real z, Real w);
	inline Quaternion(Quaternion const& q);
	inline Quaternion(Vector3 const& axis, Angle const& rotation);
	inline Quaternion(Json const& json);

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

	inline Vector3 getXYZ(void) const { return Vector3(x, y, z); }
	inline Vector3 getAxis(void) const;
	inline Angle getAngle(void) const;

	// Conversion functions
	inline std::string toString(void) const;
	inline Json toJson(void) const;

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

inline Quaternion::Quaternion(Vector3 const& axis, Angle const& rotation)
{
	Vector3 axis_nrm = axis.normalized();
	Real s = (rotation / 2).sin();
	Real c = (rotation / 2).cos();

	x = axis_nrm.x * s;
	y = axis_nrm.y * s;
	z = axis_nrm.z * s;
	w = c;
}

inline Quaternion::Quaternion(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::ARRAY) throw Exception("JSON for Quaternion must be an array!");
	if (json.getArraySize() != 4) throw Exception("JSON for Quaternion must contain exactly four numbers!");
	for (size_t num_id = 0; num_id < 4; ++ num_id) {
		if (json.getItem(num_id).getType() != Json::NUMBER) throw Exception("Unexpected non-number in JSON array for Quaternion!");
	}
	// Construct
	x = json.getItem(0).getNumber();
	y = json.getItem(1).getNumber();
	z = json.getItem(2).getNumber();
	w = json.getItem(3).getNumber();
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

inline std::string Quaternion::toString(void) const
{
	return "{" + floatToStr(x) + ", " + floatToStr(y) + ", " + floatToStr(z) + ", " + floatToStr(w) + "}";
}

inline Json Quaternion::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(x));
	result.addItem(Json::newNumber(y));
	result.addItem(Json::newNumber(z));
	result.addItem(Json::newNumber(w));
	return result;
}

inline std::ostream& operator<<(std::ostream& strm, Quaternion const& q)
{
	return strm << q.toString();
}

}

#endif
