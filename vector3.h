#ifndef HPP_VECTOR3_H
#define HPP_VECTOR3_H

#include "cast.h"
#include "json.h"
#include "real.h"
#include "assert.h"
#include "serializable.h"
#include "deserializable.h"

#include <cmath>
#include <ostream>
#include <string>
#include <vector>

namespace Hpp
{

class Vector3 : public Serializable, public Deserializable
{

public:

	typedef std::vector< Vector3 > Vec;

	static Vector3 const ONE;
	static Vector3 const ZERO;
	static Vector3 const X;
	static Vector3 const Y;
	static Vector3 const Z;

	inline Vector3(void);
	inline Vector3(Real x, Real y, Real z);
	inline Vector3(std::string const& str);
	inline Vector3(Json const& json);

	// Miscellaneous functions
	inline Real length(void) const;
	inline Real lengthTo2(void) const;
	inline void normalize(void);
	inline Vector3 normalized(void) const;

	// Conversion functions
	inline std::string toString(void) const;

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

	// Comparison operators
	inline bool operator==(Vector3 const& v) const;
	inline bool operator!=(Vector3 const& v) const;

	// Comparison operator for sorting
	inline bool operator<(Vector3 const& v) const;

	// Returns vector that is perpendicular (that is, 90 degrees) to this
	// one. The length of resulting vector might be smaller than this one,
	// but not greater.
	inline Vector3 perp(void) const;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual Json toJson(void) const;
	inline virtual void constructFromJson(Json const& json);

	Real x, y, z;

private:

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result, bool bigendian) const;
	inline virtual void doDeserialize(std::istream& strm, bool bigendian);

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

inline Vector3::Vector3(std::string const& str)
{
	// Strip possible parenthesis around numeric values
	std::string::size_type v_begin = str.find_first_not_of("([{ \t\n");
	if (v_begin == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector3 because it does not contain any numerals!");
	}
	std::string::size_type v_end = str.find_last_not_of(")]} \t\n");
	if (v_end == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector3 because it does not contain any numerals!");
	}
	v_end ++;
	// Find commas and do conversion
	std::string::size_type first_comma = str.find(',', v_begin);
	if (first_comma == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector3 because first comma is not found!");
	}
	std::string::size_type second_comma = str.find(',', first_comma + 1);
	if (second_comma == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector3 because second comma is not found!");
	}
	x = strToFloat(str.substr(v_begin, first_comma));
	y = strToFloat(str.substr(first_comma+1, second_comma-first_comma-1));
	z = strToFloat(str.substr(second_comma+1, v_end-second_comma-1));
}

inline Vector3::Vector3(Json const& json)
{
	constructFromJson(json);
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

inline std::string Vector3::toString(void) const
{
	return "(" + floatToStr(x) + ", " + floatToStr(y) + ", " + floatToStr(z) + ")";
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

inline bool Vector3::operator==(Vector3 const& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

inline bool Vector3::operator!=(Vector3 const& v) const
{
	return x != v.x || y != v.y || z != v.z;
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
	strm << v.toString();
	return strm;
}

inline Vector3 operator*(Real f, Vector3 const& v)
{
	return Vector3(f * v.x, f * v.y, f * v.z);
}

inline Json Vector3::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(x));
	result.addItem(Json::newNumber(y));
	result.addItem(Json::newNumber(z));
	return result;
}

inline void Vector3::constructFromJson(Json const& json)
{
	// Old method
	if (json.getType() == Json::OBJECT) {
		// Check JSON validity
		if (!json.keyExists("x") || json.getMember("x").getType() != Json::NUMBER) throw Exception("JSON for Vector3 must contain number named \"x\"!");
		if (!json.keyExists("y") || json.getMember("y").getType() != Json::NUMBER) throw Exception("JSON for Vector3 must contain number named \"y\"!");
		if (!json.keyExists("z") || json.getMember("z").getType() != Json::NUMBER) throw Exception("JSON for Vector3 must contain number named \"z\"!");
		// Construct
		x = json.getMember("x").getNumber();
		y = json.getMember("y").getNumber();
		z = json.getMember("z").getNumber();
		return;
	}
	// Check JSON validity
	if (json.getType() != Json::ARRAY) throw Exception("JSON for Vector3 must be an array!");
	if (json.getArraySize() != 3) throw Exception("JSON for Vector3 must contain exactly three numbers!");
	for (size_t num_id = 0; num_id < 3; ++ num_id) {
		if (json.getItem(num_id).getType() != Json::NUMBER) throw Exception("Unexpected non-number in JSON array for Vector3!");
	}
	// Construct
	x = json.getItem(0).getNumber();
	y = json.getItem(1).getNumber();
	z = json.getItem(2).getNumber();
}

inline void Vector3::doSerialize(ByteV& result, bool bigendian) const
{
	result.reserve(result.size() + 4*3);
	result += floatToByteV(x, bigendian);
	result += floatToByteV(y, bigendian);
	result += floatToByteV(z, bigendian);
}

inline void Vector3::doDeserialize(std::istream& strm, bool bigendian)
{
	char buf[12];
	strm.read(buf, 12);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	x = cStrToFloat(buf, bigendian);
	y = cStrToFloat(buf + 4, bigendian);
	z = cStrToFloat(buf + 8, bigendian);
}

}

#endif
