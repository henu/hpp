#ifndef HPP_IVECTORS_H
#define HPP_IVECTORS_H

#include "assert.h"

#include <cmath>
#include <ostream>


namespace Hpp
{



// ----------------------------------------
// ----------------------------------------
//
// Class IVector2
//
// ----------------------------------------
// ----------------------------------------

class IVector2
{

public:

	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	// Constructor and destructor
	inline IVector2(void);

	// Miscellaneous constructors
	inline IVector2(ssize_t x, ssize_t y);

	// Copy constructor and assignment operator
	inline IVector2(IVector2 const& v);
	inline IVector2& operator=(IVector2 const& v);

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


	// ----------------------------------------
	// Public members
	// ----------------------------------------

	ssize_t x, y;

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


IVector2::IVector2(void)
{
}

IVector2::IVector2(ssize_t x, ssize_t y) :
x(x), y(y)
{
}

IVector2::IVector2(IVector2 const& v) :
x(v.x), y(v.y)
{
}

IVector2& IVector2::operator=(IVector2 const& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

IVector2 IVector2::operator+(IVector2 const& v) const
{
	return IVector2(x + v.x, y + v.y);
}

IVector2 IVector2::operator-(IVector2 const& v) const
{
	return IVector2(x - v.x, y - v.y);
}

IVector2 IVector2::operator+=(IVector2 const& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

IVector2 IVector2::operator-=(IVector2 const& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

IVector2 IVector2::operator*(ssize_t i) const
{
	return IVector2(x * i, y * i);
}

IVector2 IVector2::operator*=(ssize_t i)
{
	x *= i;
	y *= i;
	return *this;
}

IVector2 IVector2::operator/(ssize_t i) const
{
	HppAssert(i != 0, "Division by zero!");
	return IVector2(x / i, y / i);
}

IVector2 IVector2::operator/=(ssize_t i)
{
	HppAssert(i != 0, "Division by zero!");
	x /= i;
	y /= i;
	return *this;
}


IVector2 IVector2::operator-(void) const
{
	return IVector2(-x, -y);
}

std::ostream& operator<<(std::ostream& strm, IVector2 const& v)
{
	strm << '(';
	strm << v.x;
	strm << ", ";
	strm << v.y;
	strm << ')';
	return strm;
}

}

#endif

