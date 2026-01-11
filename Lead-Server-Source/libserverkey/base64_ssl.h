#ifndef __BASE64_USING_OPENSSL_
#define __BASE64_USING_OPENSSL_

bool base64_ssl(const unsigned char *input, int length, unsigned char* output, int outlen);
bool unbase64_ssl(unsigned char *input, int length, unsigned char* output, int outlen);

#endif
