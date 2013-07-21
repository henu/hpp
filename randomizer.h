#ifndef HPP_RANDOMIZER_H
#define HPP_RANDOMIZER_H

#include "assert.h"
#include "exception.h"
#include "misc.h"
#include "time.h"

#include <cstdlib>
#include <stdint.h>
#include <sys/time.h>
#include <cstring>

namespace Hpp
{

class Randomizer
{

public:

	inline Randomizer(void);
	inline ~Randomizer(void);

	inline Randomizer(Randomizer const& rnd);
	inline Randomizer operator=(Randomizer const& rnd);

	// Seed functions. All seed functions begin seeding from
	// scratch, except the one that has "More" in it.
	inline void seed(ByteV const& data);
	inline void seedMore(ByteV const& data);
	inline void seedFromTime(void);

	// Getters. "max" is inclusive.
	inline uint32_t getUInt32(void);
	inline uint32_t getUInt32(uint32_t min, uint32_t max);
	inline double getDouble(void);
	inline double getDouble(double min, double max);

private:

	uint8_t seedvalues[6];
	uint8_t seedvalues_ofs;

	drand48_data rnd;

	inline void doSeedingFromSeedvalues(void);

};

inline Randomizer::Randomizer(void) :
seedvalues_ofs(0)
{
	toZero(seedvalues, 6);
	doSeedingFromSeedvalues();
}

inline Randomizer::~Randomizer(void)
{
}

inline Randomizer::Randomizer(Randomizer const& rnd) :
seedvalues_ofs(rnd.seedvalues_ofs),
rnd(rnd.rnd)
{
	memcpy(seedvalues, rnd.seedvalues, 6);
}

inline Randomizer Randomizer::operator=(Randomizer const& rnd)
{
	seedvalues_ofs = rnd.seedvalues_ofs;
	this->rnd = rnd.rnd;
	memcpy(seedvalues, rnd.seedvalues, 6);
	return *this;
}

inline void Randomizer::seed(ByteV const& data)
{
	seedvalues_ofs = 0;
	toZero(seedvalues, 6);
	seedMore(data);
}

inline void Randomizer::seedMore(ByteV const& data)
{
	for (ByteV::const_iterator it = data.begin();
	     it != data.end();
	     ++ it) {
		seedvalues[seedvalues_ofs] ^= *it;
		seedvalues_ofs = (seedvalues_ofs + 1) % 6;
	}
	doSeedingFromSeedvalues();
}

inline void Randomizer::seedFromTime(void)
{
	Time n = now();
	uint64_t sec = n.getSeconds();
	uint32_t nsec = n.getNanoseconds();
	seed(uInt64ToByteV(sec));
	seedMore(uInt32ToByteV(nsec));
}

inline uint32_t Randomizer::getUInt32(void)
{
	long int result;
	lrand48_r(&rnd, &result);
	return static_cast< uint32_t >(result);
}

inline uint32_t Randomizer::getUInt32(uint32_t min, uint32_t max)
{
	HppAssert(max >= min, "Max range must be equal or greater than min range!");
	long int result;
	lrand48_r(&rnd, &result);
	return min + static_cast< uint32_t >(result) % (max - min + 1);
}

inline double Randomizer::getDouble(void)
{
	double result;
	drand48_r(&rnd, &result);
	return result;
}

inline double Randomizer::getDouble(double min, double max)
{
	double result;
	drand48_r(&rnd, &result);
	return min + result * (max - min);
}

inline void Randomizer::doSeedingFromSeedvalues(void)
{
	uint16_t seedvalues16[3];
	seedvalues16[0] = seedvalues[0] + (seedvalues[1] << 8);
	seedvalues16[1] = seedvalues[2] + (seedvalues[3] << 8);
	seedvalues16[2] = seedvalues[4] + (seedvalues[5] << 8);
	seed48_r(seedvalues16, &rnd);
}

}

#endif
