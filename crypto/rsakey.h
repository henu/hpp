#ifndef RSAKEY_H
#define RSAKEY_H

#include "../exception.h"
#include "../serialize.h"
#include "../serializable.h"
#include "../deserializable.h"
#include "../cast.h"

#include <openssl/rsa.h>
#include <openssl/rand.h>

namespace Hpp
{

namespace Crypto
{

class RSAKey : public Serializable, public Deserializable
{

public:

	// How encryption and decryption should be done. In case of "DEFAULT",
	// private key is used if it is available, otherwise public key.
	enum Method
	{
		PRIVATE,
		PUBLIC,
		DEFAULT
	};

	inline RSAKey(void);
	inline ~RSAKey(void);

	// Copy constructor and assignment operator
	inline RSAKey(RSAKey const& key);
	inline RSAKey const& operator=(RSAKey const& key);

	inline bool valid(void) const { return rsa; }
	inline bool hasPrivatePart(void) const { return rsa->d; }

	// "result" is not cleared.
	inline void encrypt(ByteV& result, ByteV const& data, Method method = DEFAULT);
	inline void decrypt(ByteV& result, ByteV const& data, Method method = DEFAULT);

	inline static RSAKey generatePrivateKey(size_t bits = 4096);
	inline RSAKey generatePublicKey(void) const;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual Json toJson(void) const;
	inline virtual void constructFromJson(Json const& json);

private:

	RSA* rsa;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result, bool bigendian) const;
	inline virtual void doDeserialize(std::istream& strm, bool bigendian);

	// "key" can be NULL in both functions.
	inline static void cleanKey(RSA* rsa);
	inline static RSA* cloneKey(RSA* rsa);

