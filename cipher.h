#ifndef HPP_CIPHER_H
#define HPP_CIPHER_H

#include "bytev.h"

#include <string>

namespace Hpp
{

class Cipher
{

public:

	inline Cipher(void) { }
	inline virtual ~Cipher(void) { }

	// Encrypts chunk of data. Result of encryption can be done
	// using readEncrypted(). Note, that result is not cleared!
	// Also note, that if you finalize encryption, rest of data
	// is returned no matter if you have set max_size!
	inline void encrypt(ByteV const& data);
	inline void encrypt(std::string const& data);
	inline void encrypt(uint8_t const* data, size_t data_size);
	inline void readEncrypted(ByteV& result, bool finalize, size_t max_size = 0);

	// Decrypts chunk of data. Result of decryption can be done
	// using readDecrypted(). Note, that result is not cleared!
	// Also note, that if you finalize decryption, rest of data
	// is returned no matter if you have set max_size!
	inline void decrypt(ByteV const& data);
	inline void decrypt(std::string const& data);
	inline void decrypt(uint8_t const* data, size_t data_size);
	inline void readDecrypted(ByteV& result, bool finalize, size_t max_size = 0);

private:

	// Storage for the data that user does not want to read yet
	ByteV encrypt_cache;
	ByteV decrypt_cache;

	// Actual encryption/decryption functions.
	// These functions must *NOT* clear result!
	virtual void doEncryption(ByteV& result, uint8_t const* data, size_t data_size) = 0;
	virtual void finalizeEncryption(ByteV& result) = 0;
	virtual void doDecryption(ByteV& result, uint8_t const* data, size_t data_size) = 0;
	virtual void finalizeDecryption(ByteV& result) = 0;

};

inline void Cipher::encrypt(ByteV const& data)
{
	doEncryption(encrypt_cache, &data[0], data.size());
}

inline void Cipher::encrypt(std::string const& data)
{
	doEncryption(encrypt_cache, (uint8_t*)&data[0], data.size());
}

inline void Cipher::encrypt(uint8_t const* data, size_t data_size)
{
	doEncryption(encrypt_cache, data, data_size);
}

inline void Cipher::readEncrypted(ByteV& result, bool finalize, size_t max_size)
{
	if (finalize) {
		finalizeEncryption(encrypt_cache);
	}

	if (max_size > 0 && !finalize) {
		size_t result_add_amount = std::min(max_size, encrypt_cache.size());
		result.insert(result.end(),
		              encrypt_cache.begin(),
		              encrypt_cache.begin() + result_add_amount);
		encrypt_cache.erase(encrypt_cache.begin(),
		                    encrypt_cache.begin() + result_add_amount);
	} else {
		result += encrypt_cache;
		encrypt_cache.clear();
	}
}

inline void Cipher::decrypt(ByteV const& data)
{
	doDecryption(decrypt_cache, &data[0], data.size());
}

inline void Cipher::decrypt(std::string const& data)
{
	doDecryption(decrypt_cache, (uint8_t*)&data[0], data.size());
}

inline void Cipher::decrypt(uint8_t const* data, size_t data_size)
{
	doDecryption(decrypt_cache, data, data_size);
}

inline void Cipher::readDecrypted(ByteV& result, bool finalize, size_t max_size)
{
	if (finalize) {
		finalizeDecryption(decrypt_cache);
	}

	if (max_size > 0 && !finalize) {
		size_t result_add_amount = std::min(max_size, decrypt_cache.size());
		result.insert(result.end(),
		              decrypt_cache.begin(),
		              decrypt_cache.begin() + result_add_amount);
		decrypt_cache.erase(decrypt_cache.begin(),
		                    decrypt_cache.begin() + result_add_amount);
	} else {
		result += decrypt_cache;
		decrypt_cache.clear();
	}
}

}

#endif
