#pragma once

#include "SIM.h"
#ifdef _WIN32
#include <atlenc.h>
#else
#include "base64_ssl.h"
#endif
#include "PublicKey.Gen.h"
#include "RSACrypto.h"

using namespace Security;

static FORCEINLINE bool CheckServerKey(const char* serverKey, const char* ip, const char* mac, std::string& errorString)
{
	assert(serverKey);

	//
	// '*'을 '='으로 변경
	//
	std::string key = serverKey;
	const std::string::size_type npos = -1;

	while (1)
	{
		std::string::size_type pos = key.find_first_of('*');
		if (npos == pos)
		{
			break;
		}

		key[pos] = '=';
	}

	//
	// base64 decoding
	//
	unsigned char buf[4096];
	int buflen = sizeof(buf);

#ifdef _WIN32
	if (FALSE == Base64Decode(key.c_str(), key.size(), buf, &buflen))
	{
		errorString = "base64 decode failed";
		return false;
	}
#else
	if (false == unbase64_ssl((unsigned char *)key.c_str(), key.size(), buf, buflen))
	{
		errorString = "base64 decode failed";
		return false;
	}
	buflen = strlen((const char *)buf);
#endif

	//
	// get public key
	//
	std::vector<unsigned char> publicKeyBuf;
	CreatePublicKey(publicKeyBuf);

	RSACrypto::PublicKey publicKey;
	if (false == RSACrypto::RestorePublicKey((const char *)&publicKeyBuf[0], publicKeyBuf.size(), &publicKey))
	{
		errorString = "restore public key failed";
		return false;
	}

	//
	// decrypt with public key
	//
	Buffer plainBuf = RSACrypto::DecryptPublic(&publicKey, buf, buflen);
	if (0 == plainBuf.buf)
	{
		errorString = "decrypt failed";
		return false;
	}

	//
	// create SIM
	//
	SIM sim;
	if (false == sim.ParseBinary(plainBuf.buf, plainBuf.len))
	{
		errorString = "parse sim binary failed";
		Buffer::Free(plainBuf);
		return false;
	}

	//
	// check ip and mac
	//
	if (false == sim.CheckIpAndMac(ip, mac))
	{
		errorString = "cannot find ip/mac info";
		return false;
	}
	return true;
}
