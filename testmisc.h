#ifndef HPP_TESTMISC_H
#define HPP_TESTMISC_H

#include "misc.h"
#include "assert.h"

#include <iostream>
namespace Hpp
{

namespace Tests
{

inline void testMisc(void)
{

	// Test division functions
	{
		HppAssert(iDivFloor(50, 9) == 5, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(-50, -9) == 5, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(-50, 9) == -6, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(50, -9) == -6, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(50, 5) == 10, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(-50, -5) == 10, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(-50, 5) == -10, "Testing of iDivFloor has failed!")
		HppAssert(iDivFloor(50, -5) == -10, "Testing of iDivFloor has failed!")

		HppAssert(iDivCeil(50, 9) == 6, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(-50, -9) == 6, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(-50, 9) == -5, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(50, -9) == -5, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(50, 5) == 10, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(-50, -5) == 10, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(-50, 5) == -10, "Testing of iDivCeil has failed!")
		HppAssert(iDivCeil(50, -5) == -10, "Testing of iDivCeil has failed!")

		HppAssert(iMod(50, 9) == 5, "Testing of iMod has failed!")
		HppAssert(iMod(-50, 9) == 4, "Testing of iMod has failed!")
	}

}

}

}

#endif
