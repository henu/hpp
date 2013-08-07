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

	inline void deserialize(ByteV const& bytes);
	inline void deserialize(ByteV::const_iterator& bytes_it, ByteV::const_iterator const& bytes_end);
	inline void deserialize(std::istream& strm);

private:

	virtual void doDeserialize(std::istream& strm) = 0;

};

inline void Deserializable::deserialize(ByteV const& bytes)
{
	ByteV::const_iterator bytes_it = bytes.begin();
	ByteVReaderBuf rbuf(bytes_it, bytes.end());
	doDeserialize(rbuf);
	if (bytes_it != bytes.end()) {
		throw Exception("Deserializable did not use all bytes!");
	}
}

inline void Deserializable::deserialize(ByteV::const_iterator& bytes_it, ByteV::const_iterator const& bytes_end)
{
	ByteVReaderBuf rbuf(bytes_it, bytes_end);
	doDeserialize(rbuf);
}

inline void Deserializable::deserialize(std::istream& strm)
{
	doDeserialize(strm);
}

}

#endif
