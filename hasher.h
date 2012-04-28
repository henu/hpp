#ifndef HPP_HASHER_H
#define HPP_HASHER_H

#include "bytev.h"

#include <string>

namespace Hpp
{

class Hasher
{

public:

	// Constructor and destructor
	inline Hasher(void) { }
	inline virtual ~Hasher(void);

	// Gets size of hash.
	virtual size_t getSize(void) = 0;

	// Resets hash and discard any given data
	virtual void reset(void) = 0;

	// Give more data to hash.
	inline void addData(ByteV const& data);
	inline void addData(int8_t const* data, size_t data_size);
	inline void addData(uint8_t const* data, size_t data_size);
	inline void addData(std::string const& data);

	// Gets hash. This also resets hashing.
	inline void getHash(ByteV& result);

private:

	virtual void doAddData(uint8_t const* data, size_t data_size) = 0;
	virtual void doGetHash(uint8_t* result) = 0;

};

inline Hasher::~Hasher(void)
{
}

inline void Hasher::addData(ByteV const& data)
{
	doAddData(&data[0], data.size());
}

inline void Hasher::addData(int8_t const* data, size_t data_size)
{
	doAddData(reinterpret_cast< uint8_t const* >(data), data_size);
}

inline void Hasher::addData(uint8_t const* data, size_t data_size)
{
	doAddData(data, data_size);
}

inline void Hasher::addData(std::string const& data)
{
	 doAddData((uint8_t const*)&data[0], data.size());
}

inline void Hasher::getHash(ByteV& result)
{
	result.resize(getSize());
	doGetHash(&result[0]);
	reset();
}

}

#endif

