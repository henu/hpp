#ifndef HPP_IVECTOR3_H
#define HPP_IVECTOR3_H

#include "cast.h"
#include "json.h"

#include <cmath>
#include <string>
#include <ostream>


namespace Hpp
{

class IVector3
{

public:

	// Constructors
	inline IVector3(void);
	inline IVector3(ssize_t x, ssize_t y, ssize_t z);
	inline IVector3(Json const& json);

	// Copy constructor and assignment operator
	inline IVector3(IVector3 const& v);
	inline IVector3& operator=(IVector3 const& v);

	// Miscellaneous operators
	inline IVector3 operator+(IVector3 const& v) const;
	inline IVector3 operator-(IVector3 const& v) const;
	inline IVector3 operator+=(IVector3 const& v);
	inline IVector3 operator-=(IVector3 const& v);
	inline IVector3 operator*(ssize_t i) const;
	inline IVector3 operator*=(ssize_t i);
	inline IVector3 operator/(ssize_t i) const;
	inline IVector3 operator/=(ssize_t i);
	inline IVector3 operator-(void) const;

	// Conversion functions
	inline std::string toString(void) const;
	inline Json toJson(void) const;

	// For comparison
	inline bool operator==(IVector3 const& v) const;
	inline bool operator!=(IVector3 const& v) const;

	// For sorting
	inline bool operator<(IVector3 const& v) const;

	ssize_t x, y, z;

};


// ----------------------------------------
// Global functions of IVector3
// ----------------------------------------

inline std::ostream& operator<<(std::ostream& strm, IVector3 const& v);
inline ssize_t dotProduct(IVector3 const& v1, IVector3 const& v2);



// ----------------------------------------
// ----------------------------------------
//
// Implemented inline functions of IVector3
//
// ----------------------------------------
// ----------------------------------------


inline IVector3::IVector3(void)
{
}

inline IVector3::IVector3(ssize_t x, ssize_t y, ssize_t z) :
x(x), y(y), z(z)
{
}

inline IVector3::IVector3(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::ARRAY) throw Exception("JSON for IVector3 must be an array!");
	if (json.getArraySize() != 3) throw Exception("JSON for IVector3 must contain exactly two numbers!");
	for (size_t num_id = 0; num_id < 3; ++ num_id) {
		if (json.getItem(num_id).getType() != Json::NUMBER) throw Exception("Unexpected non-number in JSON array for IVector3!");
	}
	// Construct
	x = json.getItem(0).getInteger();
	y = json.getItem(1).getInteger();
	z = json.getItem(2).getInteger();
}

inline IVector3::IVector3(IVector3 const& v) :
x(v.x), y(v.y), z(v.z)
{
}

inline IVector3& IVector3::operator=(IVector3 const& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

inline IVector3 IVector3::operator+(IVector3 const& v) const
{
	return IVector3(x + v.x, y + v.y, z + v.z);
}

inline IVector3 IVector3::operator-(IVector3 const& v) const
{
	return IVector3(x - v.x, y - v.y, z - v.z);
}

inline IVector3 IVector3::operator+=(IVector3 const& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline IVector3 IVector3::operator-=(IVector3 const& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline IVector3 IVector3::operator*(ssize_t i) const
{
	return IVector3(x * i, y * i, z * i);
}

inline IVector3 IVector3::operator*=(ssize_t i)
{
	x *= i;
	y *= i;
	z *= i;
	return *this;
}

inline IVector3 IVector3::operator/(ssize_t i) const
{
	HppAssert(i != 0, "Division by zero!");
	return IVector3(x / i, y / i, z / i);
}

inline IVector3 IVector3::operator/=(ssize_t i)
{
	HppAssert(i != 0, "Division by zero!");
	x /= i;
	y /= i;
	z /= i;
	return *this;
}


inline IVector3 IVector3::operator-(void) const
{
	return IVector3(-x, -y, -z);
}

inline std::string IVector3::toString(void) const
{
	return "(" + ssizeToStr(x) + ", " + ssizeToStr(y) + ", " + ssizeToStr(z) + ")";
}

inline Json IVector3::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(x));
	result.addItem(Json::newNumber(y));
	result.addItem(Json::newNumber(z));
	return result;
}

inline bool IVector3::operator==(IVector3 const& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

inline bool IVector3::operator!=(IVector3 const& v) const
{
	return x != v.x || y != v.y || z != v.z;
}

inline bool IVector3::operator<(IVector3 const& v) const
{
	if (z < v.z) return true;
	if (z > v.z) return false;
	if (y < v.y) return true;
	if (y > v.y) return false;
	if (x < v.x) return true;
	return false;
}

inline std::ostream& operator<<(std::ostream& strm, IVector3 const& v)
{
	return strm << v.toString();
}

}

#endif

