#ifndef HPP_SERIALIZABLE_H
#define HPP_SERIALIZABLE_H

#include "bytev.h"

namespace Hpp
{

class Serializable
{

public:

	// "result" is not cleared
	inline void serialize(ByteV& result) const;
	inline ByteV serialize(void) const;

private:

	// "result" must not be cleared!
	virtual void doSerialize(ByteV& result) const = 0;

};

inline void Serializable::serialize(ByteV& result) const
{
	doSerialize(result);
}

inline ByteV Serializable::serialize(void) const
{
	ByteV result;
	doSerialize(result);
	return result;
}

}

#endif
