#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int generateRSAKey();
int generateSERVERKEY(const char* path);

int _tmain(int argc, _TCHAR* argv[])
{
	//
	// RSA key를 생성하고 ServerKey.Gen.h, PublicKey.Gen.h 파일을 생성한다.
	//
	if (argc == 2 && strcmp(argv[1], "-k") == 0)
	{
		return generateRSAKey();
	}

	//
	// SIM file(즉, 월드서버 및 게임서버의 ip와 mac 정보가 있는 파일)을 읽어서
	// private key로 암호화 하여 SERVER KEY를 생성한다.
	// 즉, SERVER KEY = base64(enc(SIM file, private key)) 한 값
	//
	if (argc == 3 && strcmp(argv[1], "-d") == 0)
	{
		return generateSERVERKEY(argv[2]);
	}

	//
	// help 출력
	//
	printf("help:\n");
	printf("ServerKeyGenerator.exe -k               -- generate RSA key and PublicKey.Gen.h file.\n");
	printf("ServerKeyGenerator.exe -d [SIM file]    -- generate SERVER KEY with sim(server ip/mac) file.\n");

	return 0;
}

