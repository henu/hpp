#include "assert.h"

#include "exception.h"

#include <cstdlib>
#include <iostream>

namespace Hpp
{

void doAssertion(std::string const& file, size_t line, std::string const& func, bool check, std::string const& check_str, std::string const& msg)
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

void doAssertionCC(std::string const& file, size_t line, std::string const& func, bool check, std::string const& check_str, std::string const& msg)
{
	#ifndef NDEBUG
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
	#endif
}

}
