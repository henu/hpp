#ifndef HPP_CAST_H
#define HPP_CAST_H

#include "assert.h"
#include "bytev.h"
#include "exception.h"

#include <sstream>
#include <cstdlib>

namespace Hpp
{

inline std::string	byteVToDStr(ByteV const& v);
inline std::string	byteVToHexV(ByteV const& bytev);
inline std::string	byteVToStr(ByteV const& v);
inline float		cStrToFloat(uint8_t const* c_str);
inline int32_t		cStrToInt(uint8_t const* c_str, uint8_t bytesize);
inline int16_t		cStrToInt16(uint8_t const* c_str);
inline int32_t		cStrToInt32(uint8_t const* c_str);
inline uint32_t		cStrToUInt(uint8_t const* c_str, uint8_t bytesize);
inline uint16_t		cStrToUInt16(uint8_t const* c_str);
inline uint32_t		cStrToUInt32(uint8_t const* c_str);
inline float		cStrToFloat(char const* c_str) { return cStrToFloat((uint8_t const*)c_str); }
inline int16_t		cStrToInt16(char const* c_str) { return cStrToInt16((uint8_t const*)c_str); }
inline int32_t		cStrToInt32(char const* c_str) { return cStrToInt32((uint8_t const*)c_str); }
inline uint16_t		cStrToUInt16(char const* c_str) { return cStrToUInt16((uint8_t const*)c_str); }
inline uint32_t		cStrToUInt32(char const* c_str) { return cStrToUInt32((uint8_t const*)c_str); }
inline ByteV		dStrToByteV(std::string const& dstr);
inline ByteV		floatToByteV(float f);
inline void		floatToCStr(float f, uint8_t* c_str);
inline std::string	floatToStr(float f);
inline ByteV		hexVToByteV(std::string const& hexv);
inline ByteV		int16ToByteV(int16_t i);
inline void		int16ToCStr(int16_t i, uint8_t* c_str);
inline ByteV		int32ToByteV(int32_t i);
inline void		int32ToCStr(int32_t i, uint8_t* c_str);
inline std::string 	sizeToStr(size_t i);
inline std::string	ssizeToStr(ssize_t i);
inline float		strToFloat(std::string str);
inline size_t		strToSize(std::string const& str);
inline ssize_t		strToSSize(std::string const& str);
inline ByteV		uInt16ToByteV(uint16_t i);
inline void		uInt16ToCStr(uint16_t i, uint8_t* c_str);
inline ByteV		uInt32ToByteV(uint32_t i);
inline void		uInt32ToCStr(uint32_t i, uint8_t* c_str);
inline ByteV		uIntToByteV(uint32_t i, uint8_t bytesize);
inline void		uIntToCStr(uint32_t i, uint8_t* c_str, uint8_t bytesize);

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

inline float cStrToFloat(uint8_t const* c_str)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
// TODO: The next thing may put debuggers crazy, as the f might be consider uninitialized
	float f;
	uint8_t* f_cp = (uint8_t*)(&f);
	f_cp[0] = c_str[3];
	f_cp[1] = c_str[2];
	f_cp[2] = c_str[1];
	f_cp[3] = c_str[0];
	return f;
}

inline int32_t cStrToInt(uint8_t const* c_str, uint8_t bytesize)
{
	int32_t i = 0;
	for (uint8_t byte = 0; byte < bytesize - 1; byte ++) {
		i += c_str[byte] << (8 * byte);
	}
	i += (c_str[bytesize - 1] & 0x7F) << (8 * (bytesize - 1));
	if (c_str[bytesize - 1] & 0x80) {
		i = -i;
	}
	return i;
}

inline int16_t cStrToInt16(uint8_t const* c_str)
{
	int16_t i = 0;
	i += c_str[0] << 0;
	i += (c_str[1] & 0x7F) << 8;
	if (c_str[1] & 0x80) {
		i = -i;
	}
	return i;
}

inline int32_t cStrToInt32(uint8_t const* c_str)
{
	int32_t i = 0;
	i += c_str[0] << 0;
	i += c_str[1] << 8;
	i += c_str[2] << 16;
	i += (c_str[3] & 0x7F) << 24;
	if (c_str[3] & 0x80) {
		i = -i;
	}
	return i;
}

inline uint32_t cStrToUInt(uint8_t const* c_str, uint8_t bytesize)
{
	uint32_t i = 0;
	for (uint8_t byte = 0; byte < bytesize; byte ++) {
		i += c_str[byte] << (8 * byte);
	}
	return i;
}

inline uint16_t cStrToUInt16(uint8_t const* c_str)
{
	uint16_t i = 0;
	i += c_str[0] << 0;
	i += c_str[1] << 8;
	return i;
}

