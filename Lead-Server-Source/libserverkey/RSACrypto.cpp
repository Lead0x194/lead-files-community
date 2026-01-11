#include "RSACrypto.h"
#ifdef _WIN32
#include <atlenc.h>
#endif
#include <assert.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/engine.h>
#include <openssl/sha.h>
#include <openssl/pem.h>

namespace Security
{
	static const char rnd_seed[] = "alsfkdj#$^#Y$JBGVKA()#$J@J#OTJG)(@JG)@GJ)J@$)JG)$JG)GJ#@)G";
	class StaticInitializer
	{
	public:
		StaticInitializer()
		{
		}
		~StaticInitializer()
		{
			RAND_cleanup();
		}

		void Init()
		{
			RAND_seed( rnd_seed, sizeof( rnd_seed ) );
		}
	};

	void InitRandomSeed()
	{
		static StaticInitializer s;
		s.Init();
	}

	// RSA cryptography
	//RSACrypto::PublicKey::PublicKey(const unsigned char* n, int nsize, const unsigned char* e, int esize)
	//{
	//	rsa_ = Alloc();

	//	BIGNUM* tmp = NULL;
	//	tmp = BN_bin2bn( n, nsize, rsa_->n );
	//	assert( tmp );
	//	tmp = BN_bin2bn( e, esize, rsa_->e );
	//	assert( tmp );

	//	//#ifdef _DEBUG
	//	//			printf("n:");
	//	//			BN_print_fp(stdout, rsa_->n);
	//	//			printf("");
	//	//			printf("e:");
	//	//			BN_print_fp(stdout, rsa_->e);
	//	//			printf("");
	//	//#endif
	//}

	RSACrypto::PublicKey::PublicKey(const char* n, const char* e)
	{
		rsa_ = Alloc();

		BN_hex2bn(&rsa_->n, n);
		BN_hex2bn(&rsa_->e, e);
	}

	RSACrypto::PublicKey::PublicKey(RSACrypto::PublicKey& p)
	{
		rsa_ = RSA_new();
		assert( rsa_ );
		Copy( rsa_, p.rsa_ );
	}

	RSACrypto::PublicKey::~PublicKey()
	{
		Free( rsa_ );
		rsa_ = NULL;
	}

	RSACrypto::PublicKey& RSACrypto::PublicKey::operator =(const RSACrypto::PublicKey& p)
	{
		if ( rsa_ )
		{
			Free( rsa_ );
			rsa_ = NULL;
		}

		rsa_ = RSA_new();
		assert( rsa_ );
		Copy( rsa_, p.rsa_ );

		return *this;
	}

	//Buffer RSACrypto::PublicKey::GetN()
	//{
	//	if ( rsa_ )
	//	{
	//		int len = BN_num_bytes( rsa_->n );
	//		Buffer n = Buffer::Alloc( len );
	//		if ( NULL == n.buf )
	//		{
	//			return Buffer();
	//		}

	//		BN_bn2bin( rsa_->n, (unsigned char *)n.buf );
	//		return n;
	//	}

	//	return Buffer();
	//}

	//Buffer RSACrypto::PublicKey::GetE()
	//{
	//	if ( rsa_ )
	//	{
	//		int len = BN_num_bytes( rsa_->e );
	//		Buffer e = Buffer::Alloc( len );
	//		if ( NULL == e.buf )
	//		{
	//			return Buffer();
	//		}

	//		BN_bn2bin( rsa_->e, (unsigned char *)e.buf );
	//		return e;
	//	}

	//	return Buffer();
	//}

	RSACrypto::PublicKey::PublicKey() : rsa_( NULL )
	{
	}

	RSA* RSACrypto::PublicKey::Alloc()
	{
		RSA* rsa = RSA_new();
		assert( rsa );

		rsa->n = BN_new();
		rsa->e = BN_new();

		return rsa;
	}

	void RSACrypto::PublicKey::Free(rsa_st* p)
	{
		if ( p )
		{
			RSA_free( p );
		}
	}

	void RSACrypto::PublicKey::Copy(rsa_st* to, const rsa_st* from)
	{
		BN_copy( to->n, from->n );
		BN_copy( to->e, from->e );
	}

	RSACrypto::PrivateKey::PrivateKey() : rsa_( NULL )
	{
	}

