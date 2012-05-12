#ifndef HPP_TESTCAST_H
#define HPP_TESTCAST_H

#include "cast.h"
#include "random.h"
#include "test.h"

#include <string.h>
#include <iostream>

namespace Hpp
{

namespace Tests
{

// TODO: Make 64bit tests!
inline void testCast(void)
{
	uint8_t testbuf[4];

	// Test both bigendian and non-bigendian cases
	std::string endianness = "little-endian";
	for (uint8_t bigendian = 0; bigendian < 2; ++ bigendian) {

		// First generate some test values
		float f = random(-9999.9, 9999.9);
		ByteV f_as_bytev = floatToByteV(f, bigendian);
		std::vector< ssize_t > is;
		for (size_t bytesize = 0; bytesize <= 4; ++ bytesize) {
			ssize_t i = 0;
			if (bytesize > 0) {
				ssize_t i_min = -((ssize_t)1 << (bytesize*8-1));
				ssize_t i_max = -i_min - 1;
				i = randomInt(i_min, i_max);
			}
			is.push_back(i);
		}
		std::vector< size_t > uis;
		for (size_t bytesize = 0; bytesize <= 4; ++ bytesize) {
			size_t ui = 0;
			if (bytesize > 0) {
				size_t ui_max_plus_1 = ((ssize_t)1 << (bytesize*8));
				ui = rand() % ui_max_plus_1;
			}
			uis.push_back(ui);
		}
		std::vector< ByteV > is_as_bytevs;
		for (size_t bytesize = 0; bytesize <= 4; ++ bytesize) {
			ByteV i_as_bytev;
			if (bytesize > 0) {
				i_as_bytev = intToByteV(is[bytesize], bytesize, bigendian);
			}
			is_as_bytevs.push_back(i_as_bytev);
		}
		std::vector< ByteV > uis_as_bytevs;
		for (size_t bytesize = 0; bytesize <= 4; ++ bytesize) {
			ByteV ui_as_bytev;
			if (bytesize > 0) {
				ui_as_bytev = uIntToByteV(uis[bytesize], bytesize, bigendian);
			}
			uis_as_bytevs.push_back(ui_as_bytev);
		}

		std::cout << "Test cStrToFloat() / " << endianness << std::endl;
		testAssert(cStrToFloat(&f_as_bytev[0], bigendian) == f);

		for (size_t bytesize = 1; bytesize <= 4; ++ bytesize) {
			std::cout << "Test cStrToInt() / " << endianness << " / " << bytesize << " bytes" << std::endl;
			testAssert(cStrToInt(&(is_as_bytevs[bytesize])[0], bytesize, bigendian) == is[bytesize]);
		}

		std::cout << "Test cStrToInt16() / " << endianness << std::endl;
		testAssert(cStrToInt16(&(is_as_bytevs[2])[0], bigendian) == is[2]);

		std::cout << "Test cStrToInt32() / " << endianness << std::endl;
		testAssert(cStrToInt32(&(is_as_bytevs[4])[0], bigendian) == is[4]);

		for (size_t bytesize = 1; bytesize <= 4; ++ bytesize) {
			std::cout << "Test cStrToUInt() / " << endianness << " / " << bytesize << " bytes" << std::endl;
			testAssert(cStrToUInt(&(uis_as_bytevs[bytesize])[0], bytesize, bigendian) == uis[bytesize]);
		}

		std::cout << "Test cStrToUInt16() / " << endianness << std::endl;
		testAssert(cStrToUInt16(&(uis_as_bytevs[2])[0], bigendian) == uis[2]);

		std::cout << "Test cStrToUInt32() / " << endianness << std::endl;
		testAssert(cStrToUInt32(&(uis_as_bytevs[4])[0], bigendian) == uis[4]);

		std::cout << "Test floatToByteV() / " << endianness << std::endl;
		testAssert(floatToByteV(f, bigendian) == f_as_bytev);

		std::cout << "Test floatToCStr() / " << endianness << std::endl;
		floatToCStr(f, testbuf, bigendian);
		testAssert(memcmp((void*)testbuf, (void*)&f_as_bytev[0], 4) == 0);

		for (size_t bytesize = 1; bytesize <= 4; ++ bytesize) {
			std::cout << "Test intToByteV() / " << endianness << " / " << bytesize << " bytes" << std::endl;
			testAssert(intToByteV(is[bytesize], bytesize, bigendian) == is_as_bytevs[bytesize]);
		}

		std::cout << "Test int16ToByteV() / " << endianness << std::endl;
		testAssert(int16ToByteV(is[2], bigendian) == is_as_bytevs[2]);

		std::cout << "Test int16ToCStr() / " << endianness << std::endl;
		int16ToCStr(is[2], testbuf, bigendian);
		testAssert(memcmp((void*)testbuf, (void*)&(is_as_bytevs[2])[0], 2) == 0);

		std::cout << "Test int32ToByteV() / " << endianness << std::endl;
		testAssert(int32ToByteV(is[4], bigendian) == is_as_bytevs[4]);

		std::cout << "Test int32ToCStr() / " << endianness << std::endl;
		int32ToCStr(is[4], testbuf, bigendian);
		testAssert(memcmp((void*)testbuf, (void*)&(is_as_bytevs[4])[0], 4) == 0);

		std::cout << "Test uInt16ToByteV() / " << endianness << std::endl;
		testAssert(uInt16ToByteV(uis[2], bigendian) == uis_as_bytevs[2]);

		std::cout << "Test uInt16ToCStr() / " << endianness << std::endl;
		uInt16ToCStr(uis[2], testbuf, bigendian);
		testAssert(memcmp((void*)testbuf, (void*)&(uis_as_bytevs[2])[0], 2) == 0);

		std::cout << "Test uInt32ToByteV() / " << endianness << std::endl;
		testAssert(uInt32ToByteV(uis[4], bigendian) == uis_as_bytevs[4]);

		std::cout << "Test uInt32ToCStr() / " << endianness << std::endl;
		uInt32ToCStr(uis[4], testbuf, bigendian);
		testAssert(memcmp((void*)testbuf, (void*)&(uis_as_bytevs[4])[0], 4) == 0);

		for (size_t bytesize = 1; bytesize <= 4; ++ bytesize) {
			std::cout << "Test uIntToByteV() / " << endianness << " / " << bytesize << " bytes" << std::endl;
			testAssert(uIntToByteV(uis[bytesize], bytesize, bigendian) == uis_as_bytevs[bytesize]);
		}

		for (size_t bytesize = 1; bytesize <= 4; ++ bytesize) {
			std::cout << "Test uIntToCStr() / " << endianness << " / " << bytesize << " bytes" << std::endl;
			uIntToCStr(uis[bytesize], testbuf, bytesize, bigendian);
			testAssert(memcmp((void*)testbuf, (void*)&(uis_as_bytevs[bytesize])[0], bytesize) == 0);
		}

// TODO: Test also the following: matrix3ToBytes, matrix4ToBytes.
// TODO: Test also serialize.h!

		endianness = "big-endian";
	}
}

}

}

#endif