	inline static ByteV bignumToByteV(BIGNUM* bn, bool bigendian);
	inline static BIGNUM* istreamToBignum(std::istream& strm, bool bigendian);

};

inline RSAKey::RSAKey(void) :
rsa(NULL)
{
}

inline RSAKey::~RSAKey(void)
{
	cleanKey(rsa);
}

inline RSAKey::RSAKey(RSAKey const& key)
{
	rsa = cloneKey(key.rsa);
}

inline RSAKey const& RSAKey::operator=(RSAKey const& key)
{
	cleanKey(rsa);
	rsa = cloneKey(key.rsa);
	return *this;
}

inline void RSAKey::encrypt(ByteV& result, ByteV const& data, Method method)
{
	if (!rsa) {
		throw Exception("Unable to encrypt because RSA key is not initialized!");
	}

	Method final_method;
	if (method == PUBLIC) {
		final_method = PUBLIC;
	} else if (method == PRIVATE) {
		if (!hasPrivatePart()) {
			throw Exception("Unable to encrypt using private part because RSA key does not have it!");
		}
		final_method = PRIVATE;
	} else {
		if (hasPrivatePart()) {
			final_method = PRIVATE;
		} else {
			final_method = PUBLIC;
		}
	}

	size_t encrypted = 0;

	ByteV chunk(RSA_size(rsa), 0);

	while (encrypted < data.size()) {

		// Calculate how much we have left to encrypt
		size_t data_left = data.size() - encrypted;

		// Calculate how big chunk we can encrypt with this RSA key.
		// If it's more than there's data left to encrypt, we will
		// snip the value shorter.
		size_t encrypt_size = std::min< size_t >(RSA_size(rsa), data_left);

		// Encrypt
		int now_encrypted;
		if (final_method == PUBLIC) {
			now_encrypted = RSA_public_encrypt(encrypt_size,
			                                   &data[encrypted], &chunk[0],
			                                   rsa, RSA_PKCS1_OAEP_PADDING);
			if (now_encrypted < 0) {
				throw Exception("RSA public encryption has failed!");
			}
		} else {
			now_encrypted = RSA_private_encrypt(encrypt_size,
			                                    &data[encrypted], &chunk[0],
			                                    rsa, RSA_PKCS1_PADDING);
			if (now_encrypted < 0) {
				throw Exception("RSA private encryption has failed!");
			}
		}
		HppAssert(now_encrypted <= RSA_size(rsa), "Unexpectedly big result!");

		// Add part to result
		result.insert(result.end(), chunk.begin(), chunk.begin() + now_encrypted);

		encrypted += encrypt_size;
	}
}

void RSAKey::decrypt(ByteV& result, ByteV const& data, Method method)
{
	if (!rsa) {
		throw Exception("Unable to decrypt because RSA key is not initialized!");
	}

	Method final_method;
	if (method == PUBLIC) {
		final_method = PUBLIC;
	} else if (method == PRIVATE) {
		if (!hasPrivatePart()) {
			throw Exception("Unable to encrypt using private part because RSA key does not have it!");
		}
		final_method = PRIVATE;
	} else {
		if (hasPrivatePart()) {
			final_method = PRIVATE;
		} else {
			final_method = PUBLIC;
		}
	}

	size_t decrypted = 0;

	ByteV chunk(RSA_size(rsa), 0);

	while (decrypted < data.size()) {

		// Calculate how much we have left to decrypt
		size_t data_left = data.size() - decrypted;

		// Calculate how big chunk we can decrypt with this RSA key.
		// If it's more than there's data left to decrypt, we will
		// snip the value shorter.
		size_t decrypt_size = std::min< size_t >(RSA_size(rsa), data_left);

		// Encrypt
		int now_decrypted;
		if (final_method == PRIVATE) {
			now_decrypted = RSA_private_decrypt(decrypt_size,
			                                    &data[decrypted], &chunk[0],
			                                    rsa, RSA_PKCS1_OAEP_PADDING);
			if (now_decrypted < 0) {
				throw Exception("RSA private decryption has failed!");
			}
		} else {
			now_decrypted = RSA_public_decrypt(decrypt_size,
			                                   &data[decrypted], &chunk[0],
			                                   rsa, RSA_PKCS1_PADDING);
			if (now_decrypted < 0) {
				throw Exception("RSA public decryption has failed!");
			}
		}
		HppAssert(now_decrypted <= RSA_size(rsa), "Unexpectedly big result!");

		// Add part to result
		result.insert(result.end(), chunk.begin(), chunk.begin() + now_decrypted);

		decrypted += decrypt_size;
	}
}

inline RSAKey RSAKey::generatePrivateKey(size_t bits)
{
	RSAKey result;

	RAND_load_file("/dev/urandom", bits / 4);

	// Generate private key
	result.rsa = RSA_new();
	if (!result.rsa) {
		throw Exception("Unable to create private RSA key!");
	}
	BIGNUM* e = BN_new();
	if (!e) {
		throw Exception("Unable to create exponent!");
	}
	BN_set_word(e, 65537);
	RSA_generate_key_ex(result.rsa, bits, e, NULL);
	BN_free(e);

	return result;
}

inline RSAKey RSAKey::generatePublicKey(void) const
{
	if (!rsa || !rsa->e || !rsa->n) {
		throw Exception("Unable to generate public key, because given key is not a proper private key!");
	}

	// Copy information from private key to public one.
	RSAKey result;
	result.rsa = RSA_new();
	if (!result.rsa) {
		throw Exception("Unable to create public RSA key!");
	}
	result.rsa->e = BN_dup(rsa->e);
	result.rsa->n = BN_dup(rsa->n);

	return result;
}

inline Json RSAKey::toJson(void) const
{
	ByteV bytes;
	serialize(bytes);
	return Json::newString(byteVToDStr(bytes));
}

inline void RSAKey::constructFromJson(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::STRING) throw Exception("JSON for RSAKey must be a string!");
	ByteV bytes = dStrToByteV(json.getString());
	deserialize(bytes);
}

