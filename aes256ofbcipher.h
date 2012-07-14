#ifndef HPP_AES256OFBCIPHER_H
#define HPP_AES256OFBCIPHER_H

#include "opensslcipher.h"

#include <openssl/evp.h>

namespace Hpp
{

class AES256OFBCipher : public OpensslCipher< &EVP_aes_256_ofb >
{

public:

	inline AES256OFBCipher(void) : OpensslCipher< &EVP_aes_256_ofb >() { }
	inline AES256OFBCipher(ByteV const& key, ByteV const& iv, bool padding) : OpensslCipher< &EVP_aes_256_ofb >(key, iv, padding) { }
	inline virtual ~AES256OFBCipher(void) { }

};

}

#endif

