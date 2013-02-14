#include "json.h"

#include "misc.h"
#include "cast.h"

namespace Hpp
{

Json Json::getItem(size_t index) const
{
	if (type != ARRAY) {
		throw Exception("Unable to get index, because this JSON is not an array!");
	}
	if (index >= arr.size()) {
		throw Exception("Index " + sizeToStr(index) + " is out of range!");
	}
	return arr[index];
}

std::string Json::doEncode(size_t indent, bool nice) const
{
	switch (type)
	{
	case NUMBER:
		if (num_is_integer) {
			return ssizeToStr(num_i);
		} else {
			return floatToStr(num);
		}
	case STRING:
		return "\"" + slashEncode(str, "\"") + "\"";
	case BOOLEAN:
		if (num_is_integer) return "true";
		else return "false";
	case OBJECT:
		{
			std::string result;

			std::string indent_str;
			if (nice) indent_str = std::string(indent, '\t');

			result += "{";
			if (nice) result += "\n";
			for (Object::const_iterator obj_it = obj.begin();
			     obj_it != obj.end();
			     ++ obj_it) {
				if (nice) result += indent_str + "\t";
				result += "\"" + slashEncode(obj_it->first, "\"") + "\":";
				if (nice) result += " ";
				result += obj_it->second.doEncode(indent + 1, nice) + ",";
				if (nice) result += "\n";
			}
			if (!obj.empty()) {
				if (!nice) {
					result.resize(result.size()-1);
				} else {
					result.resize(result.size()-2);
					result += "\n";
				}
			}
			result += indent_str + "}";

			return result;
		}
	case ARRAY:
		{
			std::string result;

			std::string indent_str;
			if (nice) indent_str = std::string(indent, '\t');

			result += "[";
			if (nice) result += "\n";
			for (Array::const_iterator arr_it = arr.begin();
			     arr_it != arr.end();
			     ++ arr_it) {
				if (nice) result += indent_str + "\t";
				result += arr_it->doEncode(indent + 1, nice) + ",";
				if (nice) result += "\n";
			}
			if (!arr.empty()) {
				if (!nice) {
					result.resize(result.size()-1);
				} else {
					result.resize(result.size()-2);
					result += "\n";
				}
			}
			result += indent_str + "]";

			return result;
		}
	case NUL:
		return "null";
	default:
		break;
	}
	HppAssert(false, "Invalid JSON type!");
	return "";
}

void Json::loadRecursively(std::string::const_iterator& it, std::string::const_iterator end)
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

}

