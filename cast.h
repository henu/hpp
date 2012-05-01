#ifndef HPP_CAST_H
#define HPP_CAST_H

#include "assert.h"
#include "exception.h"
#include "bytev.h"
#include "vector2.h"
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"

#include <sstream>
#include <cstdlib>

namespace Hpp
{

inline std::string	byteVToDStr(ByteV const& v);
inline std::string	byteVToHexV(ByteV const& bytev);
inline std::string	byteVToStr(ByteV const& v);
inline float		cStrToFloat(uint8_t const* c_str, bool bigendian = true);
inline ssize_t		cStrToInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian = true);
inline int16_t		cStrToInt16(uint8_t const* c_str, bool bigendian = true);
inline int32_t		cStrToInt32(uint8_t const* c_str, bool bigendian = true);
inline size_t		cStrToUInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian = true);
inline uint16_t		cStrToUInt16(uint8_t const* c_str, bool bigendian = true);
inline uint32_t		cStrToUInt32(uint8_t const* c_str, bool bigendian = true);
inline float		cStrToFloat(char const* c_str, bool bigendian = true) { return cStrToFloat((uint8_t const*)c_str, bigendian); }
inline int16_t		cStrToInt16(char const* c_str, bool bigendian = true) { return cStrToInt16((uint8_t const*)c_str, bigendian); }
inline int32_t		cStrToInt32(char const* c_str, bool bigendian = true) { return cStrToInt32((uint8_t const*)c_str, bigendian); }
inline uint16_t		cStrToUInt16(char const* c_str, bool bigendian = true) { return cStrToUInt16((uint8_t const*)c_str, bigendian); }
inline uint32_t		cStrToUInt32(char const* c_str, bool bigendian = true) { return cStrToUInt32((uint8_t const*)c_str, bigendian); }
inline ByteV		dStrToByteV(std::string const& dstr);
inline ByteV		floatToByteV(float f, bool bigendian = true);
inline void		floatToCStr(float f, uint8_t* c_str, bool bigendian = true);
inline std::string	floatToStr(float f);
inline ByteV		hexVToByteV(std::string const& hexv);
inline ByteV		intToByteV(ssize_t i, uint8_t bytesize, bool bigendian = true);
inline ByteV		int16ToByteV(int16_t i, bool bigendian = true);
inline void		int16ToCStr(int16_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		int32ToByteV(int32_t i, bool bigendian = true);
inline void		int32ToCStr(int32_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		matrix3ToBytes(Matrix3 const& m, bool bigendian = true);
inline ByteV		matrix4ToBytes(Matrix4 const& m, bool bigendian = true);
inline std::string 	sizeToStr(size_t i);
inline std::string	ssizeToStr(ssize_t i);
inline float		strToFloat(std::string const& str);
inline Vector2		strToVector2(std::string const& str);
inline Vector3		strToVector3(std::string const& str);
inline size_t		strToSize(std::string const& str);
inline ssize_t		strToSSize(std::string const& str);
inline ByteV		uInt16ToByteV(uint16_t i, bool bigendian = true);
inline void		uInt16ToCStr(uint16_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		uInt32ToByteV(uint32_t i, bool bigendian = true);
inline void		uInt32ToCStr(uint32_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		uIntToByteV(size_t i, uint8_t bytesize, bool bigendian = true);
inline void		uIntToCStr(size_t i, uint8_t* c_str, uint8_t bytesize, bool bigendian = true);

inline std::string byteVToDStr(ByteV const& v)
{
	std::string result;
	result.reserve((size_t)(v.size() * 1.334 + 0.666));
	uint16_t bits;
	#ifndef NDEBUG
	bits = 0;
	#endif
	uint8_t bits_size = 0;
	for (ByteV::const_iterator v_it = v.begin();
	     v_it != v.end();
	     v_it ++) {
		uint8_t c8 = *v_it;
		// Read bits to buffer
		bits <<= 8;
		bits |= c8;
		bits_size += 8;
		HppAssert(bits_size <= 16, "Bit buffer has grown too large!");
		while (bits_size >= 6) {
			uint8_t c6 = (bits >> (bits_size-6)) & 0x3f;
			bits_size -= 6;
			if (c6 < 10) result += (char)('0' + c6);
			else if (c6 < 36) result += (char)('A' + c6 - 10);
			else if (c6 < 62) result += (char)('a' + c6 - 36);
			else if (c6 == 62) result += '.';
			else result += '_';
		}
	}
	if (bits_size > 0) {
		// Add extra bits
		bits <<= (6 - bits_size);
		uint8_t c6 = bits & 0x3f;
		if (c6 < 10) result += (char)('0' + c6);
		else if (c6 < 36) result += (char)('A' + c6 - 10);
		else if (c6 < 62) result += (char)('a' + c6 - 36);
		else if (c6 == 62) result += '.';
		else result += '_';
	}
	HppAssert(dStrToByteV(result) == v, "Conversion has failed!");
	return result;
}

inline std::string byteVToHexV(ByteV const& bytev)
{
	std::string result;
	result.reserve(bytev.size() * 2);
	for (ByteV::const_iterator bytev_it = bytev.begin();
	     bytev_it != bytev.end();
	     bytev_it ++) {
		uint8_t part = (*bytev_it >> 4);
		if (part < 10) {
			result.push_back('0' + part);
		} else {
			result.push_back('a' + part - 10);
		}
		part = (*bytev_it & 0x0f);
		if (part < 10) {
			result.push_back('0' + part);
		} else {
			result.push_back('a' + part - 10);
		}
	}
	return result;
}

inline std::string byteVToStr(ByteV const& v)
{
	return std::string(v.begin(), v.end());
}

inline float cStrToFloat(uint8_t const* c_str, bool bigendian)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
// TODO: The next thing may put debuggers crazy, as the f might be consider uninitialized
	float f;
	uint8_t* f_cp = (uint8_t*)(&f);
	if (bigendian) {
		f_cp[0] = c_str[3];
		f_cp[1] = c_str[2];
		f_cp[2] = c_str[1];
		f_cp[3] = c_str[0];
	} else {
		f_cp[0] = c_str[0];
		f_cp[1] = c_str[1];
		f_cp[2] = c_str[2];
		f_cp[3] = c_str[3];
	}
	return f;
}

inline ssize_t cStrToInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian)
{
	ssize_t i_max_plus_1 = (size_t)1 << (bytesize*8-1);
	ssize_t i_u = cStrToUInt(c_str, bytesize, bigendian);
	if (i_u >= i_max_plus_1) {
		i_u -= i_max_plus_1 * 2;
	}
	return i_u;
}

inline int16_t cStrToInt16(uint8_t const* c_str, bool bigendian)
{
	return (int16_t)cStrToUInt16(c_str, bigendian);
}

inline int32_t cStrToInt32(uint8_t const* c_str, bool bigendian)
{
	return (int32_t)cStrToUInt32(c_str, bigendian);
}

inline size_t cStrToUInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian)
{
	uint32_t i = 0;
	if (bigendian) {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			i += (size_t)c_str[bytesize - 1 -byte] << (8 * byte);
		}
	} else {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			i += (size_t)c_str[byte] << (8 * byte);
		}
	}
	return i;
}

