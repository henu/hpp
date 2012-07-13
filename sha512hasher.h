#ifndef HPP_SHA512HASHER_H
#define HPP_SHA512HASHER_H

#include "opensslhasher.h"

namespace Hpp
{

class Sha512Hasher : public OpensslHasher< &EVP_sha512 >
{

public:

	// Constructor and destructor
	Sha512Hasher(void) :
	OpensslHasher< &EVP_sha512 >()
	{
	}
	virtual ~Sha512Hasher(void)
	{
	}

};

}

#endif