	RSACrypto::PrivateKey::PrivateKey(rsa_st* rsa) : rsa_( rsa )
	{
	}

	RSACrypto::PrivateKey::~PrivateKey()
	{
		if ( rsa_ )
		{
			RSA_free( rsa_ );
			rsa_ = NULL;
		}
	}

	RSA* RSACrypto::PrivateKey::Alloc()
	{
		RSA* rsa = RSA_new();
		assert( rsa );

		rsa->d = BN_new();
		rsa->p = BN_new();
		rsa->q = BN_new();

		return rsa;
	}

	Buffer RSACrypto::EncryptPublic(const RSACrypto::PublicKey* k, const unsigned char* plain, int plainLen)
	{
		int rsaSize = RSA_size( k->rsa_ );
		Buffer cipherText = Buffer::Alloc( rsaSize );
		if ( NULL == cipherText.buf )
		{
			return Buffer();
		}

		// must be checked when RSA_PKCS1_OAEP_PADDING mode
		if ( plainLen >= rsaSize - 41 )
		{
			Buffer::Free(cipherText);
			assert(false);
			return Buffer();
		}

		int cipherTextLen = RSA_public_encrypt(
			plainLen,
			plain,
			(unsigned char *)cipherText.buf,
			k->rsa_,
			RSA_PKCS1_OAEP_PADDING);
		if ( -1 == cipherTextLen )
		{
			Buffer::Free(cipherText);
			return Buffer();
		}

		assert( cipherTextLen == rsaSize );
		//XSystem::MemoryPool::MemoryPool_Realloc( cipherText, cipherTextLen );
		return cipherText;
	}

	Buffer RSACrypto::DecryptPrivate(const RSACrypto::PrivateKey* k, const unsigned char* cipher, int cipherLen)
	{
		int rsaSize = RSA_size( k->rsa_ );
		Buffer plainText = Buffer::Alloc( rsaSize );
		if ( NULL == plainText.buf )
		{
			return Buffer();
		}

		int plainTextLen = RSA_private_decrypt(
			cipherLen,
			cipher,
			(unsigned char *)plainText.buf,
			k->rsa_,
			RSA_PKCS1_OAEP_PADDING);
		if ( -1 == plainTextLen )
		{
			Buffer::Free(plainText);
			return Buffer();
		}

		plainText.len = plainTextLen;

		return plainText;
	}

	Buffer RSACrypto::EncryptPrivate(const RSACrypto::PrivateKey* k, const unsigned char* plain, int plainLen)
	{
		int rsaSize = RSA_size( k->rsa_ );
		Buffer cipherText = Buffer::Alloc( rsaSize );
		if ( NULL == cipherText.buf )
		{
			return Buffer();
		}

		// must be checked when RSA_PKCS1_PADDING mode (private encrypt에서는 다른 padding을 지원안한다.)
		if ( plainLen >= rsaSize - 11 )
		{
			Buffer::Free(cipherText);
			assert(false);
			return Buffer();
		}

		int cipherTextLen = RSA_private_encrypt(
			plainLen,
			plain,
			(unsigned char *)cipherText.buf,
			k->rsa_,
			RSA_PKCS1_PADDING);
		if ( -1 == cipherTextLen )
		{
			Buffer::Free(cipherText);
			return Buffer();
		}

		assert( cipherTextLen == rsaSize );
		//XSystem::MemoryPool::MemoryPool_Realloc( cipherText, cipherTextLen );
		return cipherText;
	}

	Buffer RSACrypto::DecryptPublic(const RSACrypto::PublicKey* k, const unsigned char* cipher, int cipherLen)
	{
		int rsaSize = RSA_size( k->rsa_ );
		Buffer plainText = Buffer::Alloc( rsaSize );
		if ( NULL == plainText.buf )
		{
			return Buffer();
		}

		int plainTextLen = RSA_public_decrypt(
			cipherLen,
			cipher,
			(unsigned char *)plainText.buf,
			k->rsa_,
			RSA_PKCS1_PADDING);
		if ( -1 == plainTextLen )
		{
			Buffer::Free(plainText);
			return Buffer();
		}

		plainText.len = plainTextLen;

		return plainText;
	}

