#ifndef HPP_RANDOMIZER_H
#define HPP_RANDOMIZER_H

#include "assert.h"
#include "exception.h"

#include <cstdlib>
#include <stdint.h>
#include <sys/time.h>

namespace Hpp
{

class Randomizer
{

public:

	inline Randomizer(void);
	inline ~Randomizer(void);

	inline Randomizer(Randomizer const& rnd);
	inline Randomizer operator=(Randomizer const& rnd);

	// Seed functions
	inline void seedFromTime(void);

	// Getters
	inline uint32_t getUInt32(void);
	inline uint32_t getUInt32(uint32_t min, uint32_t max);
	inline double getDouble(void);
	inline double getDouble(double min, double max);

private:

	drand48_data rnd;

};

inline Randomizer::Randomizer(void)
{
	uint16_t seedvalue[3];
	seedvalue[0] = 0;
	seedvalue[1] = 0;
	seedvalue[2] = 0;
	seed48_r(seedvalue, &rnd);
}

inline Randomizer::~Randomizer(void)
{
}

inline Randomizer::Randomizer(Randomizer const& rnd) :
rnd(rnd.rnd)
{
}

inline Randomizer Randomizer::operator=(Randomizer const& rnd)
{
	this->rnd = rnd.rnd;
	return *this;
}

inline void Randomizer::seedFromTime(void)
{
	// Read time
	timeval tv;
	if (gettimeofday(&tv, NULL)) {
		throw Exception("Unable to read time!");
	}
	uint16_t seedvalue[3];
	seedvalue[0] = tv.tv_sec % 0xffff;
	seedvalue[1] = (tv.tv_sec >> 16) % 0xffff;
	seedvalue[2] = (tv.tv_usec >> 11) % 0xffff;
	seed48_r(seedvalue, &rnd);
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

}

#endif
