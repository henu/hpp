#ifndef HPP_SERIALIZE_H
#define HPP_SERIALIZE_H

#include "bytev.h"
#include "assert.h"
#include "exception.h"
#include "cast.h"

#include <istream>

// TODO: Use (u)int64_t instead of (u)int32_t in casting functions with variable bytesize!
namespace Hpp
{

inline void serializeString(ByteV& result, std::string const& str, uint8_t bytes, bool bigendian = true);

inline int8_t deserializeInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline int16_t deserializeInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline int32_t deserializeInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline int64_t deserializeInt64(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline int32_t deserializeInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize, bool bigendian = true);
inline uint8_t deserializeUInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline uint16_t deserializeUInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline uint32_t deserializeUInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline uint64_t deserializeUInt64(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline uint32_t deserializeUInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize, bool bigendian = true);
inline float deserializeFloat(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian = true);
inline std::string deserializeString(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytes, bool bigendian = true);
inline ByteV deserializeByteV(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, size_t size);

inline int8_t deserializeInt8(std::istream& strm);
inline int16_t deserializeInt16(std::istream& strm, bool bigendian = true);
inline int32_t deserializeInt32(std::istream& strm, bool bigendian = true);
inline int64_t deserializeInt64(std::istream& strm, bool bigendian = true);
inline uint8_t deserializeUInt8(std::istream& strm);
inline uint16_t deserializeUInt16(std::istream& strm, bool bigendian = true);
inline uint32_t deserializeUInt32(std::istream& strm, bool bigendian = true);
inline uint64_t deserializeUInt64(std::istream& strm, bool bigendian = true);
inline float deserializeFloat(std::istream& strm, bool bigendian = true);
inline std::string deserializeString(std::istream& strm, uint8_t bytes, bool bigendian = true);
inline ByteV deserializeByteV(std::istream& strm, size_t size);

inline void serializeString(ByteV& result, std::string const& str, uint8_t bytes, bool bigendian)
{
	if (bytes == 1) {
		HppAssert(str.size() <= 0xff, "String is too long!");
		result.push_back(str.size());
	} else if (bytes == 2) {
		HppAssert(str.size() <= 0xffff, "String is too long!");
		result += uInt16ToByteV(str.size(), bigendian);
	} else {
		HppAssert(bytes == 4, "Unsupported byte count!");
		HppAssert(str.size() <= 0xffffffff, "String is too long!");
		result += uInt32ToByteV(str.size(), bigendian);
	}
	result += str;
}

inline int8_t deserializeInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 1) {
		throw Exception("Unexpected end of data!");
	}
	int8_t result = (int8_t)*data_it;
	data_it ++;
	return result;
}

inline int16_t deserializeInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 2) {
		throw Exception("Unexpected end of data!");
	}
	int16_t result = cStrToInt16(&*data_it, bigendian);
	data_it += 2;
	return result;
}

inline int32_t deserializeInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	int32_t result = cStrToInt32(&*data_it, bigendian);
	data_it += 4;
	return result;
}

inline int64_t deserializeInt64(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 8) {
		throw Exception("Unexpected end of data!");
	}
	int64_t result = cStrToInt64(&*data_it, bigendian);
	data_it += 8;
	return result;
}

inline int32_t deserializeInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize, bool bigendian)
{
	if (data_end - data_it < bytesize) {
		throw Exception("Unexpected end of data!");
	}
	int32_t result = cStrToInt(&*data_it, bytesize, bigendian);
	data_it += bytesize;
	return result;
}

inline uint8_t deserializeUInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 1) {
		throw Exception("Unexpected end of data!");
	}
	uint8_t result = *data_it;
	data_it ++;
	return result;
}

inline uint16_t deserializeUInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 2) {
		throw Exception("Unexpected end of data!");
	}
	uint16_t result = cStrToUInt16(&*data_it, bigendian);
	data_it += 2;
	return result;
}

inline uint32_t deserializeUInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result = cStrToUInt32(&*data_it, bigendian);
	data_it += 4;
	return result;
}

inline uint64_t deserializeUInt64(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 8) {
		throw Exception("Unexpected end of data!");
	}
	uint64_t result = cStrToUInt64(&*data_it, bigendian);
	data_it += 8;
	return result;
}

inline uint32_t deserializeUInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize, bool bigendian)
{
	if (data_end - data_it < bytesize) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result = cStrToUInt(&*data_it, bytesize, bigendian);
	data_it += bytesize;
	return result;
}

inline float deserializeFloat(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, bool bigendian)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	float result = cStrToFloat(&*data_it, bigendian);
	data_it += 4;
	return result;
}

inline std::string deserializeString(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytes, bool bigendian)
{
	if (data_end - data_it < bytes) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result_size = cStrToUInt(&*data_it, bytes, bigendian);
	data_it += bytes;
	if (data_end - data_it < result_size) {
		throw Exception("Unexpected end of data!");
	}
	std::string result = std::string((char const*)&*data_it, result_size);
	data_it += result_size;
	return result;
}

inline ByteV deserializeByteV(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, size_t size)
{
	if (data_end - data_it < (ssize_t)size) {
		throw Exception("Unexpected end of data!");
	}
	ByteV result(data_it, data_it + size);
	data_it += size;
	return result;
}

inline int8_t deserializeInt8(std::istream& strm)
{
	int8_t result = strm.get();
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return result;
}

inline int16_t deserializeInt16(std::istream& strm, bool bigendian)
{
	char buf[2];
	strm.read(buf, 2);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToInt16(buf, bigendian);
}

inline int32_t deserializeInt32(std::istream& strm, bool bigendian)
{
	char buf[4];
	strm.read(buf, 4);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToInt32(buf, bigendian);
}

inline int64_t deserializeInt64(std::istream& strm, bool bigendian)
{
	char buf[8];
	strm.read(buf, 8);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToInt64(buf, bigendian);
}

inline uint8_t deserializeUInt8(std::istream& strm)
{
	uint8_t result = strm.get();
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return result;
}

inline uint16_t deserializeUInt16(std::istream& strm, bool bigendian)
{
	char buf[2];
	strm.read(buf, 2);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToUInt16(buf, bigendian);
}

inline uint32_t deserializeUInt32(std::istream& strm, bool bigendian)
{
	char buf[4];
	strm.read(buf, 4);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToUInt32(buf, bigendian);
}

inline uint64_t deserializeUInt64(std::istream& strm, bool bigendian)
{
	char buf[8];
	strm.read(buf, 8);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToUInt64(buf, bigendian);
}

inline float deserializeFloat(std::istream& strm, bool bigendian)
{
	char buf[4];
	strm.read(buf, 4);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return cStrToFloat(buf, bigendian);
}

inline std::string deserializeString(std::istream& strm, uint8_t bytes, bool bigendian)
{
	uint8_t buf[8];
	if (bytes > 8) {
		throw Exception("Unsupported byte count!");
	}
	strm.read((char*)buf, bytes);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result_size = cStrToUInt(buf, bytes, bigendian);
	std::string result;
	result.reserve(result_size);
	while (result.size() < result_size) {
// TODO: Read to buffer!
		result += (char)strm.get();
	}
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return result;
}

inline ByteV deserializeByteV(std::istream& strm, size_t size)
{
	ByteV result;
	result.assign(size, 0);
	strm.read((char*)&result[0], size);
	if (strm.eof()) {
		throw Exception("Unexpected end of data!");
	}
	return result;
}

}

#endif