	bool RSACrypto::GenerateKey(RSACrypto::PublicKey* publicKey, RSACrypto::PrivateKey* privateKey)
	{
		RSA* rsa = RSA_generate_key( 1024, 7, NULL, NULL );
		if ( NULL == rsa )
		{
			//ERR_get_error();
			return false;
		}

		if ( 1 != RSA_check_key( rsa ) )
		{
			//ERR_get_error();
			return false;
		}

		publicKey->rsa_ = publicKey->Alloc();
		publicKey->Copy( publicKey->rsa_, rsa );
		privateKey->rsa_ = rsa;

		#ifdef _DEBUG
		//			printf("n:");
		//			BN_print_fp(stdout, publicKey->rsa_->n);
		//			printf("");
		//			printf("e:");
		//			BN_print_fp(stdout, publicKey->rsa_->e);
		//			printf("");

		//char buf[1024];
		//BIO* bp = BIO_new_mem_buf(buf, sizeof(buf));
		//PEM_write_bio_RSAPrivateKey(bp, rsa, 0, 0, 0, 0, 0);

		//PEM_read_bio_RSAPrivateKey(bp, )
		#endif

		return true;
	}

	bool RSACrypto::PrintKey(const PublicKey* k, std::string& e, std::string& n)
	{
		char* tmp = BN_bn2hex(k->rsa_->e);
		if (!tmp)
		{
			return false;
		}
		e = tmp;

		tmp = BN_bn2hex(k->rsa_->n);
		if (!tmp)
		{
			return false;
		}
		n = tmp;

		return true;
	}

	bool RSACrypto::PrintKey(const PrivateKey* k, std::string& n, std::string& e, std::string& d)
	{
		char* tmp = BN_bn2hex(k->rsa_->n);
		if (!tmp)
		{
			return false;
		}
		n = tmp;

		tmp = BN_bn2hex(k->rsa_->e);
		if (!tmp)
		{
			return false;
		}
		e = tmp;

		tmp = BN_bn2hex(k->rsa_->d);
		if (!tmp)
		{
			return false;
		}
		d = tmp;

		return true;
	}

	bool RSACrypto::StorePrivateKey(const PrivateKey* k, char* buf, size_t& buflen)
	{
		// DER 포맷으로 변환
		char* tmp = NULL;
		int n = i2d_RSAPrivateKey(k->rsa_, (unsigned char **)&tmp);
		if (n < 0)
		{
			return false;
		}

		if (n > (int)buflen)
		{
			printf("RSACrypto::StorePrivateKey: buflen is too small\n");
			return false;
		}

		memcpy(buf, tmp, n);
		buflen = n;

		free(tmp);
		return true;
	}

	bool RSACrypto::RestorePrivateKey(const char* buf, size_t buflen, PrivateKey* k)
	{
		// DER 포맷에서 변환
		char* tmp = (char *)malloc(buflen);
		if (!tmp)
		{
			return false;
		}
		memcpy(tmp, buf, buflen);

		if (!d2i_RSAPrivateKey(&k->rsa_, (const unsigned char **)&tmp, buflen))
		{
			free(tmp);
			return false;
		}

		return true;
	}

	bool RSACrypto::StorePublicKey(const PublicKey* k, char* buf, size_t& buflen)
	{
		// DER 포맷으로 변환
		char* tmp = NULL;
		int n = i2d_RSAPublicKey(k->rsa_, (unsigned char **)&tmp);
		if (n < 0)
		{
			return false;
		}

		if (n > (int)buflen)
		{
			printf("RSACrypto::StorePublicKey: buflen is too small\n");
			return false;
		}

		memcpy(buf, tmp, n);
		buflen = n;

		free(tmp);
		return true;
	}

	bool RSACrypto::RestorePublicKey(const char* buf, size_t buflen, PublicKey* k)
	{
		// DER 포맷에서 변환
		char* tmp = (char *)malloc(buflen);
		if (!tmp)
		{
			return false;
		}
		memcpy(tmp, buf, buflen);

		if (!d2i_RSAPublicKey(&k->rsa_, (const unsigned char **)&tmp, buflen))
		{
			free(tmp);
			return false;
		}

		return true;
	}

	Buffer SHA1::Digest(const Buffer& plain)
	{
		Buffer result = Buffer::Alloc( 20 );
		::SHA1( (const unsigned char *)plain.buf,
			(unsigned long)plain.len,
			(unsigned char *)result.buf);

		return result;
	}
}
