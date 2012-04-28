#ifndef HPP_SHA256HASHER_H
#define HPP_SHA256HASHER_H

#include "opensslhasher.h"

namespace Hpp
{

class Sha256Hasher : public OpensslHasher< &EVP_sha256 >
{

public:

	// Constructor and destructor
	Sha256Hasher(void) :
	OpensslHasher< &EVP_sha256 >()
	{
	}
	virtual ~Sha256Hasher(void)
	{
	}

};

}

#endif

