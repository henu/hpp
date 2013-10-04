#ifndef HPP_DESERIALIZABLE_H
#define HPP_DESERIALIZABLE_H

#include "bytevreaderbuf.h"
#include "bytev.h"
#include "exception.h"

namespace Hpp
{

class Deserializable
{

public:

	inline void deserialize(ByteV const& bytes, bool bigendian = true);
	inline void deserialize(ByteV::const_iterator& bytes_it, ByteV::const_iterator const& bytes_end, bool bigendian = true);
	inline void deserialize(std::istream& strm, bool bigendian = true);

private:

	virtual void doDeserialize(std::istream& strm, bool bigendian) = 0;

};

inline void Deserializable::deserialize(ByteV const& bytes, bool bigendian)
{
	ByteV::const_iterator bytes_it = bytes.begin();
	ByteVReaderBuf rbuf(bytes_it, bytes.end());
	doDeserialize(rbuf, bigendian);
	if (bytes_it != bytes.end()) {
		throw Exception("Deserializable did not use all bytes!");
	}
}

inline void Deserializable::deserialize(ByteV::const_iterator& bytes_it, ByteV::const_iterator const& bytes_end, bool bigendian)
{
	ByteVReaderBuf rbuf(bytes_it, bytes_end);
	doDeserialize(rbuf, bigendian);
}

inline void Deserializable::deserialize(std::istream& strm, bool bigendian)
{
	doDeserialize(strm, bigendian);
}

}

#endif
