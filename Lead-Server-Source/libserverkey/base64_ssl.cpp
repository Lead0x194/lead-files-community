#include <string.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "base64_ssl.h"

bool base64_ssl(const unsigned char *input, int length, unsigned char* output, int outlen)
{
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	if (outlen < bptr->length)
	{
		BIO_free_all(b64);
		return false;
	}

	memcpy(output, bptr->data, bptr->length-1);
	output[bptr->length-1] = 0;

	BIO_free_all(b64);

	return true;
}

bool unbase64_ssl(unsigned char *input, int length, unsigned char* output, int outlen)
{
	BIO *b64, *bmem;

	memset(output, 0, outlen);

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new_mem_buf(input, length);
	bmem = BIO_push(b64, bmem);

	BIO_read(bmem, output, outlen);

	BIO_free_all(bmem);

	return true;
}
