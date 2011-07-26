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

// Divides ByteV/string to parts using separator character
inline std::vector< ByteV > explode(ByteV const& v, char separator);
inline std::vector< std::string > explode(std::string const& str, char separator);

// Removes whitespace from beginning and end of string
inline std::string trim(std::string const& str, std::string ws = " \t\n");

// Power function for integers
inline ssize_t ipow(ssize_t a, size_t n);

// Converts camera fov angles between X and Y.
inline Angle fovYToFovX(Angle const& fov_y, Real display_w, Real display_h);
inline Angle fovXToFovY(Angle const& fov_x, Real display_w, Real display_h);

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

inline std::vector< ByteV > explode(ByteV const& v, char separator)
{
	std::vector< ByteV > result;
	ByteV::const_iterator part_begin = v.begin();
	for (ByteV::const_iterator v_it = v.begin();
	     v_it != v.end();
	     v_it ++) {
		char c = *v_it;
		if (c == separator) {
			ByteV part(part_begin, v_it);
			result.push_back(part);
			part_begin = v_it;
			part_begin ++;
		}
	}
	if (part_begin != v.end()) {
		result.push_back(ByteV(part_begin, v.end()));
	}
	return result;
}

inline std::vector< std::string > explode(std::string const& str, char separator)
{
	std::vector< std::string > result;
	std::string::const_iterator part_begin = str.begin();
	for (std::string::const_iterator str_it = str.begin();
	     str_it != str.end();
	     str_it ++) {
		char c = *str_it;
		if (c == separator) {
			std::string part(part_begin, str_it);
			result.push_back(part);
			part_begin = str_it;
			part_begin ++;
		}
	}
	if (part_begin != str.end()) {
		result.push_back(std::string(part_begin, str.end()));
	}
	return result;
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
