#ifndef HPP_MISC_H
#define HPP_MISC_H

#include "bytev.h"
#include "angle.h"
#include "trigon.h"

#include <algorithm>
#include <string>

#define HppFuncStr() (std::string(__FUNCTION__) + ": ")

namespace Hpp
{

template< typename Type >
inline void toZero(Type* buf, size_t size)
{
	#ifndef WIN32
	bzero(buf, size);
	#else
	char* buf_end = (char*)buf + size;
	for (char* buf_it = (char*)buf;
	     buf_it != buf_end;
	     buf_it ++) {
	     	*buf_it = 0;
	}
	#endif
}

template< typename Type >
inline Type clamp(Type min, Type max, Type val);

// Calculates median. Range must be sorted
template< typename Type, typename Iter1, typename Iter2 >
inline void median(Type& result, Iter1 const& begin, Iter2 const& end);

// Compares two ranges. Returns greater than, equal, or less than zero
// if range #1 is greater, equal, or less than range #2, respectively.
template< typename Iter1, typename Iter2, typename Iter3 >
inline int compare(Iter1 range1_begin, Iter2 range1_end, Iter3 range2_begin);

// Divides ByteV/string to parts using separator character. Last functions do
// this in parts to save memory. They return true whenever subsearch was found.
inline std::vector< ByteV > splitString(ByteV const& v, char separator);
inline std::vector< std::string > splitString(std::string const& str, char separator);
template < class Type, class IterIt, class IterEnd, class Char >
inline bool splitStringInParts(Type& result, IterIt& it, IterEnd const& end, Char separator);

// Removes whitespace from beginning and end of string
inline std::string trim(std::string const& str, std::string ws = " \t\n");

// Power function for integers
inline ssize_t ipow(ssize_t a, size_t n);

// Division functions for integers that always round to down/up
inline ssize_t iDivFloor(ssize_t dividend, ssize_t divisor);
inline ssize_t iDivCeil(ssize_t dividend, ssize_t divisor);

// Modulo function that does not produce negative results. Always returns value
// that is in range [0 - divisor). Note, that divisor must be positive!
inline size_t iMod(ssize_t dividend, size_t divisor);

// Converts camera fov angles between X and Y.
inline Angle fovYToFovX(Angle const& fov_y, Real display_w, Real display_h);
inline Angle fovXToFovY(Angle const& fov_x, Real display_w, Real display_h);

// Converts tab, enter, \ and given characters to slashes
inline std::string slashEncode(std::string str, std::string const& chars = "\"\'");


// ----------------------------------------
// Implementation of functions
// ----------------------------------------

template< typename Type >
inline Type clamp(Type min, Type max, Type val)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

template< typename Type, typename Iter1, typename Iter2 >
inline void median(Type& result, Iter1 const& begin, Iter2 const& end)
{
	size_t items = end - begin;
	if (items % 2 == 1) {
		size_t offset = items/2;
		result = *(begin + offset);
	} else {
		size_t offset = items/2;
		result = *(begin + offset - 1);
		result += *(begin + offset);
		result /= 2.0;
	}
}

template< typename Iter1, typename Iter2, typename Iter3 >
inline int compare(Iter1 range1_begin, Iter2 range1_end, Iter3 range2_begin)
{
	while (range1_begin != range1_end) {
		if (*range1_begin < *range2_begin) {
			return -1;
		} else if (*range1_begin > *range2_begin) {
			return 1;
		}
		++ range1_begin;
		++ range2_begin;
	}
	return 0;
}

inline std::vector< ByteV > splitString(ByteV const& v, char separator)
{
	std::vector< ByteV > result;
	ByteV::const_iterator part_begin = v.begin();
	for (ByteV::const_iterator v_it = v.begin();
	     v_it != v.end();
	     ++ v_it) {
		char c = *v_it;
		if (c == separator) {
			ByteV part(part_begin, v_it);
			result.push_back(part);
			part_begin = v_it;
			++ part_begin;
		}
	}
	if (part_begin != v.end()) {
		result.push_back(ByteV(part_begin, v.end()));
	}
	return result;
}

inline std::vector< std::string > splitString(std::string const& str, char separator)
{
	std::vector< std::string > result;
	std::string::const_iterator part_begin = str.begin();
	for (std::string::const_iterator str_it = str.begin();
	     str_it != str.end();
	     ++ str_it) {
		char c = *str_it;
		if (c == separator) {
			std::string part(part_begin, str_it);
			result.push_back(part);
			part_begin = str_it;
			++ part_begin;
		}
	}
	if (part_begin != str.end()) {
		result.push_back(std::string(part_begin, str.end()));
	}
	return result;
}

template < class Type, class IterIt, class IterEnd, class Char >
inline bool splitStringInParts(Type& result, IterIt& it, IterEnd const& end, Char separator)
{
	result.clear();

	if (it == end) {
		return false;
	}

	do {
		Char c = *it;
		it ++;

		if (c == separator) {
			break;
		}

		result += c;

	} while (it != end);

	return true;
}

inline std::string trim(std::string const& str, std::string ws)
{
	std::string::size_type non_ws_begin = str.find_first_not_of(ws);
	std::string::size_type non_ws_end = str.find_last_not_of(ws);
	if (non_ws_begin == std::string::npos) {
		return "";
	}
	HppAssert(non_ws_end != std::string::npos, "Fail!");
	return str.substr(non_ws_begin, non_ws_end + 1 - non_ws_begin);
}

inline ssize_t ipow(ssize_t a, size_t n)
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

inline size_t iMod(ssize_t dividend, size_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0) {
		return dividend % divisor;
	}
	return (dividend % (ssize_t)divisor) + divisor;
}

inline Angle fovYToFovX(Angle const& fov_y, Real display_w, Real display_h)
{
	HppAssert(display_w > 0.0, "Display width must be greater than zero!");
	HppAssert(display_h > 0.0, "Display height must be greater than zero!");
	Real aspect_ratio = display_w / display_h;
	Real plane_height = (fov_y / 2.0).tan() * 2.0;
	Real plane_width = plane_height * aspect_ratio;
	Angle fov_x = atan(plane_width / 2.0) * 2.0;
	return fov_x;
}

inline Angle fovXToFovY(Angle const& fov_x, Real display_w, Real display_h)
{
	HppAssert(display_w > 0.0, "Display width must be greater than zero!");
	HppAssert(display_h > 0.0, "Display height must be greater than zero!");
	return fovYToFovX(fov_x, display_h, display_w);
}

inline std::string slashEncode(std::string str, std::string const& chars)
{
	std::string result;
	result.reserve(str.size());
	for (std::string::const_iterator it = str.begin();
	     it != str.end();
	     ++ it) {
		char c = *it;
		if (c == '\t') {
			result += "\\t";
		} else if (c == '\n') {
			result += "\\n";
		} else if (c == '\\') {
			result += "\\\\";
		} else if (chars.find(c) != std::string::npos) {
			result += "\\";
			result += c;
		} else {
			result += c;
		}
	}
	return result;
}

// Goes through a range of elements and checks which of them should be moved to
// the back of range. Check function must return true to those that are needed
// to be moved. Returns iterator to the first element in the range of moved
// elements. Does not keep range in order!
template< typename FIter, typename CompFunc >
inline FIter moveToBack(FIter begin, FIter end, CompFunc cfunc)
{
	FIter new_end = end;
	while (begin != new_end) {
		if (cfunc(*begin)) {
			std::swap(*begin, *(new_end-1));
			new_end --;
		} else {
			begin ++;
		}
	}
	return new_end;
}

}

#endif
