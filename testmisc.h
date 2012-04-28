#ifndef HPP_TESTMISC_H
#define HPP_TESTMISC_H

#include "sha256hasher.h"
#include "misc.h"
#include "assert.h"
#include "cast.h"

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

	// Test SHA-256 hasher
	{
		Sha256Hasher hasher;
		ByteV hasher_result1;
		ByteV hasher_result2;
		ByteV hasher_result3;
		hasher.getHash(hasher_result1);
		hasher.addData("The quick brown fox jumps over the lazy dog");
		hasher.getHash(hasher_result2);
		hasher.addData("The quick brown fox jumps over the lazy dog.");
		hasher.getHash(hasher_result3);

		HppAssert(byteVToHexV(hasher_result1) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", "SHA-256 hasher has failed!");
		HppAssert(byteVToHexV(hasher_result2) == "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", "SHA-256 hasher has failed!");
		HppAssert(byteVToHexV(hasher_result3) == "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c", "SHA-256 hasher has failed!");
	}

}

}

}

#endif