inline uint16_t cStrToUInt16(uint8_t const* c_str, bool bigendian)
{
	uint16_t i = 0;
	if (bigendian) {
		i += c_str[1] << 0;
		i += c_str[0] << 8;
	} else {
		i += c_str[0] << 0;
		i += c_str[1] << 8;
	}
	return i;
}

inline uint32_t cStrToUInt32(uint8_t const* c_str, bool bigendian)
{
	uint32_t i = 0;
	if (bigendian) {
		i += c_str[3] << 0;
		i += c_str[2] << 8;
		i += c_str[1] << 16;
		i += c_str[0] << 24;
	} else {
		i += c_str[0] << 0;
		i += c_str[1] << 8;
		i += c_str[2] << 16;
		i += c_str[3] << 24;
	}
	return i;
}

inline ByteV dStrToByteV(std::string const& dstr)
{
	ByteV result;
	result.reserve((size_t)(dstr.size() * 0.75 + 1));
	uint16_t bits;
	#ifndef NDEBUG
	bits = 0;
	#endif
	uint8_t bits_size = 0;
	for (std::string::const_iterator dstr_it = dstr.begin();
	     dstr_it != dstr.end();
	     dstr_it ++) {
		char c = *dstr_it;
		uint8_t c6;
		// Read bits to buffer
		if (c >= '0' && c <= '9') {
			c6 = c - '0';
		} else if (c >= 'A' && c <= 'Z') {
			c6 = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'z') {
			c6 = c - 'a' + 36;
		} else if (c == '.') {
			c6 = 62;
		} else if (c == '_') {
			c6 = 63;
		} else {
			throw Exception("Corrupted data while casting from dense string to byte vector!");
		}
		bits <<= 6;
		bits |= c6;
		bits_size += 6;
		HppAssert(bits_size <= 16, "Bit buffer has grown too large!");
		while (bits_size >= 8) {
			uint8_t c8 = (bits >> (bits_size-8)) & 0xff;
			bits_size -= 8;
			result.push_back(c8);
		}
	}
	return result;
}

