#ifndef HPP_ASSERT_H
#define HPP_ASSERT_H

#include "exception.h"

#include <string>
#include <cstdlib>
#include <iostream>

#ifndef NDEBUG
#define HppAssert(check, error) Hpp::doAssertion(__FILE__, __LINE__, __FUNCTION__, (check), #check, error);
#else
#define HppAssert(check, error)
#endif

namespace Hpp
{

inline void doAssertion(std::string const& file, size_t line, std::string const& func, bool check, std::string const& check_str, std::string const& msg)
{
	if (check) {
		return;
	}
	std::cerr << "ERROR: An assertion has failed!" << std::endl;
	std::cerr << "Condition: " << check_str << std::endl;
	std::cerr << "File:      " << file << std::endl;
	std::cerr << "Line:      " << line << std::endl;
	std::cerr << "Function:  " << func << std::endl;
	if (!msg.empty()) {
		std::cerr << "Message:   " << msg << std::endl;
	}
	#ifdef HPP_ASSERT_ABORTS
	abort();
	#else
	throw Exception("Assertion has failed!");
	#endif
}

}

#endif

