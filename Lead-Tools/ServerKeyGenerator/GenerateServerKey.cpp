#include "ServerKey.Gen.h"
#include "SIM.h"
#include <fstream>
#include <atlenc.h>
#include "RSACrypto.h"

using namespace Security;

//
// private key를 이용하여 SIM file을 암호화 한다.
// 암호화한 SIM file은 base64 인코딩한 스트링으로 바꾸어준다. 즉, SERVER KEY = base64(enc(SIM file, private key))
// 주의: SIM file의 크기는 최대 117바이트를 넘을 수 없다.
//
int generateSERVERKEY(const char* path)
{
	// private key 가져오기
	std::vector<unsigned char> privateKeyBuf;
	CreatePrivateKey(privateKeyBuf);

	// private key 생성
	RSACrypto::PrivateKey privateKey;
	if (false == RSACrypto::RestorePrivateKey((const char *)&privateKeyBuf[0], privateKeyBuf.size(), &privateKey))
	{
		printf("Restore private key failed...\n");
		return 1;
	}

	// SIM file 로딩
	std::ifstream f;
	f.open(path);
	if (false == f.is_open())
	{
		printf("SIM file open failed...\n");
		return 1;
	}

	SIM sim;

	// read file line by line
	do
	{
		char linebuf[1024];
		f.getline(linebuf, sizeof(linebuf));

		std::string s(linebuf);

		// 주석 및 빈 줄 무시
		if (s[0] == '/' || s.size() == 0)
			continue;

		//
		// parse SIM file
		//
		if (false == sim.ParseLine(s.c_str()))
		{
			printf("ERROR: invalid line - %s\n", s.c_str());
			return 1;
		}

		printf("SIM(%u) = %s\n", f.gcount(), linebuf);
	} while (!f.eof());

	f.close();

	//
	// make SERVER KEY
	//
	std::vector<unsigned char> simbin;
	sim.MakeBinary(simbin);

	// private key로 암호화
	Buffer cipherBuf = RSACrypto::EncryptPrivate(&privateKey, &simbin[0], (int)simbin.size());
	if (0 == cipherBuf.buf)
	{
		printf("ERROR: encrypt failed\n");
		return 1;
	}

	// base64 encoding
	char base64buf[4096];
	int base64len = sizeof(base64buf);

	if (FALSE == Base64Encode((const BYTE *)cipherBuf.buf, cipherBuf.len, base64buf, &base64len, ATL_BASE64_FLAG_NOCRLF))
	{
		printf("ERROR: base64 encoding failed\n");
		return 1;
	}
	base64buf[base64len] = 0;

	// '=' 을 '*'로 변경
	std::string serverKey = base64buf;
	const std::string::size_type npos = -1;

	while (1)
	{
		std::string::size_type pos = serverKey.find_first_of('=');
		if (npos == pos)
		{
			break;
		}

		serverKey[pos] = '*';
	}

	printf("\n<SERVER KEY>\n");
	printf("%s\n\n", serverKey.c_str());

	//
	// decode SERVER KEY (for check)
	//
	//SIM simDec(serverKey.c_str());

	return 0;
}
