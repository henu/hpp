#include "test3d.h"
#include "testmisc.h"
#include "exception.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;
	try {
		Hpp::Tests::test3D();
		Hpp::Tests::testMisc();
	}
	catch (Hpp::Exception const& e)	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
