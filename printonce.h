#ifndef HPP_PRINTONCE_H
#define HPP_PRINTONCE_H

#define HppPrintOnce(message) Hpp::printOnce(message, __FILE__, __LINE__);

#include <string>

namespace Hpp
{

	void printOnce(std::string const& message, std::string const& filename, size_t line);

}

#endif