inline ByteV floatToByteV(float f, bool bigendian)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
	uint8_t* f_cp = (uint8_t*)(&f);
	ByteV result;
	result.reserve(4);
	if (bigendian) {
		result.push_back(f_cp[3]);
		result.push_back(f_cp[2]);
		result.push_back(f_cp[1]);
		result.push_back(f_cp[0]);
	} else {
		result.push_back(f_cp[0]);
		result.push_back(f_cp[1]);
		result.push_back(f_cp[2]);
		result.push_back(f_cp[3]);
	}
	return result;
}

inline void floatToCStr(float f, uint8_t* c_str, bool bigendian)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
	uint8_t* f_cp = (uint8_t*)(&f);
	if (bigendian) {
		c_str[0] = f_cp[3];
		c_str[1] = f_cp[2];
		c_str[2] = f_cp[1];
		c_str[3] = f_cp[0];
	} else {
		c_str[0] = f_cp[0];
		c_str[1] = f_cp[1];
		c_str[2] = f_cp[2];
		c_str[3] = f_cp[3];
	}
}

inline std::string floatToStr(float f)
{
	std::ostringstream f_strm;
	f_strm << f;
	return std::string(f_strm.str());
}

inline ByteV hexVToByteV(std::string const& hexv)
{
	// Count how many valid character there is
	uint32_t valid_chars = 0;
	for (std::string::const_iterator hexv_it = hexv.begin();
	     hexv_it != hexv.end();
	     hexv_it ++) {
		if ((*hexv_it >= '0' && *hexv_it <= '9') ||
		    (*hexv_it >= 'a' && *hexv_it <= 'f') ||
		    (*hexv_it >= 'A' && *hexv_it <= 'F')) {
			valid_chars ++;
		}
	}
	ByteV result;
	result.reserve((valid_chars + 1) / 2);
	bool first_part = (valid_chars % 2 == 0);
	if (!first_part) {
		result.push_back(0);
	}
	for (std::string::const_iterator hexv_it = hexv.begin();
	     hexv_it != hexv.end();
	     hexv_it ++) {
		uint8_t ival;
		if (*hexv_it >= '0' && *hexv_it <= '9') {
			ival = *hexv_it - '0';
		} else if (*hexv_it >= 'a' && *hexv_it <= 'f') {
			ival = *hexv_it - 'a' + 10;
		} else if (*hexv_it >= 'A' && *hexv_it <= 'F') {
			ival = *hexv_it - 'A' + 10;
		} else {
			continue;
		}
		if (first_part) {
			result.push_back(ival << 4);
		} else {
			result.back() += ival;
		}
		first_part = !first_part;
	}
	return result;
}

inline ByteV intToByteV(ssize_t i, uint8_t bytesize, bool bigendian)
{
	ssize_t i_max_plus_1 = (ssize_t)1 << (bytesize*8-1);
	if (i < i_max_plus_1) {
		return uIntToByteV(i, bytesize, bigendian);
	} else {
		return uIntToByteV(i_max_plus_1 + i_max_plus_1 - i, bytesize, bigendian);
	}
}

inline ByteV int16ToByteV(int16_t i, bool bigendian)
{
	return uInt16ToByteV((uint16_t)i, bigendian);
}

inline void int16ToCStr(int16_t i, uint8_t* c_str, bool bigendian)
{
	uInt16ToCStr((uint16_t)i, c_str, bigendian);
}

inline ByteV int32ToByteV(int32_t i, bool bigendian)
{
	return uInt32ToByteV((uint32_t)i, bigendian);
}

inline void int32ToCStr(int32_t i, uint8_t* c_str, bool bigendian)
{
	uInt32ToCStr((uint32_t)i, c_str, bigendian);
}

inline ByteV matrix3ToBytes(Matrix3 const& m, bool bigendian)
{
	ByteV result;
	result.reserve(4*9);
	for (uint8_t cell_id = 0; cell_id < 9; cell_id ++) {
		result += floatToByteV(m.cell(cell_id), bigendian);
	}
	return result;
}

