#include "json.h"

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
		return floatToStr(num);
	case STRING:
		return "\"" + slashEncode(str, "\"") + "\"";
	case BOOLEAN:
		if (num > 0) return "true";
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
}

}

