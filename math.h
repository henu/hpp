#ifndef MATH_H
#define MATH_H

#include "assert.h"

#include <cstdlib>

namespace Hpp
{

// Power function for integers
inline ssize_t iPow(ssize_t a, size_t n);

// Division functions for integers that always round to down/up
inline ssize_t iDivFloor(ssize_t dividend, ssize_t divisor);
inline ssize_t iDivCeil(ssize_t dividend, ssize_t divisor);

// Rounding, ceiling and floor for integers
inline ssize_t iRound(double d);
inline ssize_t iFloor(double d);
inline ssize_t iCeil(double d);

// Modulo function that does not produce negative results. Always returns value
// that is in range [0 - divisor). Note, that divisor must be positive!
inline size_t iMod(ssize_t dividend, size_t divisor);
inline double dMod(double dividend, double divisor);

inline ssize_t iPow(ssize_t a, size_t n)
{
	ssize_t result = 1;
	while (n > 0) {
		// If n is odd, then multiply and reduce n
		if (n % 2 == 1) {
			result *= a;
			n --;
		}
		// n is even, so it can be halved
		// if a is multiplied with itself.
		else {
			n /= 2;
			a *= a;
		}
	}
	return result;
}

inline ssize_t iDivFloor(ssize_t dividend, ssize_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0 && divisor > 0) {
		return dividend / divisor;
	}
	if (dividend < 0 && divisor < 0) {
		return dividend / divisor;
	}
	if (dividend < 0) {
		HppAssert(divisor > 0, "Fail!");
		return (dividend - (divisor - 1)) / divisor;
	}
	return (dividend - (divisor + 1)) / divisor;
}

inline ssize_t iDivCeil(ssize_t dividend, ssize_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0 && divisor > 0) {
		return (dividend + (divisor - 1)) / divisor;
	}
	if (dividend < 0 && divisor < 0) {
		return (dividend + (divisor + 1)) / divisor;
	}
	return dividend / divisor;
}

inline ssize_t iRound(double d)
{
	if (d >= 0) {
		return ssize_t(d + 0.5);
	}
	return ssize_t(d - 0.5);
}

inline ssize_t iFloor(double d)
{
	ssize_t result = d;
	if (result > d) {
		-- result;
	}
	HppAssert(result <= d, "Fail!");
	return result;
}

inline ssize_t iCeil(double d)
{
	ssize_t result = d;
	if (result < d) {
		++ result;
	}
	HppAssert(result >= d, "Fail!");
	return result;
}

inline size_t iMod(ssize_t dividend, size_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0) {
		return dividend % divisor;
	}
	return (dividend % (ssize_t)divisor) + divisor;
}

inline double dMod(double dividend, double divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0) {
		dividend /= divisor;
		dividend -= ssize_t(dividend);
		return dividend * divisor;
	}
	dividend /= divisor;
	dividend += ssize_t(-dividend) + 1;
	return dividend * divisor;
}

}

#endif
