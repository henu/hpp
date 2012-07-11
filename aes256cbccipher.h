#ifndef HPP_AES256CBCCIPHER_H
#define HPP_AES256CBCCIPHER_H

#include "opensslcipher.h"

#include <openssl/evp.h>

namespace Hpp
{

class AES256CBCCipher : public OpensslCipher< &EVP_aes_256_cbc >
{

public:

	inline AES256CBCCipher(void) : OpensslCipher< &EVP_aes_256_cbc >() { }
	inline AES256CBCCipher(ByteV const& key, ByteV const& iv, bool padding) : OpensslCipher< &EVP_aes_256_cbc >(key, iv, padding) { }
	inline virtual ~AES256CBCCipher(void) { }

};

}

#endif

