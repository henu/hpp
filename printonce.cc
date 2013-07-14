#include "printonce.h"

#include <set>
#include <iostream>

namespace Hpp
{

struct FileLine
{
	std::string filename;
	size_t line;
	inline FileLine(std::string const& filename, size_t line) :
	filename(filename),
	line(line)
	{
	}
	inline bool operator<(FileLine const& fl) const
	{
		if (line < fl.line) return true;
		if (line > fl.line) return false;
		return filename < fl.filename;
	}
};
typedef std::set< FileLine > FileLines;

FileLines already_printed;

void printOnce(std::string const& message, std::string const& filename, size_t line)
{
	FileLine fl(filename, line);
	if (already_printed.find(fl) != already_printed.end()) {
		return;
	}
	std::cout << filename << ":" << line << ": " << message << std::endl;
	already_printed.insert(fl);
}

}
