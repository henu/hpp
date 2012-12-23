#ifndef HPP_JSON_H
#define HPP_JSON_H

#include "bytev.h"
#include "exception.h"
#include "misc.h"
#include "cast.h"

#include <map>
#include <vector>
#include <cstdlib>

namespace Hpp
{

class Json
{

public:

	enum Type { NUMBER, STRING, BOOLEAN, OBJECT, ARRAY, NUL };

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

private:

	Type type;

	double num;
	ssize_t num_i;
	bool num_is_integer;
	std::string str;
	Object obj;
	Array arr;

	std::string doEncode(size_t indent, bool nice = false) const;

	inline void clear(void);

	inline void loadRecursively(std::string::const_iterator& it, std::string::const_iterator end);

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

inline void Json::loadRecursively(std::string::const_iterator& it, std::string::const_iterator end)
{
	skipWhitespace(it, end);
	if (it == end) {
		throw Exception("Empty JSON!");
	}

	clear();

	// Object
	if (*it == '{') {
		++ it;
		type = OBJECT;
		bool expect_more = false;
		while (true) {
			skipWhitespace(it, end);
			if (it == end) {
				throw Exception("JSON object ended prematurely!");
			}
			if (*it == ',' && !expect_more) {
				++ it;
				expect_more = true;
				continue;
			}
			if (*it == '}') {
				if (expect_more) {
					throw Exception("Expected more JSON object members, but got \"}\"!");
				}
				++ it;
				break;
			}

			// Ensure more stuff is expected
			if (!expect_more && !obj.empty()) {
				throw Exception(std::string("Expected \",\" but got \"") + *it + "\"!");
			}

			// Read key 
			std::string key = readString(it, end);
			skipWhitespace(it, end);

			// Read delimiter
			if (*it != ':') {
				throw Exception(std::string("Expected \":\" but got \"") + *it + "\"!");
			}
			++ it;
			skipWhitespace(it, end);

			// Read member
			Json new_member;
			new_member.loadRecursively(it, end);
			obj[key] = new_member;

			expect_more = false;

		}
	}
	// Array
	else if (*it == '[') {
		++ it;
		type = ARRAY;
		bool expect_more = false;
		while (true) {
			skipWhitespace(it, end);
			if (it == end) {
				throw Exception("JSON array ended prematurely!");
			}
			if (*it == ',' && !expect_more) {
				++ it;
				expect_more = true;
				continue;
			}
			if (*it == ']') {
				if (expect_more) {
					throw Exception("Expected more JSON array items, but got \"}\"!");
				}
				++ it;
				break;
			}

			// Ensure more stuff is expected
			if (!expect_more && !arr.empty()) {
				throw Exception(std::string("Expected \",\" but got \"") + *it + "\"!");
			}

			// Read item
			Json new_item;
			new_item.loadRecursively(it, end);
			arr.push_back(new_item);

			expect_more = false;

		}
	}
	// String
	else if (*it == '\"') {
		type = STRING;
		str = readString(it, end);
	}
	// Boolean true
	else if (*it == 't') {
		++ it;
		if (end - it < 3) {
			throw Exception("Unexpected \"t" + std::string(it, end) + "\"!");
		}
		if (std::string(it, it + 3) != "rue") {
			throw Exception("Unexpected \"t" + std::string(it, it + 3) + "\"!");
		}
		it += 3;
		type = BOOLEAN;
		num_is_integer = 1;
	}
	// Boolean false
	else if (*it == 'f') {
		++ it;
		if (end - it < 4) {
			throw Exception("Unexpected \"f" + std::string(it, end) + "\"!");
		}
		if (std::string(it, it + 4) != "alse") {
			throw Exception("Unexpected \"f" + std::string(it, it + 4) + "\"!");
		}
		it += 4;
		type = BOOLEAN;
		num_is_integer = 0;
	}
	// Null
	else if (*it == 'n') {
		++ it;
		if (end - it < 3) {
			throw Exception("Unexpected \"n" + std::string(it, end) + "\"!");
		}
		if (std::string(it, it + 3) != "ull") {
			throw Exception("Unexpected \"n" + std::string(it, it + 3) + "\"!");
		}
		it += 3;
		type = NUL;
	}
	// Number
	else if (*it == '-' || (*it >= '0' && *it <= '9')) {
		type = NUMBER;
		std::string::const_iterator num_begin = it;
		++ it;
		bool dot_met = false;
		while (it != end && ((*it >= '0' && *it <= '9') || (*it == '.' && !dot_met))) {
			if (*it == '.') {
				dot_met = true;
			}
			++ it;
		}
		std::string num_str(num_begin, it);
		if (num_str[num_str.size() - 1] == '.') {
			throw Exception("Unexpected \"" + num_str + "\"!");
		}
		num = atof(num_str.c_str());
		if (num_str.find('.') == std::string::npos) {
			num_i = strToSSize(num_str);
			num_is_integer = true;
		} else {
			num_i = num + 0.5;
			num_is_integer = false;
		}
	}
	// Unknown
	else {
		throw Exception(std::string("Unexpected character \"") + *it + "\"!");
	}
	skipWhitespace(it, end);
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
