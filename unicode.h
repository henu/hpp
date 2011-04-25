#ifndef HPP_UNICODE_H
#define HPP_UNICODE_H

#include "assert.h"
#include "exception.h"

#include <string>
#include <stdint.h>

namespace Hpp
{

typedef uint32_t UChr;

// Extracts unicode characters from UTF-8 encoded strings. If unexpected end is
// met, then an exception is thrown.
template< typename Iter1, typename Iter2 >
inline UChr extractUTF8(Iter1& it, Iter2 end);

// Calculates length of UTF8 encoded string
template< typename Iter1, typename Iter2 >
inline size_t sizeUTF8(Iter1 begin, Iter2 end);

// Calculates length of next UTF8 encoded character. Also moves iterator to the
// beginning of next character or to the end of string if this is the last
// character.
template< typename Iter1, typename Iter2 >
inline uint8_t sizeUTF8Char(Iter1& it, Iter2 end);

// Calculates length of last UTF8 encoded character
template< typename Iter1, typename Iter2 >
inline uint8_t sizeUTF8CharLast(Iter1 begin, Iter2 end);

// Converts single unicode character to UTF8 encoded string
inline std::string uChrToUTF8(UChr c);

// Letter conversions
inline UChr tolower(UChr c);
inline UChr toupper(UChr c);



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline functions
//
// ----------------------------------------
// ----------------------------------------

template< typename Iter1, typename Iter2 >
inline UChr extractUTF8(Iter1& it, Iter2 end)
{
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c1 = *it;
	it ++;
	if ((c1 & 0x80) == 0x00) {
		return c1;
	}
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c2 = *it;
	it ++;
	if ((c1 & 0xe0) == 0xc0 && (c2 & 0xc0) == 0x80) {
		UChr result = (c2 & 0x3f) +
		              ((c1 & 0x1f) << 6);
		return result;
	}
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c3 = *it;
	it ++;
	if ((c1 & 0xf0) == 0xe0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80) {
		UChr result = (c3 & 0x3f) +
		              ((c2 & 0x3f) << 6) +
		              ((c1 & 0x0f) << 12);
		return result;
	}
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c4 = *it;
	it ++;
	if ((c1 & 0xf8) == 0xf0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80 && (c4 & 0xc0) == 0x80) {
		UChr result = (c4 & 0x3f) +
		              ((c3 & 0x3f) << 6) +
		              ((c2 & 0x3f) << 12) +
		              ((c1 & 0x07) << 18);
		return result;
	}
	throw Exception("Corrupted UTF-8 string!");
	return 0;
}

template< typename Iter1, typename Iter2 >
inline size_t sizeUTF8(Iter1 begin, Iter2 end)
{
	size_t result = 0;
	while (begin != end) {
		uint8_t c1 = *begin;
		begin ++;
		if ((c1 & 0x80) == 0x00) {
			result ++;
			continue;
		}
		if (begin == end) throw Exception("Unexpected end of UTF-8 string!");
		uint8_t c2 = *begin;
		begin ++;
		if ((c1 & 0xe0) == 0xc0 && (c2 & 0xc0) == 0x80) {
			result ++;
			continue;
		}
		if (begin == end) throw Exception("Unexpected end of UTF-8 string!");
		uint8_t c3 = *begin;
		begin ++;
		if ((c1 & 0xf0) == 0xe0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80) {
			result ++;
			continue;
		}
		if (begin == end) throw Exception("Unexpected end of UTF-8 string!");
		uint8_t c4 = *begin;
		begin ++;
		if ((c1 & 0xf8) == 0xf0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80 && (c4 & 0xc0) == 0x80) {
			result ++;
			continue;
		}
	}
	return result;
}

template< typename Iter1, typename Iter2 >
inline uint8_t sizeUTF8Char(Iter1& it, Iter2 end)
{
	if (it == end) return 0;

	uint8_t c1 = *it;
	it ++;
	if ((c1 & 0x80) == 0x00) {
		return 1;
	}

	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c2 = *it;
	it ++;
	if ((c1 & 0xe0) == 0xc0 && (c2 & 0xc0) == 0x80) {
		return 2;
	}
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c3 = *it;
	it ++;
	if ((c1 & 0xf0) == 0xe0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80) {
		return 3;
	}
	if (it == end) throw Exception("Unexpected end of UTF-8 string!");
	uint8_t c4 = *it;
	it ++;
	if ((c1 & 0xf8) ==0xf0 && (c2 & 0xc0) == 0x80 && (c3 & 0xc0) == 0x80 && (c4 & 0xc0) == 0x80) {
		return 4;
	}
	throw Exception("Unsupported UTF-8 character of more than four bytes!");
	return 0;
}

template< typename Iter1, typename Iter2 >
inline uint8_t sizeUTF8CharLast(Iter1 begin, Iter2 end)
{
	if (begin == end) {
		return 0;
	}
	Iter1 it = end;
	while (it != begin) {
		it --;
		try {
			Iter1 it2 = it;
			uint8_t size = sizeUTF8Char(it2, end);
			// If the handled character was not the last one, then
			// it means that there is corrupted content after this
			// one correct character.
			if (it2 != end) {
				throw Exception("The end of string is not valid UTF-8 character!");
			}
			return size;
		}
		catch ( ... ) {
			continue;
		}
	}
	throw Exception("Corrupted UTF-8 string!");
	return 0;
}

inline std::string uChrToUTF8(UChr c)
{
	if (c < 0x007f) {
		return std::string(1, c);
	}
	std::string result;
	if (c < 0x07ff) {
		result.reserve(2);
		result += static_cast< char >(0xc0 + (c >> 6));
		result += static_cast< char >(0x80 + (c & 0x3f));
	} else if (c < 0xffff) {
		result.reserve(3);
		result += static_cast< char >(0xe0 + (c >> 12));
		result += static_cast< char >(0x80 + ((c >> 6) & 0x3f));
		result += static_cast< char >(0x80 + (c & 0x3f));
	} else {
		HppAssert(c < 0x10ffff, "Unsupported unicode character!");
		result.reserve(4);
		result += static_cast< char >(0xf0 + (c >> 18));
		result += static_cast< char >(0x80 + ((c >> 12) & 0x3f));
		result += static_cast< char >(0x80 + ((c >> 6) & 0x3f));
		result += static_cast< char >(0x80 + (c & 0x3f));
	}
	return result;
}

inline UChr tolower(UChr c)
{
// TODO: Support more characters!
	if (c >= 'A' && c <= 'Z') {
		return c + 'a' - 'A';
	}
	return c;
}

inline UChr toupper(UChr c)
{
// TODO: Support more characters!
	if (c >= 'a' && c <= 'z') {
		return c + 'A' - 'z';
	}
	return c;
}

}

#endif