inline ByteV matrix4ToBytes(Matrix4 const& m, bool bigendian)
{
	ByteV result;
	result.reserve(4*16);
	for (uint8_t cell_id = 0; cell_id < 16; cell_id ++) {
		result += floatToByteV(m.cell(cell_id), bigendian);
	}
	return result;
}

inline std::string sizeToStr(size_t i)
{
	std::ostringstream i_strm;
	i_strm << i;
	return std::string(i_strm.str());
}

inline std::string ssizeToStr(ssize_t i)
{
	std::ostringstream i_strm;
	i_strm << i;
	return std::string(i_strm.str());
}

inline float strToFloat(std::string const& str)
{
	return static_cast< float >(atof(str.c_str()));
}

inline Vector2 strToVector2(std::string const& str)
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
	Vector2 result;
	result.x = strToFloat(str.substr(v_begin, comma));
	result.y = strToFloat(str.substr(comma+1, v_end-comma-1));
	return result;
}

inline Vector3 strToVector3(std::string const& str)
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
	Vector3 result;
	result.x = strToFloat(str.substr(v_begin, first_comma));
	result.y = strToFloat(str.substr(first_comma+1, second_comma-first_comma-1));
	result.z = strToFloat(str.substr(second_comma+1, v_end-second_comma-1));
	return result;
}

inline size_t strToSize(std::string const& str)
{
	return atoll(str.c_str());
}

inline ssize_t strToSSize(std::string const& str)
{
	return atoll(str.c_str());
}

inline ByteV uInt16ToByteV(uint16_t i, bool bigendian)
{
	ByteV result;
	result.reserve(2);
	if (bigendian) {
		result.push_back(static_cast< uint8_t >(i >> 8));
		result.push_back(static_cast< uint8_t >(i >> 0));
	} else {
		result.push_back(static_cast< uint8_t >(i >> 0));
		result.push_back(static_cast< uint8_t >(i >> 8));
	}
	return result;
}

inline void uInt16ToCStr(uint16_t i, uint8_t* c_str, bool bigendian)
{
	if (bigendian) {
		c_str[1] = static_cast< uint8_t >(i >> 0);
		c_str[0] = static_cast< uint8_t >(i >> 8);
	} else {
		c_str[0] = static_cast< uint8_t >(i >> 0);
		c_str[1] = static_cast< uint8_t >(i >> 8);
	}
}

inline ByteV uInt32ToByteV(uint32_t i, bool bigendian)
{
	ByteV result;
	result.reserve(4);
	if (bigendian) {
		result.push_back(static_cast< uint8_t >(i >> 24));
		result.push_back(static_cast< uint8_t >(i >> 16));
		result.push_back(static_cast< uint8_t >(i >> 8));
		result.push_back(static_cast< uint8_t >(i >> 0));
	} else {
		result.push_back(static_cast< uint8_t >(i >> 0));
		result.push_back(static_cast< uint8_t >(i >> 8));
		result.push_back(static_cast< uint8_t >(i >> 16));
		result.push_back(static_cast< uint8_t >(i >> 24));
	}
	return result;
}

inline void uInt32ToCStr(uint32_t i, uint8_t* c_str, bool bigendian)
{
	if (bigendian) {
		c_str[3] = static_cast< uint8_t >(i >> 0);
		c_str[2] = static_cast< uint8_t >(i >> 8);
		c_str[1] = static_cast< uint8_t >(i >> 16);
		c_str[0] = static_cast< uint8_t >(i >> 24);
	} else {
		c_str[0] = static_cast< uint8_t >(i >> 0);
		c_str[1] = static_cast< uint8_t >(i >> 8);
		c_str[2] = static_cast< uint8_t >(i >> 16);
		c_str[3] = static_cast< uint8_t >(i >> 24);
	}
}

inline ByteV uIntToByteV(size_t i, uint8_t bytesize, bool bigendian)
{
	ByteV result;
	result.reserve(bytesize);
	if (bigendian) {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			result.push_back(static_cast< uint8_t >(i >> (8 * (bytesize - 1 - byte))));
		}
	} else {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			result.push_back(static_cast< uint8_t >(i >> (8 * byte)));
		}
	}
	return result;
}

inline void uIntToCStr(size_t i, uint8_t* c_str, uint8_t bytesize, bool bigendian)
{
	if (bigendian) {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			c_str[byte] = static_cast< uint8_t >(i >> (8 * (bytesize - 1 - byte)));
		}
	} else {
		for (uint8_t byte = 0; byte < bytesize; byte ++) {
			c_str[byte] = static_cast< uint8_t >(i >> (8 * byte));
		}
	}
}

}

#endif
