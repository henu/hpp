#ifndef HPP_TEST_H
#define HPP_TEST_H

#include <iostream>
#include <cstdlib>

inline void testAssert(bool test)
{
	if (!test) {
		std::cerr << "TEST HAS FAILED!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

#endif
