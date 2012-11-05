#ifndef HPP_CAST_H
#define HPP_CAST_H

#include "assert.h"
#include "exception.h"
#include "bytev.h"

#include <sstream>
#include <cstdlib>

// TODO: Use (u)int64_t instead of (u)int32_t in casting functions with variable bytesize!
namespace Hpp
{

inline std::string	byteVToDStr(ByteV const& v);
inline std::string	byteVToHexV(ByteV const& bytev);
inline std::string	byteVToStr(ByteV const& v);
inline float		cStrToFloat(uint8_t const* c_str, bool bigendian = true);
inline ssize_t		cStrToInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian = true);
inline int16_t		cStrToInt16(uint8_t const* c_str, bool bigendian = true);
inline int32_t		cStrToInt32(uint8_t const* c_str, bool bigendian = true);
inline int64_t		cStrToInt64(uint8_t const* c_str, bool bigendian = true);
inline size_t		cStrToUInt(uint8_t const* c_str, uint8_t bytesize, bool bigendian = true);
inline uint16_t		cStrToUInt16(uint8_t const* c_str, bool bigendian = true);
inline uint32_t		cStrToUInt32(uint8_t const* c_str, bool bigendian = true);
inline uint64_t		cStrToUInt64(uint8_t const* c_str, bool bigendian = true);
inline float		cStrToFloat(char const* c_str, bool bigendian = true) { return cStrToFloat((uint8_t const*)c_str, bigendian); }
inline int16_t		cStrToInt16(char const* c_str, bool bigendian = true) { return cStrToInt16((uint8_t const*)c_str, bigendian); }
inline int32_t		cStrToInt32(char const* c_str, bool bigendian = true) { return cStrToInt32((uint8_t const*)c_str, bigendian); }
inline int64_t		cStrToInt64(char const* c_str, bool bigendian = true) { return cStrToInt64((uint8_t const*)c_str, bigendian); }
inline uint16_t		cStrToUInt16(char const* c_str, bool bigendian = true) { return cStrToUInt16((uint8_t const*)c_str, bigendian); }
inline uint32_t		cStrToUInt32(char const* c_str, bool bigendian = true) { return cStrToUInt32((uint8_t const*)c_str, bigendian); }
inline uint64_t		cStrToUInt64(char const* c_str, bool bigendian = true) { return cStrToUInt64((uint8_t const*)c_str, bigendian); }
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
inline ByteV		int64ToByteV(int64_t i, bool bigendian = true);
inline void		int64ToCStr(int64_t i, uint8_t* c_str, bool bigendian = true);
inline std::string 	sizeToStr(size_t i);
inline std::string	ssizeToStr(ssize_t i);
inline float		strToFloat(std::string const& str);
inline size_t		strToSize(std::string const& str);
inline ssize_t		strToSSize(std::string const& str);
inline ByteV		uInt16ToByteV(uint16_t i, bool bigendian = true);
inline void		uInt16ToCStr(uint16_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		uInt32ToByteV(uint32_t i, bool bigendian = true);
inline void		uInt32ToCStr(uint32_t i, uint8_t* c_str, bool bigendian = true);
inline ByteV		uInt64ToByteV(uint64_t i, bool bigendian = true);
inline void		uInt64ToCStr(uint64_t i, uint8_t* c_str, bool bigendian = true);
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
	     ++ v_it) {
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
	     ++ bytev_it) {
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

inline int64_t cStrToInt64(uint8_t const* c_str, bool bigendian)
{
	return (int64_t)cStrToUInt64(c_str, bigendian);
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

inline uint64_t cStrToUInt64(uint8_t const* c_str, bool bigendian)
{
	uint64_t i = 0;
	if (bigendian) {
		i += (uint64_t)c_str[7] << 0;
		i += (uint64_t)c_str[6] << 8;
		i += (uint64_t)c_str[5] << 16;
		i += (uint64_t)c_str[4] << 24;
		i += (uint64_t)c_str[3] << 32;
		i += (uint64_t)c_str[2] << 40;
		i += (uint64_t)c_str[1] << 48;
		i += (uint64_t)c_str[0] << 56;
	} else {
		i += (uint64_t)c_str[0] << 0;
		i += (uint64_t)c_str[1] << 8;
		i += (uint64_t)c_str[2] << 16;
		i += (uint64_t)c_str[3] << 24;
		i += (uint64_t)c_str[4] << 32;
		i += (uint64_t)c_str[5] << 40;
		i += (uint64_t)c_str[6] << 48;
		i += (uint64_t)c_str[7] << 56;
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
	     ++ dstr_it) {
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
	     ++ hexv_it) {
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
	     ++ hexv_it) {
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

inline ByteV int64ToByteV(int64_t i, bool bigendian)
{
	return uInt64ToByteV((uint64_t)i, bigendian);
}

inline void int64ToCStr(int64_t i, uint8_t* c_str, bool bigendian)
{
	uInt64ToCStr((uint64_t)i, c_str, bigendian);
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

inline ByteV uInt64ToByteV(uint64_t i, bool bigendian)
{
	ByteV result;
	result.reserve(8);
	if (bigendian) {
		result.push_back(static_cast< uint8_t >(i >> 56));
		result.push_back(static_cast< uint8_t >(i >> 48));
		result.push_back(static_cast< uint8_t >(i >> 40));
		result.push_back(static_cast< uint8_t >(i >> 32));
		result.push_back(static_cast< uint8_t >(i >> 24));
		result.push_back(static_cast< uint8_t >(i >> 16));
		result.push_back(static_cast< uint8_t >(i >> 8));
		result.push_back(static_cast< uint8_t >(i >> 0));
	} else {
		result.push_back(static_cast< uint8_t >(i >> 0));
		result.push_back(static_cast< uint8_t >(i >> 8));
		result.push_back(static_cast< uint8_t >(i >> 16));
		result.push_back(static_cast< uint8_t >(i >> 24));
		result.push_back(static_cast< uint8_t >(i >> 32));
		result.push_back(static_cast< uint8_t >(i >> 40));
		result.push_back(static_cast< uint8_t >(i >> 48));
		result.push_back(static_cast< uint8_t >(i >> 56));
	}
	return result;
}

inline void uInt64ToCStr(uint64_t i, uint8_t* c_str, bool bigendian)
{
	if (bigendian) {
		c_str[7] = static_cast< uint8_t >(i >> 0);
		c_str[6] = static_cast< uint8_t >(i >> 8);
		c_str[5] = static_cast< uint8_t >(i >> 16);
		c_str[4] = static_cast< uint8_t >(i >> 24);
		c_str[3] = static_cast< uint8_t >(i >> 32);
		c_str[2] = static_cast< uint8_t >(i >> 40);
		c_str[1] = static_cast< uint8_t >(i >> 48);
		c_str[0] = static_cast< uint8_t >(i >> 56);
	} else {
		c_str[0] = static_cast< uint8_t >(i >> 0);
		c_str[1] = static_cast< uint8_t >(i >> 8);
		c_str[2] = static_cast< uint8_t >(i >> 16);
		c_str[3] = static_cast< uint8_t >(i >> 24);
		c_str[4] = static_cast< uint8_t >(i >> 32);
		c_str[5] = static_cast< uint8_t >(i >> 40);
		c_str[6] = static_cast< uint8_t >(i >> 48);
		c_str[7] = static_cast< uint8_t >(i >> 56);
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
