#ifndef __RSA_Crypto_H__
#define __RSA_Crypto_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>

#ifdef _WIN32
// needed only for using openssl
#pragma comment(lib, "libeay32")
#endif

struct rsa_st;
struct evp_cipher_ctx_st;

namespace Security
{
	struct Buffer
	{
		Buffer(char* p = 0, size_t c = 0) : buf(p), len(c) {};

		char* buf;
		size_t len;

		static Buffer Alloc(size_t len)
		{
			Buffer tmp;
			tmp.len = len;
			tmp.buf = new char[len];

			return tmp;
		}

		static void Free(Buffer& buf)
		{
			delete [] buf.buf;
			memset(&buf, 0, sizeof(Buffer));
		}
	};

	void InitRandomSeed();

	class RSACrypto
	{
	public:
		class PublicKey
		{
			friend class RSACrypto;
		public:
			PublicKey();
			PublicKey(const char* n, const char* e);
			PublicKey(PublicKey& p);
			~PublicKey();

			PublicKey& operator=(const PublicKey& p);
			//Buffer GetN();
			//Buffer GetE();

		private:
			rsa_st* Alloc();
			void Free(rsa_st* p);
			void Copy(rsa_st* to, const rsa_st* from);

		private:
			rsa_st* rsa_;
		};

		class PrivateKey
		{
			friend class RSACrypto;
		public:
			PrivateKey();
			PrivateKey(rsa_st* rsa);
			~PrivateKey();

		private:
			rsa_st* Alloc();

		private:
			rsa_st* rsa_;
		};

		static Buffer EncryptPublic(const PublicKey* k, const unsigned char* plain, int plainLen);
		static Buffer DecryptPrivate(const PrivateKey* k, const unsigned char* cipher, int cipherLen);
		static Buffer EncryptPrivate(const PrivateKey* k, const unsigned char* plain, int plainLen);
		static Buffer DecryptPublic(const PublicKey* k, const unsigned char* cipher, int cipherLen);
		static bool GenerateKey(PublicKey* publicKey, PrivateKey* privateKey);
		static bool PrintKey(const PublicKey* k, std::string& e, std::string& n);
		static bool PrintKey(const PrivateKey* k, std::string& d, std::string& p, std::string& q);
		//
		// private, public key를 DER 포맷으로 변환하여 buf에 저장
		//
		static bool StorePrivateKey(const PrivateKey* k, char* buf, size_t& buflen);
		static bool RestorePrivateKey(const char* buf, size_t buflen, PrivateKey* k);
		static bool StorePublicKey(const PublicKey* k, char* buf, size_t& buflen);
		static bool RestorePublicKey(const char* buf, size_t buflen, PublicKey* k);
	};

	class SHA1
	{
	public:
		static Buffer Digest(const Buffer& plain);
	};
}

#endif

