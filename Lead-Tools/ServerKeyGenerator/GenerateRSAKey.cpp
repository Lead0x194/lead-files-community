#include <fstream>
#include <sstream>
#include "time.h"
#include "RSACrypto.h"

using namespace Security;

//
// RSA key를 생성하여, ServerKey.Gen.h, PublicKey.Gen.h 두개의 파일을 생성한다.
// PublicKey.Gen.h에는 public key를 vector 형태로 리턴하는 CreatePublicKey() 함수가 있으며,
// ServerKey.Gen.h에는 CreatePublicKey() 함수와 private key를 vector 형태로 리턴하는 CreatePrivateKey() 함수가 있다.
//
// PublicKey.Gen.h는 게임서버 및 월드서버에서 사용하며 public key는 공개되어도 상관없다.
// ServerKey.Gen.h는 외부에 공개되어서는 안된다. (사설서버 방지 목적)
//
int generateRSAKey()
{
	InitRandomSeed();

	RSACrypto::PublicKey publicKey;
	RSACrypto::PrivateKey privateKey;
	if (false == RSACrypto::GenerateKey(&publicKey, &privateKey))
	{
		printf("RSA key generation failed\n");
		return 1;
	}

	//
	// 생성된 키값 버퍼에 저장
	//
	char privateKeyBuf[1024];
	size_t privateKeyBufLen = sizeof(privateKeyBuf);

	if (false == RSACrypto::StorePrivateKey(&privateKey, privateKeyBuf, privateKeyBufLen))
	{
		printf("RSA private key store failed\n");
		return 1;
	}

	char publicKeyBuf[1024];
	size_t publicKeyBufLen = sizeof(publicKeyBuf);

	if (false == RSACrypto::StorePublicKey(&publicKey, publicKeyBuf, publicKeyBufLen))
	{
		printf("RSA public key store failed\n");
		return 1;
	}

	//
	// 헤더파일 내용 작성
	//
	time_t now = time(NULL);

	char buf[4096];
	sprintf_s(buf, sizeof(buf),
		"#ifndef __GenerateRSAKey_%I64u__\n"
		"#define __GenerateRSAKey_%I64u__\n\n"
		"// created : %s (time_t = %I64u)\n"
		"#include <vector>\n\n", now, now, __TIMESTAMP__, now);

	std::string hdr = buf;		// 헤더파일 시작부분

	sprintf_s(buf, sizeof(buf), "\n#endif\n");
	std::string footer = buf;

	std::stringstream public_h;	// public key 생성함수 본문

	sprintf_s(buf, sizeof(buf), "inline void CreatePublicKey(std::vector<unsigned char>& key)\n{\n");
	public_h << buf;

	for (size_t i = 0; i < publicKeyBufLen; i++)
	{
		sprintf_s(buf, sizeof(buf), "	key.push_back(%u);	// %u\n", (unsigned char)publicKeyBuf[i], i);
		public_h << buf;
	}

	sprintf_s(buf, sizeof(buf), "}\n");
	public_h << buf;

	std::stringstream private_h;	// private key 생성함수 본문

	sprintf_s(buf, sizeof(buf), "inline void CreatePrivateKey(std::vector<unsigned char>& key)\n{\n");
	private_h << buf;

	for (size_t i = 0; i < privateKeyBufLen; i++)
	{
		sprintf_s(buf, sizeof(buf), "	key.push_back(%u);	// %u\n", (unsigned char)privateKeyBuf[i], i);
		private_h << buf;
	}

	sprintf_s(buf, sizeof(buf), "}\n");
	private_h << buf;

	//
	// 헤더파일 생성
	//
	std::ofstream f;
	f.open("ServerKey.Gen.h");
	if (!f.is_open())
	{
		printf("ServerKey.Gen.h file generate failed\n");
		return 1;
	}
	f << hdr;
	f << private_h.str();
	f << public_h.str();
	f << footer;
	f.close();

	f.open("PublicKey.Gen.h");
	if (!f.is_open())
	{
		printf("PublicKey.Gen.h file generate failed\n");
		return 1;
	}
	f << hdr;
	f << public_h.str();
	f << footer;
	f.close();

	return 0;
}
