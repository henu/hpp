#ifndef HPP_OPENSSLCIPHER_H
#define HPP_OPENSSLCIPHER_H

#include "cipher.h"
#include "assert.h"
#include "exception.h"

#include <openssl/evp.h>

namespace Hpp
{

template< EVP_CIPHER const* (*cipfunc)(void) > class OpensslCipher : public Cipher
{

public:

	inline OpensslCipher(void) : initialized(false) { }

	inline OpensslCipher(ByteV const& key, ByteV const& iv, bool padding) :
	initialized(false)
	{
		init(key, iv, padding);
	}
	inline virtual ~OpensslCipher(void)
	{
		// For security, clean ciphers.
		if (initialized) {
			EVP_CIPHER_CTX_cleanup(&enc_cipher);
			EVP_CIPHER_CTX_cleanup(&dec_cipher);
		}
	}

	inline void init(ByteV const& key, ByteV const& iv, bool padding)
	{
		if (initialized) {
			EVP_CIPHER_CTX_cleanup(&enc_cipher);
			EVP_CIPHER_CTX_cleanup(&dec_cipher);
		}

		// Verify parameters
		size_t key_size = EVP_CIPHER_key_length(cipfunc());
		size_t iv_size = EVP_CIPHER_iv_length(cipfunc());
		if (key_size != key.size()) {
			throw Exception("Key has invalid size!");
		}
		if (iv_size != iv.size()) {
			throw Exception("Initialization vector has invalid size!");
		}

		// Initialize encryption cipher
		EVP_CIPHER_CTX_init(&enc_cipher);
		if (!EVP_EncryptInit_ex(&enc_cipher, cipfunc(), NULL, &key[0], &iv[0])) {
			EVP_CIPHER_CTX_cleanup(&enc_cipher);
			throw Exception("Unable to initialize OpenSSL cipher!");
		}

		// Initialize decryption cipher
		EVP_CIPHER_CTX_init(&dec_cipher);
		if (!EVP_DecryptInit_ex(&dec_cipher, cipfunc(), NULL, &key[0], &iv[0])) {
			EVP_CIPHER_CTX_cleanup(&enc_cipher);
			EVP_CIPHER_CTX_cleanup(&dec_cipher);
			throw Exception("Unable to initialize OpenSSL cipher!");
		}

		// Set padding
		EVP_CIPHER_CTX_set_padding(&enc_cipher, padding);
		EVP_CIPHER_CTX_set_padding(&dec_cipher, padding);

		initialized = true;
	}

private:

	// Is class initialized or not
	bool initialized;

	// Ciphers
	EVP_CIPHER_CTX enc_cipher;
	EVP_CIPHER_CTX dec_cipher;

	inline virtual void doEncryption(ByteV& result, uint8_t const* data, size_t data_size)
	{
		if (!initialized) {
			throw Exception("Cipher is not initialized!");
		}

		size_t const BLOCKSIZE = EVP_CIPHER_CTX_block_size(&enc_cipher);

		// Make some space to result
		size_t subresult_reserve = data_size + BLOCKSIZE * 3;
		size_t subresult_begin = result.size();
		result.insert(result.end(), subresult_reserve, 0);

		// Encrypt chunk
		int subresult_size;
		if (!EVP_EncryptUpdate(&enc_cipher, &result[subresult_begin], &subresult_size, data, data_size)) {
			throw Exception("Unable to encrypt with OpenSSL cipher!");
		}
		HppAssert(subresult_size <= (int)subresult_reserve, "Result overflow!");

		// Resize result to correct size
		result.erase(result.begin() + subresult_begin + subresult_size, result.end());
	}

	inline virtual void finalizeEncryption(ByteV& result)
	{
		if (!initialized) {
			throw Exception("Cipher is not initialized!");
		}

		size_t const BLOCKSIZE = EVP_CIPHER_CTX_block_size(&enc_cipher);

		// Make some space to result
		size_t subresult_reserve = BLOCKSIZE * 3;
		size_t subresult_begin = result.size();
		result.insert(result.end(), subresult_reserve, 0);

		// Finalize encryption
		int subresult_size;
		if (!EVP_EncryptFinal_ex(&enc_cipher, &result[subresult_begin], &subresult_size)) {
			throw Exception("Unable to finalize encryption with OpenSSL cipher!");
		}
		HppAssert(subresult_size <= (int)subresult_reserve, "Result overflow!");

		// Resize result to correct size
		result.erase(result.begin() + subresult_begin + subresult_size, result.end());
	}

	inline virtual void doDecryption(ByteV& result, uint8_t const* data, size_t data_size)
	{
		if (!initialized) {
			throw Exception("Cipher is not initialized!");
		}

		size_t const BLOCKSIZE = EVP_CIPHER_CTX_block_size(&enc_cipher);

		// Make some space to result
		size_t subresult_reserve = data_size + BLOCKSIZE * 3;
		size_t subresult_begin = result.size();
		result.insert(result.end(), subresult_reserve, 0);

		// Decrypt chunk
		int subresult_size;
		if (!EVP_DecryptUpdate(&dec_cipher, &result[subresult_begin], &subresult_size, data, data_size)) {
			throw Exception("Unable to decrypt with OpenSSL cipher!");
		}
		HppAssert(subresult_size <= (int)subresult_reserve, "Result overflow!");

		// Resize result to correct size
		result.erase(result.begin() + subresult_begin + subresult_size, result.end());
	}

	inline virtual void finalizeDecryption(ByteV& result)
	{
		if (!initialized) {
			throw Exception("Cipher is not initialized!");
		}

		size_t const BLOCKSIZE = EVP_CIPHER_CTX_block_size(&dec_cipher);

		// Make some space to result
		size_t subresult_reserve = BLOCKSIZE * 3;
		size_t subresult_begin = result.size();
		result.insert(result.end(), subresult_reserve, 0);

		// Finalize decryption
		int subresult_size;
		if (!EVP_DecryptFinal_ex(&dec_cipher, &result[subresult_begin], &subresult_size)) {
			throw Exception("Unable to finalize decryption with OpenSSL cipher!");
		}
		HppAssert(subresult_size <= (int)subresult_reserve, "Result overflow!");

		// Resize result to correct size
		result.erase(result.begin() + subresult_begin + subresult_size, result.end());
	}

};

}

#endif
