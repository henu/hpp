#ifndef HPP_OPENSSLHASHER_H
#define HPP_OPENSSLHASHER_H

#include "hasher.h"

#include "bytev.h"
#include "assert.h"
#include "exception.h"
#include <openssl/evp.h>


namespace Hpp
{

template< EVP_MD const* (*digfunc)(void) > class OpensslHasher : public Hasher
{

public:

	// Constructor and destructor
	inline OpensslHasher(void) :
	Hasher(),
	hasher_initialized(false)
	{
	}
	inline virtual ~OpensslHasher(void)
	{
		reset();
	}

	// Virtual functions, needed by superclass Hasher
	inline virtual size_t getSize(void)
	{
		return EVP_MD_size(digfunc());
	}
	inline virtual void reset(void)
	{
		if (hasher_initialized) {
			EVP_MD_CTX_cleanup(&hasher);
			hasher_initialized = false;
		}
	}

private:

	// Openssl hasher and boolean if hasher has been initialized or not.
	EVP_MD_CTX hasher;
	bool hasher_initialized;

	// Virtual functions, needed by superclass Hasher
	virtual void doAddData(uint8_t const* data, size_t data_size)
	{
		ensureHasherIsInitialized();
		if (data_size > 0 && !EVP_DigestUpdate(&hasher, data, data_size)) {
			throw Exception("Unable to hash data!");
		}
	}
	virtual void doGetHash(uint8_t* hash)
	{
		ensureHasherIsInitialized();
		unsigned int result_size;
		if (!EVP_DigestFinal_ex(&hasher, hash, &result_size)) {
			throw Exception("Unable to get hash!");
		}
		HppAssert((int)result_size == EVP_MD_size(digfunc()), "Hash sizes do not match!");
	}

	inline void ensureHasherIsInitialized(void)
	{
		if (!hasher_initialized) {
			EVP_MD_CTX_init(&hasher);
			if (!EVP_DigestInit_ex(&hasher, digfunc(), NULL)) {
				throw Exception("Unable to initialize hasher!");
			}
			hasher_initialized = true;
		}
	}

};

}

#endif

