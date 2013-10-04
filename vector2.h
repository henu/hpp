#ifndef HPP_VECTOR2_H
#define HPP_VECTOR2_H

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

class Vector2 : public Serializable, public Deserializable
{

public:

	typedef std::vector< Vector2 > Vec;

	static Vector2 const ONE;
	static Vector2 const ZERO;
	static Vector2 const X;
	static Vector2 const Y;

	inline Vector2(void);
	inline Vector2(Real x, Real y);
	inline Vector2(std::string const& str);
	inline Vector2(Json const& json);

	// Miscellaneous functions
	inline Real length(void) const;
	inline Real lengthTo2(void) const;
	inline void normalize(void);
	inline Vector2 normalized(void) const;

	// Conversion functions
	inline std::string toString(void) const;
	inline Json toJson(void) const;

	// Operators between Vector2s
	inline Vector2 operator-(void) const;
	inline Vector2 operator+(Vector2 const& v) const;
	inline Vector2 operator-(Vector2 const& v) const;
	inline Vector2 operator*(Vector2 const& v) const;
	inline Vector2 operator+=(Vector2 const& v);
	inline Vector2 operator-=(Vector2 const& v);
	inline Vector2 operator*=(Vector2 const& v);

	// Operators with other types
	inline Vector2 operator*(Real r) const;
	inline Vector2 operator/(Real r) const;
	inline Vector2 operator*=(Real r);
	inline Vector2 operator/=(Real r);

	// Comparison operators
	inline bool operator==(Vector2 const& v) const;
	inline bool operator!=(Vector2 const& v) const;

	// Comparison operator for sorting
	inline bool operator<(Vector2 const& v) const;

	// Returns vector that is perpendicular (that is, 90 degrees) to this
	// one. Basically result is this vector that is rotated 90 degrees
	// counter clockwise when X axis is right and Y axis up.
	inline Vector2 perp(void) const;

	Real x, y;

private:

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result, bool bigendian) const;
	inline virtual void doDeserialize(std::istream& strm, bool bigendian);

};

inline std::ostream& operator<<(std::ostream& strm, Vector2 const& v);

inline Vector2 operator*(Real f, Vector2 const& v);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Vector2::Vector2(void)
{
}

inline Vector2::Vector2(Real x, Real y) :
x(x), y(y)
{
}

inline Vector2::Vector2(std::string const& str)
{
	// Strip possible parenthesis around numeric values
	std::string::size_type v_begin = str.find_first_not_of("([{ \t\n");
	if (v_begin == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector2 because it does not contain any numerals!");
	}
	std::string::size_type v_end = str.find_last_not_of(")]} \t\n");
	if (v_end == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector2 because it does not contain any numerals!");
	}
	v_end ++;
	// Find commas and do conversion
	std::string::size_type comma = str.find(',', v_begin);
	if (comma == std::string::npos) {
		throw Exception("Unable to convert string \"" + str + "\" to Vector2 because comma is not found!");
	}
	x = strToFloat(str.substr(v_begin, comma));
	y = strToFloat(str.substr(comma+1, v_end-comma-1));
}

inline Vector2::Vector2(Json const& json)
{
	// Old method
	if (json.getType() == Json::OBJECT) {
		// Check JSON validity
		if (!json.keyExists("x") || json.getMember("x").getType() != Json::NUMBER) throw Exception("JSON for Vector2 must contain number named \"x\"!");
		if (!json.keyExists("y") || json.getMember("y").getType() != Json::NUMBER) throw Exception("JSON for Vector2 must contain number named \"y\"!");
		// Construct
		x = json.getMember("x").getNumber();
		y = json.getMember("y").getNumber();
		return;
	}
	// Check JSON validity
	if (json.getType() != Json::ARRAY) throw Exception("JSON for Vector2 must be an array!");
	if (json.getArraySize() != 2) throw Exception("JSON for Vector2 must contain exactly two numbers!");
	for (size_t num_id = 0; num_id < 2; ++ num_id) {
		if (json.getItem(num_id).getType() != Json::NUMBER) throw Exception("Unexpected non-number in JSON array for Vector2!");
	}
	// Construct
	x = json.getItem(0).getNumber();
	y = json.getItem(1).getNumber();
}

inline Real Vector2::length(void) const
{
	return sqrt(x*x + y*y);
}

inline Real Vector2::lengthTo2(void) const
{
	return x*x + y*y;
}

inline void Vector2::normalize(void)
{
	Real len = length();
	HppAssert(len != 0.0, "Division by zero!");
	x /= len;
	y /= len;
}

inline Vector2 Vector2::normalized(void) const
{
	Vector2 result;
	Real len = length();
	HppAssert(len != 0.0, "Division by zero!");
	result.x = x / len;
	result.y = y / len;
	return result;
}

inline std::string Vector2::toString(void) const
{
	return "(" + floatToStr(x) + ", " + floatToStr(y) + ")";
}

inline Json Vector2::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(x));
	result.addItem(Json::newNumber(y));
	return result;
}

inline Vector2 Vector2::operator-(void) const
{
	return Vector2(-x, -y);
}

inline Vector2 Vector2::operator+(Vector2 const& v) const
{
	return Vector2(x+v.x, y+v.y);
}

inline Vector2 Vector2::operator-(Vector2 const& v) const
{
	return Vector2(x-v.x, y-v.y);
}

inline Vector2 Vector2::operator*(Vector2 const& v) const
{
	return Vector2(x*v.x, y*v.y);
}

inline Vector2 Vector2::operator+=(Vector2 const& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline Vector2 Vector2::operator-=(Vector2 const& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline Vector2 Vector2::operator*=(Vector2 const& v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

inline Vector2 Vector2::operator*(Real r) const
{
	return Vector2(x*r, y*r);
}

inline Vector2 Vector2::operator/(Real r) const
{
	HppAssert(r != 0.0, "Division by zero!");
	return Vector2(x/r, y/r);
}

inline Vector2 Vector2::operator*=(Real r)
{
	x *= r;
	y *= r;
	return *this;
}

inline Vector2 Vector2::operator/=(Real r)
{
	HppAssert(r != 0.0, "Division by zero!");
	x /= r;
	y /= r;
	return *this;
}

inline bool Vector2::operator==(Vector2 const& v) const
{
	return x == v.x && y == v.y;
}

inline bool Vector2::operator!=(Vector2 const& v) const
{
	return x != v.x || y != v.y;
}

inline bool Vector2::operator<(Vector2 const& v) const
{
	if (x < v.x) return true;
	if (x > v.x) return false;
	if (y < v.y) return true;
	return false;
}

inline Vector2 Vector2::perp(void) const
{
	return Vector2(-y, x);
}

inline std::ostream& operator<<(std::ostream& strm, Vector2 const& v)
{
	strm << v.toString();
	return strm;
}

inline Vector2 operator*(Real f, Vector2 const& v)
{
	return Vector2(f * v.x, f * v.y);
}

inline void Vector2::doSerialize(ByteV& result, bool bigendian) const
{
	result.reserve(result.size() + 4*2);
	result += floatToByteV(x, bigendian);
	result += floatToByteV(y, bigendian);
}

inline void Vector2::doDeserialize(std::istream& strm, bool bigendian)
{
	char buf[8];
	strm.read(buf, 8);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	x = cStrToFloat(buf, bigendian);
	y = cStrToFloat(buf + 4, bigendian);
}

}

#endif