inline uint32_t cStrToUInt32(uint8_t const* c_str)
{
	uint32_t i = 0;
	i += c_str[0] << 0;
	i += c_str[1] << 8;
	i += c_str[2] << 16;
	i += c_str[3] << 24;
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

inline ByteV floatToByteV(float f)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
	uint8_t* f_cp = (uint8_t*)(&f);
	ByteV result;
	result.reserve(4);
	result.push_back(f_cp[3]);
	result.push_back(f_cp[2]);
	result.push_back(f_cp[1]);
	result.push_back(f_cp[0]);
	return result;
}

inline void floatToCStr(float f, uint8_t* c_str)
{
// TODO: Fix this! Now this works only system that has 32bit floats and versions between different architectures may not be compatible even if they both have 32bit floats
	uint8_t* f_cp = (uint8_t*)(&f);
	c_str[0] = f_cp[3];
	c_str[1] = f_cp[2];
	c_str[2] = f_cp[1];
	c_str[3] = f_cp[0];
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

inline ByteV int16ToByteV(int16_t i)
{
	uint16_t ui;
	bool negative = i < 0;
	if (negative) {
		ui = -i;
	} else {
		ui = i;
	}
	ByteV result;
	result.reserve(2);
	result.push_back(static_cast< uint8_t >(ui >> 0));
	result.push_back(static_cast< uint8_t >(((ui >> 8) & 0x7F) + (negative << 7)));
	return result;
}

inline void int16ToCStr(int16_t i, uint8_t* c_str)
{
	uint16_t ui;
	bool negative = i < 0;
	if (negative) {
		ui = -i;
	} else {
		ui = i;
	}
	c_str[0] = static_cast< uint8_t >(ui >> 0);
	c_str[1] = static_cast< uint8_t >(((ui >> 8) & 0x7F) + (negative << 7));
}

inline ByteV int32ToByteV(int32_t i)
{
	uint32_t ui;
	bool negative = i < 0;
	if (negative) {
		ui = -i;
	} else {
		ui = i;
	}
	ByteV result;
	result.reserve(4);
	result.push_back(static_cast< uint8_t >(ui >> 0));
	result.push_back(static_cast< uint8_t >(ui >> 8));
	result.push_back(static_cast< uint8_t >(ui >> 16));
	result.push_back(static_cast< uint8_t >(((ui >> 24) & 0x7F) + (negative << 7)));
	return result;
}

inline void int32ToCStr(int32_t i, uint8_t* c_str)
{
	uint32_t ui;
	bool negative = i < 0;
	if (negative) {
		ui = -i;
	} else {
		ui = i;
	}
	c_str[0] = static_cast< uint8_t >(ui >> 0);
	c_str[1] = static_cast< uint8_t >(ui >> 8);
	c_str[2] = static_cast< uint8_t >(ui >> 16);
	c_str[3] = static_cast< uint8_t >(((ui >> 24) & 0x7F) + (negative << 7));
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

inline float strToFloat(std::string str)
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

inline ByteV uInt16ToByteV(uint16_t i)
{
	ByteV result;
	result.reserve(2);
	result.push_back(static_cast< uint8_t >(i >> 0));
	result.push_back(static_cast< uint8_t >(i >> 8));
	return result;
}

inline void uInt16ToCStr(uint16_t i, uint8_t* c_str)
{
	c_str[0] = static_cast< uint8_t >(i >> 0);
	c_str[1] = static_cast< uint8_t >(i >> 8);
}

inline ByteV uInt32ToByteV(uint32_t i)
{
	ByteV result;
	result.reserve(4);
	result.push_back(static_cast< uint8_t >(i >> 0));
	result.push_back(static_cast< uint8_t >(i >> 8));
	result.push_back(static_cast< uint8_t >(i >> 16));
	result.push_back(static_cast< uint8_t >(i >> 24));
	return result;
}

inline void uInt32ToCStr(uint32_t i, uint8_t* c_str)
{
	c_str[0] = static_cast< uint8_t >(i >> 0);
	c_str[1] = static_cast< uint8_t >(i >> 8);
	c_str[2] = static_cast< uint8_t >(i >> 16);
	c_str[3] = static_cast< uint8_t >(i >> 24);
}

inline ByteV uIntToByteV(uint32_t i, uint8_t bytesize)
{
	ByteV result;
	result.reserve(bytesize);
	for (uint8_t byte = 0; byte < bytesize; byte ++) {
		result.push_back(static_cast< uint8_t >(i >> (8 * byte)));
	}
	return result;
}

inline void uIntToCStr(uint32_t i, uint8_t* c_str, uint8_t bytesize)
{
	for (uint8_t byte = 0; byte < bytesize; byte ++) {
		c_str[byte] = static_cast< uint8_t >(i >> (8 * byte));
	}
}

}

#endif
