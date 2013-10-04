#ifndef HPP_SERIALIZABLE_H
#define HPP_SERIALIZABLE_H

#include "bytev.h"

namespace Hpp
{

class Serializable
{

public:

	// "result" is not cleared
	inline void serialize(ByteV& result, bool bigendian = true) const;
	inline ByteV serialize(bool bigendian = true) const;

private:

	// "result" must not be cleared!
	virtual void doSerialize(ByteV& result, bool bigendian) const = 0;

};

inline void Serializable::serialize(ByteV& result, bool bigendian) const
{
	doSerialize(result, bigendian);
}

inline ByteV Serializable::serialize(bool bigendian) const
{
	ByteV result;
	doSerialize(result, bigendian);
	return result;
}

}

#endif