inline void RSAKey::doSerialize(ByteV& result, bool bigendian) const
{
	// If key is not initialized, then mark all
	// numbers to have their length as zero.
	if (!rsa) {
		result += uInt32ToByteV(0);
		result += uInt32ToByteV(0);
		result += uInt32ToByteV(0);
		return;
	}

	try {
		result += bignumToByteV(rsa->e, bigendian);
		result += bignumToByteV(rsa->n, bigendian);
		result += bignumToByteV(rsa->d, bigendian);
	}
	catch (Exception const& e) {
		throw Exception(std::string("Unable to serialize RSAKey! Reason: ") + e.what());
	}
}

inline void RSAKey::doDeserialize(std::istream& strm, bool bigendian)
{
	cleanKey(rsa);
	BIGNUM* e = NULL;
	BIGNUM* n = NULL;
	BIGNUM* d = NULL;
	try {
		e = istreamToBignum(strm, bigendian);
		n = istreamToBignum(strm, bigendian);
		d = istreamToBignum(strm, bigendian);
	}
	catch (Exception const& e2) {
		BN_free(e);
		BN_free(n);
		BN_free(d);
		throw Exception(std::string("Unable to deserialize an RSA key! Reason: ") + e2.what());
	}

	// If all numbers have zero length, then it means there is no RSA key
	if (!e && !n && !d) {
		return;
	}

	// Create new RSA key
	rsa = RSA_new();
	if (!rsa) {
		throw Exception("Unable to deserialize an RSA key!");
	}

	// Set bignums
	rsa->e = e;
	rsa->n = n;
	rsa->d = d;
}

inline void RSAKey::cleanKey(RSA* rsa)
{
	RSA_free(rsa);
}

inline RSA* RSAKey::cloneKey(RSA* rsa)
{
	// If there is no key, then cloning is not done
	if (!rsa) return NULL;

	// Create new RSA key
	RSA* result = RSA_new();
	if (!result) {
		throw Exception("Unable to create new RSA key for cloning!");
	}

	// Copy data from another key
	if (!rsa->e || !rsa->n) {
		RSA_free(result);
		throw Exception("Unable to clone RSA key, because source key is not complete!");
	}
	result->e = BN_dup(rsa->e);
	result->n = BN_dup(rsa->n);
	result->d = BN_dup(rsa->d);
	if (!result->e || !result->n) {
		RSA_free(result);
		throw Exception("Unable to copy data from source RSA key to a new one!");
	}

	return result;
}

inline ByteV RSAKey::bignumToByteV(BIGNUM* bn, bool bigendian)
{
	if (!bn) {
		return Hpp::uInt32ToByteV(0, bigendian);
	}

	int32_t size = BN_bn2mpi(bn, NULL);
	ByteV result_raw;
	result_raw.assign(size, 0);
	int32_t bytes_stored = BN_bn2mpi(bn, &result_raw[0]);
	if (bytes_stored != size) {
		throw Exception("Unable to copy all bits of big number!");
	}

	// First four bytes represent length. But
	// just to be sure, do this our way.
	ByteV result = Hpp::uInt32ToByteV(size - 4, bigendian);
	result.insert(result.end(), result_raw.begin() + 4, result_raw.end());

	return result;
}

inline BIGNUM* RSAKey::istreamToBignum(std::istream& strm, bool bigendian)
{
	// Read size of serialized bignum. If its
	// zero, then it means there is no bignum.
	uint32_t size = deserializeUInt32(strm, bigendian);
	if (size == 0) {
		return NULL;
	}

	// Put size part back
	Hpp::ByteV bignum_serialized = uInt32ToByteV(size);
	bignum_serialized += deserializeByteV(strm, size);

	BIGNUM* result = BN_mpi2bn(&bignum_serialized[0], bignum_serialized.size(), NULL);
	if (!result) {
		throw Exception("Unable to deserialize big number!");
	}

	return result;
}

}

}

#endif
