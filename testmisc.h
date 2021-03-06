#ifndef HPP_TESTMISC_H
#define HPP_TESTMISC_H

#include "math.h"
#include "collisiontests.h"
#include "collisions.h"
#include "sha256hasher.h"
#include "misc.h"
#include "assert.h"
#include "cast.h"
#include "path.h"
#include "bytevreaderbuf.h"

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

		HppAssert(abs(dMod(9.5, 2) - 1.5) < 0.001, "Testing of dMod has failed!")
		HppAssert(abs(dMod(-8.5, 2) - 1.5) < 0.001, "Testing of dMod has failed!")
	}

	// Test round, floor and ceiling functions
	{
		HppAssert(iRound(-9.00001) == -9, "Testing of iRound has failed!");
		HppAssert(iRound(9.99999) == 10, "Testing of iRound has failed!");
		HppAssert(iRound(9.00001) == 9, "Testing of iRound has failed!");
		HppAssert(iRound(-9.99999) == -10, "Testing of iRound has failed!");
		HppAssert(iFloor(-9.00001) == -10, "Testing of iFloor has failed!");
		HppAssert(iFloor(9.99999) == 9, "Testing of iFloor has failed!");
		HppAssert(iCeil(9.00001) == 10, "Testing of iCeil has failed!");
		HppAssert(iCeil(-9.99999) == -9, "Testing of iCeil has failed!");
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

	// Test path sorting
	{
		std::vector< Path > paths;
		paths.push_back(Path::getUnknown());
		paths.push_back(Path::getUnknown());
		paths.push_back(Path::getHome() / "test" / "foobar");
		paths.push_back(Path::getHome() / "omg");
		paths.push_back(Path::getConfig() / "123" / "abc");
		paths.push_back(Path::getConfig() / "zomg");
		paths.push_back(Path("/abs/test"));
		paths.push_back(Path("/zzz/xxx/yyy"));
		paths.push_back(Path("/abs/abs"));
		paths.push_back(Path("/zzz/xxx/yyy/aaa"));
		paths.push_back(Path("rel/test"));
		paths.push_back(Path("zzz/xxx/yyy"));
		paths.push_back(Path("rel/test5"));
		paths.push_back(Path("zzz/xxx/123"));

		std::vector< Path > paths_sorted = paths;
		std::sort(paths_sorted.begin(), paths_sorted.end());

		for (size_t test = 0; test < 100; ++ test) {
			std::random_shuffle(paths.begin(), paths.end());
			std::sort(paths.begin(), paths.end());
			HppAssert(paths == paths_sorted, "Unable to sort paths!");
		}
	}

	// Test path name getting
	{
		Path p1("/tmp/package.tar.gz");
		Path p2("/tmp/image.jpeg");

		HppAssert(p1.getFilename() == "package.tar.gz", "Getting name of path has failed!");
		HppAssert(p2.getFilename() == "image.jpeg", "Getting name of path has failed!");

		HppAssert(p1.getFilenameWithoutExtension() == "package.tar", "Getting name of path has failed!");
		HppAssert(p2.getFilenameWithoutExtension() == "image", "Getting name of path has failed!");
		HppAssert(p1.getFilenameWithoutExtension(0) == "package.tar", "Getting name of path has failed!");
		HppAssert(p2.getFilenameWithoutExtension(0) == "image", "Getting name of path has failed!");
		HppAssert(p1.getFilenameWithoutExtension(1) == "package", "Getting name of path has failed!");
		HppAssert(p2.getFilenameWithoutExtension(1) == "image.jpeg", "Getting name of path has failed!");

		HppAssert(p1.getExtension() == "gz", "Getting name of path has failed!");
		HppAssert(p2.getExtension() == "jpeg", "Getting name of path has failed!");
		HppAssert(p1.getExtension(0) == "gz", "Getting name of path has failed!");
		HppAssert(p2.getExtension(0) == "jpeg", "Getting name of path has failed!");
		HppAssert(p1.getExtension(1) == "tar.gz", "Getting name of path has failed!");
		HppAssert(p2.getExtension(1) == "", "Getting name of path has failed!");
	}

	// Test collision detection
	{
		Collisions colls;
		colls.push_back(Collision(Vector3(0.751959, -0.65921, 8.46226e-07), 0.0534745));
		colls.push_back(Collision(Vector3(-0.852809, -0.522223, 1.98682e-07), -2.98023e-08));
		colls.push_back(Collision(Vector3(-0, -0, 1), 0.00270716));
		Vector3 move_out_vec = moveOut(colls);
		HppAssert(move_out_vec.length() < 0.2, "Too long move out -vector!");

		Collision coll;
		Vector3 pos0 = Vector3(1, 2, 3);
		Vector3 pos1 = Vector3(-4, 1, -2);
		HppAssert(Collisiontests::sphereToSphere(coll, pos0, 2, pos1, 5, -1), "No collision detected!");
		HppAssert(fabs(coll.depth + 0.141428429) < 0.0001, "Got invalid collision depth!");
		HppAssert(angleBetweenVectors(coll.normal, pos0 - pos1) < Angle(0.001), "Invalid collision normal!");
	}

	// Test string case changing
	{
		std::string s = "fOoBaRTestING1234567890";

		std::string s_u = Hpp::toUpper(s);
		std::string s_l = Hpp::toLower(s);

		HppAssert(s_u == "FOOBARTESTING1234567890", "Case conversion has failed!");
		HppAssert(s_l == "foobartesting1234567890", "Case conversion has failed!");
	}

	// Test ByteVReaderBuf
	{
		ByteV bv(10, 0);
		char nullbuf[10];

		ByteV::const_iterator bv_it = bv.begin();
		ByteVReaderBuf rbuf1(bv_it, bv.end());
		for (size_t reading = 0; reading < 6; ++ reading) {
			rbuf1.get();
		}
		HppAssert(bv.end() - bv_it == 4, "Iterator advancing has failed!");

		ByteVReaderBuf rbuf2(bv);
		for (size_t reading = 0; reading < 10; ++ reading) {
			rbuf2.get();
		}
		HppAssert(!rbuf2.eof(), "Unexpected EOF!");
		rbuf2.get();
		HppAssert(rbuf2.eof(), "No EOF found!");

		bv_it = bv.begin();
		ByteVReaderBuf rbuf3(bv_it, bv.end());
		rbuf3.read(nullbuf, 6);
		HppAssert(bv.end() - bv_it == 4, "Iterator advancing has failed!");

		ByteVReaderBuf rbuf4(bv);
		rbuf4.read(nullbuf, 10);
		HppAssert(!rbuf4.eof(), "Unexpected EOF!");
		rbuf4.get();
		HppAssert(rbuf4.eof(), "No EOF found!");

		ByteV word;
		word.push_back('o');
		word.push_back('m');
		word.push_back('g');
		word.push_back('z');
		ByteVReaderBuf word_rbuf(word);
		for (int i = 0; i < 4; ++ i) {
			int c = word_rbuf.get();
			HppAssert(c == word[i], "get() does not work!");
		}
	}

	// Test Time
	{
		Time t1 = now();
		size_t year = strToSize(t1.toString("%Y"));
		size_t month = strToSize(t1.toString("%m"));
		size_t day = strToSize(t1.toString("%d"));
		size_t hour = strToSize(t1.toString("%H"));
		size_t minute = strToSize(t1.toString("%M"));
		size_t second = strToSize(t1.toString("%S"));

		Time t2(year, month, day, hour, minute, second);
		HppAssert(year == strToSize(t2.toString("%Y")), "Years do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(month == strToSize(t2.toString("%m")), "Months do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(day == strToSize(t2.toString("%d")), "Days do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(hour == strToSize(t2.toString("%H")), "Hours do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(minute == strToSize(t2.toString("%M")), "Minutes do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(second == strToSize(t2.toString("%S")), "Seconds do not match! Dates are " + t1.toString() + " and " + t2.toString() + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");

		// TODO: Uncomment this when timezone conversions in Time() constructor work!
		/*
		size_t year2 = strToSize(t1.toString("%Y", "UTC"));
		size_t month2 = strToSize(t1.toString("%m", "UTC"));
		size_t day2 = strToSize(t1.toString("%d", "UTC"));
		size_t hour2 = strToSize(t1.toString("%H", "UTC"));
		size_t minute2 = strToSize(t1.toString("%M", "UTC"));
		size_t second2 = strToSize(t1.toString("%S", "UTC"));

		Time t3(year2, month2, day2, hour2, minute2, second2, 0, "UTC");
		HppAssert(year2 == strToSize(t3.toString("%Y", "UTC")), "Years do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(month2 == strToSize(t3.toString("%m", "UTC")), "Months do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(day2 == strToSize(t3.toString("%d", "UTC")), "Days do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(hour2 == strToSize(t3.toString("%H", "UTC")), "Hours do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(minute2 == strToSize(t3.toString("%M", "UTC")), "Minutes do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");
		HppAssert(second2 == strToSize(t3.toString("%S", "UTC")), "Seconds do not match! Dates are " + t1.toString("%F %T", "UTC") + " and " + t2.toString("%F %T", "UTC") + ", and difference is " + floatToStr((t2 - t1).getSecondsAsDouble()) + " seconds.");

		HppAssert(t1.getSeconds() == t2.getSeconds(), "Seconds #1 and #2 do not match! Seconds are: " + sizeToStr(t1.getSeconds()) + "/" + sizeToStr(t2.getSeconds()) + "/" + sizeToStr(t3.getSeconds()));
		HppAssert(t1.getSeconds() == t3.getSeconds(), "Seconds #1 and #3 do not match! Seconds are: " + sizeToStr(t1.getSeconds()) + "/" + sizeToStr(t2.getSeconds()) + "/" + sizeToStr(t3.getSeconds()));
		*/
	}

}

}

}

#endif
