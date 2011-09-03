#ifndef HPP_SERIALIZE_H
#define HPP_SERIALIZE_H

#include "bytev.h"
#include "assert.h"
#include "exception.h"
#include "vector2.h"
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"
#include "cast.h"

#include <istream>

namespace Hpp
{

inline void serializeString(ByteV& result, std::string const& str, uint8_t bytes);

inline int8_t deserializeInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline int16_t deserializeInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline int32_t deserializeInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline int32_t deserializeInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize);
inline uint8_t deserializeUInt8(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline uint16_t deserializeUInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline uint32_t deserializeUInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline uint32_t deserializeUInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize);
inline float deserializeFloat(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline std::string deserializeString(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytes);
inline Vector2 deserializeVector2(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline Vector3 deserializeVector3(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline Matrix3 deserializeMatrix3(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);
inline Matrix4 deserializeMatrix4(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end);

inline uint8_t deserializeUInt8(std::istream& strm);
inline uint16_t deserializeUInt16(std::istream& strm);
inline uint32_t deserializeUInt32(std::istream& strm);
inline float deserializeFloat(std::istream& strm);
inline std::string deserializeString(std::istream& strm, uint8_t bytes);
inline Vector2 deserializeVector2(std::istream& strm);
inline Vector3 deserializeVector3(std::istream& strm);
inline Matrix3 deserializeMatrix3(std::istream& strm);
inline Matrix4 deserializeMatrix4(std::istream& strm);

inline void serializeString(ByteV& result, std::string const& str, uint8_t bytes)
{
	if (bytes == 1) {
		HppAssert(str.size() <= 0xff, "String is too long!");
		result.push_back(str.size());
	} else if (bytes == 2) {
		HppAssert(str.size() <= 0xffff, "String is too long!");
		result += uInt16ToByteV(str.size());
	} else {
		HppAssert(bytes == 4, "Unsupported byte count!");
		HppAssert(str.size() <= 0xffffffff, "String is too long!");
		result += uInt32ToByteV(str.size());
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

inline int16_t deserializeInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 2) {
		throw Exception("Unexpected end of data!");
	}
	int16_t result = cStrToInt16(&*data_it);
	data_it += 2;
	return result;
}

inline int32_t deserializeInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	int32_t result = cStrToInt32(&*data_it);
	data_it += 4;
	return result;
}

inline int32_t deserializeInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize)
{
	if (data_end - data_it < bytesize) {
		throw Exception("Unexpected end of data!");
	}
	int32_t result = cStrToInt(&*data_it, bytesize);
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

inline uint16_t deserializeUInt16(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 2) {
		throw Exception("Unexpected end of data!");
	}
	uint16_t result = cStrToUInt16(&*data_it);
	data_it += 2;
	return result;
}

inline uint32_t deserializeUInt32(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result = cStrToUInt32(&*data_it);
	data_it += 4;
	return result;
}

inline uint32_t deserializeUInt(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytesize)
{
	if (data_end - data_it < bytesize) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result = cStrToUInt(&*data_it, bytesize);
	data_it += bytesize;
	return result;
}

inline float deserializeFloat(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	if (data_end - data_it < 4) {
		throw Exception("Unexpected end of data!");
	}
	float result = cStrToFloat(&*data_it);
	data_it += 4;
	return result;
}

inline std::string deserializeString(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end, uint8_t bytes)
{
	if (data_end - data_it < bytes) {
		throw Exception("Unexpected end of data!");
	}
	uint32_t result_size;
	if (bytes == 1) {
		result_size = *data_it;
	} else if (bytes == 2) {
		result_size = cStrToUInt16(&*data_it);
	} else {
		HppAssert(bytes == 4, "Unsupported byte count!");
		result_size = cStrToUInt32(&*data_it);
	}
	data_it += bytes;
	std::string result = std::string((char const*)&*data_it, result_size);
	data_it += result_size;
	return result;
}

inline Vector2 deserializeVector2(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	Vector2 result;
	result.x = deserializeFloat(data_it, data_end);
	result.y = deserializeFloat(data_it, data_end);
	return result;
}

inline Vector3 deserializeVector3(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	Vector3 result;
	result.x = deserializeFloat(data_it, data_end);
	result.y = deserializeFloat(data_it, data_end);
	result.z = deserializeFloat(data_it, data_end);
	return result;
}

inline Matrix3 deserializeMatrix3(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	Matrix3 result;
	for (uint8_t cell_id = 0; cell_id < 9; cell_id ++) {
		result.cell(cell_id) = deserializeFloat(data_it, data_end);
	}
	return result;
}

inline Matrix4 deserializeMatrix4(ByteV::const_iterator& data_it, ByteV::const_iterator const& data_end)
{
	Matrix4 result;
	for (uint8_t cell_id = 0; cell_id < 16; cell_id ++) {
		result.cell(cell_id) = deserializeFloat(data_it, data_end);
	}
	return result;
}

inline uint8_t deserializeUInt8(std::istream& strm)
{
	uint8_t result = strm.get();
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	return result;
}

inline uint16_t deserializeUInt16(std::istream& strm)
{
	char buf[2];
	strm.read(buf, 2);
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	return cStrToUInt16(buf);
}

inline uint32_t deserializeUInt32(std::istream& strm)
{
	char buf[4];
	strm.read(buf, 4);
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	return cStrToUInt32(buf);
}

inline float deserializeFloat(std::istream& strm)
{
	char buf[4];
	strm.read(buf, 4);
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	return cStrToFloat(buf);
}

inline std::string deserializeString(std::istream& strm, uint8_t bytes)
{
	uint32_t result_size;
	if (bytes == 1) {
		result_size = strm.get();
	} else if (bytes == 2) {
		char buf[2];
		strm.read(buf, 2);
		result_size = cStrToUInt16(buf);
	} else {
		HppAssert(bytes == 4, "Unsupported byte count!");
		char buf[4];
		strm.read(buf, 4);
		result_size = cStrToUInt32(buf);
	}
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	std::string result;
	result.reserve(result_size);
	while (result.size() < result_size) {
		result += (char)strm.get();
	}
	if (strm.eof()) {
		throw Exception("Unexpected data end!");
	}
	return result;
}

inline Vector2 deserializeVector2(std::istream& strm)
{
	Vector2 result;
	result.x = deserializeFloat(strm);
	result.y = deserializeFloat(strm);
	return result;
}

inline Vector3 deserializeVector3(std::istream& strm)
{
	Vector3 result;
	result.x = deserializeFloat(strm);
	result.y = deserializeFloat(strm);
	result.z = deserializeFloat(strm);
	return result;
}

inline Matrix3 deserializeMatrix3(std::istream& strm)
{
	Matrix3 result;
	for (uint8_t cell_id = 0; cell_id < 9; cell_id ++) {
		result.cell(cell_id) = deserializeFloat(strm);
	}
	return result;
}

inline Matrix4 deserializeMatrix4(std::istream& strm)
{
	Matrix4 result;
	for (uint8_t cell_id = 0; cell_id < 16; cell_id ++) {
		result.cell(cell_id) = deserializeFloat(strm);
	}
	return result;
}

}

#endif
