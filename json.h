#ifndef HPP_JSON_H
#define HPP_JSON_H

// This class can be used to encode and decode JSON.
//
// It can also be used to represent other than JSON types, by using Userdata.
// However, since userdata is not part of JSON standard, no JSON string can be
// decoded so it would contain any userdata.

#include "bytev.h"
#include "exception.h"

#include <map>
#include <vector>
#include <cstdlib>

namespace Hpp
{

class Json
{

public:

	enum Type { NUMBER, STRING, BOOLEAN, OBJECT, ARRAY, NUL, USERDATA };

	typedef std::map< std::string, Json > Object;
	typedef std::vector< Json > Array;

	inline Json(void);
	inline Json(std::string const& json);

	// Copy constructor and assignment operator
	inline Json(Json const& json);
	inline Json const& operator=(Json const& json);

	inline void decode(std::string const& json);
	inline std::string encode(bool nice = false) const;

	inline Type getType(void) const;
	inline bool isNumberInteger(void) const;

	// Constructors for new JSON variables
	inline static Json newObject(Object const& obj = Object());
	inline static Json newArray(Array const& arr = Array());
	inline static Json newString(std::string const& str = "");
	inline static Json newNumber(double num = 0);
	inline static Json newNumber(int64_t num = 0);
	inline static Json newNumber(uint64_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(int32_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(uint32_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(int16_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(uint16_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(int8_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newNumber(uint8_t num = 0) { return newNumber(int64_t(num)); }
	inline static Json newBoolean(bool value = false);
	inline static Json newNull(void);
	inline static Json newUserdata(ssize_t type, void* data);
	inline static Json newUserdata(void* data);

	// Setters
	inline void setMember(std::string const& key, Json const& var);
	inline void addItem(Json const& var);

	// Getters
	inline double getNumber(void) const;
	inline ssize_t getInteger(void) const;
	inline std::string getString(void) const;
	inline Object getObject(void) const;
	inline bool keyExists(std::string const& key) const;
	inline Json getMember(std::string const& key) const;
	inline Array getArray(void) const;
	inline size_t getArraySize(void) const;
	Json getItem(size_t index) const;
	inline bool getBoolean(void) const;
	inline ssize_t getUserdataType(void) const;
	inline void* getUserdata(void) const;

	// For iterating Object. If there is no more keys,
	// then key that is not found is returned.
	inline std::string getFirstKey(void) const;
	std::string getNextKey(std::string const& key) const;

	// Comparison operators
	inline bool operator==(Json const& json) const;
	inline bool operator!=(Json const& json) const;

private:

	Type type;

	double num;
	ssize_t num_i;
	bool num_is_integer;
	std::string str;
	Object obj;
	Array arr;
	void* userdata;

	std::string doEncode(size_t indent, bool nice = false) const;

	inline void clear(void);

	void loadRecursively(std::string::const_iterator& it, std::string::const_iterator end);

	inline static void skipWhitespace(std::string::const_iterator& it, std::string::const_iterator end);

	inline static std::string readString(std::string::const_iterator& it, std::string::const_iterator end);

};

inline Json::Json(void) :
type(NUL)
{
}

inline Json::Json(std::string const& json) :
type(NUL)
{
	decode(json);
}

inline Json::Json(Json const& json) :
type(json.type)
{
	switch (type) {
	case NUMBER:
		num = json.num;
		num_i = json.num_i;
	case BOOLEAN:
		num_is_integer = json.num_is_integer;
		break;
	case STRING:
		str = json.str;
		break;
	case OBJECT:
		obj = json.obj;
		break;
	case ARRAY:
		arr = json.arr;
		break;
	case USERDATA:
		num_i = json.num_i;
		userdata = json.userdata;
		break;
	default:
		break;
	}
}

inline Json const& Json::operator=(Json const& json)
{
	clear();
	type = json.type;
	switch (type) {
	case NUMBER:
		num = json.num;
		num_i = json.num_i;
	case BOOLEAN:
		num_is_integer = json.num_is_integer;
		break;
	case STRING:
		str = json.str;
		break;
	case OBJECT:
		obj = json.obj;
		break;
	case ARRAY:
		arr = json.arr;
		break;
	case USERDATA:
		num_i = json.num_i;
		userdata = json.userdata;
		break;
	default:
		break;
	}
	return *this;
}

inline void Json::decode(std::string const& json)
{
	std::string::const_iterator json_it = json.begin();
	loadRecursively(json_it, json.end());
	if (type != OBJECT && type != ARRAY) {
		clear();
		type = NUL;
		throw Exception("JSON must be either object or array!");
	}
}

inline std::string Json::encode(bool nice) const
{
	return doEncode(0, nice);
}

inline Json::Type Json::getType(void) const
{
	return type;
}

inline bool Json::isNumberInteger(void) const
{
	if (type != NUMBER) {
		throw Exception("isNumberInteger() can be only called when type is number!");
	}
	return num_is_integer;
}

inline Json Json::newObject(Object const& obj)
{
	Json result;
	result.type = OBJECT;
	result.obj = obj;
	return result;
}

inline Json Json::newArray(Array const& arr)
{
	Json result;
	result.type = ARRAY;
	result.arr = arr;
	return result;
}

inline Json Json::newString(std::string const& str)
{
	Json result;
	result.type = STRING;
	result.str = str;
	return result;
}

inline Json Json::newNumber(double num)
{
	Json result;
	result.type = NUMBER;
	result.num = num;
	result.num_i = num + 0.5;
	result.num_is_integer = false;
	return result;
}

inline Json Json::newNumber(int64_t num)
{
	Json result;
	result.type = NUMBER;
	result.num = num;
	result.num_i = num;
	result.num_is_integer = true;
	return result;
}

inline Json Json::newBoolean(bool value)
{
	Json result;
	result.type = BOOLEAN;
	result.num_is_integer = value;
	return result;
}

inline Json Json::newNull(void)
{
	Json result;
	return result;
}

inline Json Json::newUserdata(ssize_t type, void* data)
{
	Json result;
	result.type = USERDATA;
	result.num_i = type;
	result.userdata = data;
	return result;
}

inline Json Json::newUserdata(void* data)
{
	Json result;
	result.type = USERDATA;
	result.num_i = 0;
	result.userdata = data;
	return result;
}

inline void Json::setMember(std::string const& key, Json const& var)
{
	if (type != OBJECT) {
		throw Exception("Unable to set member, because this JSON is not an object!");
	}
	obj[key] = var;
}

inline void Json::addItem(Json const& var)
{
	if (type != ARRAY) {
		throw Exception("Unable to add item, because this JSON is not an array!");
	}
	arr.push_back(var);
}

inline double Json::getNumber(void) const
{
	if (type != NUMBER) {
		throw Exception("Unable to get number, because this JSON is not a number!");
	}
	return num;
}

inline ssize_t Json::getInteger(void) const
{
	if (type != NUMBER) {
		throw Exception("Unable to get integer, because this JSON is not a number!");
	}
	return num_i;
}

inline std::string Json::getString(void) const
{
	if (type != STRING) {
		throw Exception("Unable to get string, because this JSON is not a string!");
	}
	return str;
}

inline Json::Object Json::getObject(void) const
{
	if (type != OBJECT) {
		throw Exception("Unable to get object, because this JSON is not an object!");
	}
	return obj;
}

inline bool Json::keyExists(std::string const& key) const
{
	if (type != OBJECT) {
		throw Exception("Unable to check if key \"" + key + "\" exists, because this JSON is not an object!");
	}
	return obj.find(key) != obj.end();
}

inline Json Json::getMember(std::string const& key) const
{
	if (type != OBJECT) {
		throw Exception("Unable to get member \"" + key + "\", because this JSON is not an object!");
	}
	Object::const_iterator obj_find = obj.find(key);
	if (obj_find == obj.end()) {
		throw Exception("Member \"" + key + "\" does not exist!");
	}
	return obj_find->second;
}

inline Json::Array Json::getArray(void) const
{
	if (type != ARRAY) {
		throw Exception("Unable to get array, because this JSON is not an array!");
	}
	return arr;
}

inline size_t Json::getArraySize(void) const
{
	if (type != ARRAY) {
		throw Exception("Unable to get size of array, because this JSON is not an array!");
	}
	return arr.size();
}

inline bool Json::getBoolean(void) const
{
	if (type != BOOLEAN) {
		throw Exception("Unable to get boolean, because this JSON is not a boolean!");
	}
	return num_is_integer;
}

inline ssize_t Json::getUserdataType(void) const
{
	if (type != USERDATA) {
		throw Exception("Unable to get type of userdata, because this JSON is not userdata!");
	}
	return num_i;
}

inline void* Json::getUserdata(void) const
{
	if (type != USERDATA) {
		throw Exception("Unable to get userdata, because this JSON is not userdata!");
	}
	return userdata;
}

inline std::string Json::getFirstKey(void) const
{
	if (type != OBJECT) {
		throw Exception("Unable to get first key, because this JSON is not an object!");
	}
	if (obj.empty()) {
		return "";
	}
	return obj.begin()->first;
}

inline bool Json::operator==(Json const& json) const
{
	if (type != json.type) {
		return false;
	}

	switch (type) {
	case NUMBER:
		if (num_is_integer != json.num_is_integer) return false;
		if (num_is_integer) {
			return num_i == json.num_i;
		} else {
			return num == json.num;
		}
	case BOOLEAN:
		return num_is_integer == json.num_is_integer;
	case STRING:
		return str == json.str;
	case OBJECT:
		return obj == json.obj;
	case ARRAY:
		return arr == json.arr;
	case USERDATA:
		return num_i == json.num_i && userdata == json.userdata;
	default:
		break;
	}

	return true;
}

inline bool Json::operator!=(Json const& json) const
{
	return !(*this == json);
}

inline void Json::clear(void)
{
	switch (type) {
	case STRING:
		str.clear();
		break;
	case OBJECT:
		obj.clear();
		break;
	case ARRAY:
		arr.clear();
		break;
	default:
		break;
	}
}

inline void Json::skipWhitespace(std::string::const_iterator& it, std::string::const_iterator end)
{
	while (it != end && (*it == ' ' || *it == '\t' || *it == '\n')) {
		++ it;
	}
}

inline std::string Json::readString(std::string::const_iterator& it, std::string::const_iterator end)
{
	if (*it != '\"') {
		throw Exception(std::string("Expected \"\"\" but got \"") + *it + "\"!");
	}
	++ it;
	if (it == end) {
		throw Exception("JSON string ended prematurely!");
	}
	std::string result;
	while (*it != '\"') {
		if (*it == '\\') {
			++ it;
			if (it == end) {
				throw Exception("JSON string ended prematurely!");
			}
			if (*it == '\\') {
				result += '\\';
			} else if (*it == '\"') {
				result += '\"';
			} else if (*it == 'n') {
				result += '\n';
			} else if (*it == 't') {
				result += '\t';
			} else {
				throw Exception(std::string("Escaped \"") + *it + "\" is unexpected in JSON string!");
			}
			++ it;
		} else {
			result += *it;
		}
		++ it;
		if (it == end) {
			throw Exception("JSON string ended prematurely!");
		}
	}
	++ it;
	return result;
}

}

#endif
