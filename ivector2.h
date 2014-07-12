#ifndef HPP_IVECTOR2_H
#define HPP_IVECTOR2_H

#include "cast.h"
#include "json.h"
#include "real.h"
#include "serializable.h"
#include "deserializable.h"

#include <cmath>
#include <string>
#include <ostream>
#include <vector>

namespace Hpp
{

class IVector2 : public Serializable, public Deserializable
{

public:

	typedef std::vector< IVector2 > Vec;

	static IVector2 const ONE;
	static IVector2 const ZERO;
	static IVector2 const X;
	static IVector2 const Y;

	// Constructors
	inline IVector2(void);
	inline IVector2(ssize_t x, ssize_t y);
	inline IVector2(Json const& json);

	// Copy constructor and assignment operator
	inline IVector2(IVector2 const& v);
	inline IVector2& operator=(IVector2 const& v);

	inline Real length(void) const;
	inline size_t lengthTo2(void) const;

	// Miscellaneous operators
	inline IVector2 operator+(IVector2 const& v) const;
	inline IVector2 operator-(IVector2 const& v) const;
	inline IVector2 operator+=(IVector2 const& v);
	inline IVector2 operator-=(IVector2 const& v);
	inline IVector2 operator*(ssize_t i) const;
	inline IVector2 operator*=(ssize_t i);
	inline IVector2 operator/(ssize_t i) const;
	inline IVector2 operator/=(ssize_t i);
	inline IVector2 operator-(void) const;

	// Conversion functions
	inline std::string toString(void) const;

	// For comparison
	inline bool operator==(IVector2 const& v) const;
	inline bool operator!=(IVector2 const& v) const;

	// For sorting
	inline bool operator<(IVector2 const& v) const;

	ssize_t x, y;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual Json toJson(void) const;
	inline virtual void constructFromJson(Json const& json);

private:

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result, bool bigendian) const;
	inline virtual void doDeserialize(std::istream& strm, bool bigendian);

};


// ----------------------------------------
// Global functions of IVector2
// ----------------------------------------

inline std::ostream& operator<<(std::ostream& strm, IVector2 const& v);
inline ssize_t dotProduct(IVector2 const& v1, IVector2 const& v2);



// ----------------------------------------
// ----------------------------------------
//
// Implemented inline functions of IVector2
//
// ----------------------------------------
// ----------------------------------------


inline IVector2::IVector2(void)
{
}

inline IVector2::IVector2(ssize_t x, ssize_t y) :
x(x), y(y)
{
}

inline IVector2::IVector2(Json const& json)
{
	constructFromJson(json);
}

inline IVector2::IVector2(IVector2 const& v) :
x(v.x), y(v.y)
{
}

inline IVector2& IVector2::operator=(IVector2 const& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

inline Real IVector2::length(void) const
{
	return sqrt(x*x + y*y);
}

inline size_t IVector2::lengthTo2(void) const
{
	return x*x + y*y;
}

inline IVector2 IVector2::operator+(IVector2 const& v) const
{
	return IVector2(x + v.x, y + v.y);
}

inline IVector2 IVector2::operator-(IVector2 const& v) const
{
	return IVector2(x - v.x, y - v.y);
}

inline IVector2 IVector2::operator+=(IVector2 const& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline IVector2 IVector2::operator-=(IVector2 const& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline IVector2 IVector2::operator*(ssize_t i) const
{
	return IVector2(x * i, y * i);
}

inline IVector2 IVector2::operator*=(ssize_t i)
{
	x *= i;
	y *= i;
	return *this;
}

inline IVector2 IVector2::operator/(ssize_t i) const
{
	HppAssert(i != 0, "Division by zero!");
	return IVector2(x / i, y / i);
}

inline IVector2 IVector2::operator/=(ssize_t i)
{
	HppAssert(i != 0, "Division by zero!");
	x /= i;
	y /= i;
	return *this;
}


inline IVector2 IVector2::operator-(void) const
{
	return IVector2(-x, -y);
}

inline std::string IVector2::toString(void) const
{
	return "(" + ssizeToStr(x) + ", " + ssizeToStr(y) + ")";
}

inline bool IVector2::operator==(IVector2 const& v) const
{
	return x == v.x && y == v.y;
}

inline bool IVector2::operator!=(IVector2 const& v) const
{
	return x != v.x || y != v.y;
}

inline bool IVector2::operator<(IVector2 const& v) const
{
	if (y < v.y) return true;
	if (y > v.y) return false;
	if (x < v.x) return true;
	return false;
}

inline Json IVector2::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(int64_t(x)));
	result.addItem(Json::newNumber(int64_t(y)));
	return result;
}

inline void IVector2::constructFromJson(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::ARRAY) throw Exception("JSON for IVector2 must be an array!");
	if (json.getArraySize() != 2) throw Exception("JSON for IVector2 must contain exactly two numbers!");
	for (size_t num_id = 0; num_id < 2; ++ num_id) {
		if (json.getItem(num_id).getType() != Json::NUMBER) throw Exception("Unexpected non-number in JSON array for IVector2!");
	}
	// Construct
	x = json.getItem(0).getInteger();
	y = json.getItem(1).getInteger();
}

inline void IVector2::doSerialize(ByteV& result, bool bigendian) const
{
	result += int64ToByteV(x, bigendian);
	result += int64ToByteV(y, bigendian);
}

inline void IVector2::doDeserialize(std::istream& strm, bool bigendian)
{
	char buf[16];
	strm.read(buf, 16);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	x = cStrToInt64(buf, bigendian);
	y = cStrToInt64(buf + 8, bigendian);
}

inline std::ostream& operator<<(std::ostream& strm, IVector2 const& v)
{
	return strm << v.toString();
}

}

#endif
