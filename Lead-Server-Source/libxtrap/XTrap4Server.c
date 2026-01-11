
/***********************************************************************************************************/
/* [TAG] Xtrap4Server.c | 2010.10.06 | (Sample) X-TRAP Interface Library For Server                        */
/*                                                                                                         */
/*  Copyright (C)WiseLogic 2005 - 2013 All Rights Reserved                                                 */
/***********************************************************************************************************/

// If "stdafx.h" is being used, "XTrap4Server.c" need to be renamed as "XTrap4Server.cpp"
//#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <io.h>
#include <windows.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#endif

#include "Xtrap_S_Interface.h"

PFN_XTrap_S_Start			XTrap_S_Start=0;
PFN_XTrap_S_SessionInit		XTrap_S_SessionInit=0;
PFN_XTrap_CS_Step1			XTrap_CS_Step1=0;
PFN_XTrap_CS_Step3			XTrap_CS_Step3=0;

PFN_XTrap_S_SetActiveCode	XTrap_S_SetActiveCode=0; 
PFN_XTrap_S_SetOption		XTrap_S_SetOption=0;
PFN_XTrap_S_SetAllowDelay	XTrap_S_SetAllowDelay=0;
PFN_XTrap_S_SendGamePacket	XTrap_S_SendGamePacket=0;
PFN_XTrap_S_RecvGamePacket	XTrap_S_RecvGamePacket=0;
PFN_XTrap_S_GetDetectCode	XTrap_S_GetDetectCode=0;
PFN_XTrap_S_GetDetectLog	XTrap_S_GetDetectLog=0;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)

HINSTANCE hXTrap4Server = 0;

unsigned int XTrap_S_LoadDll()
{
	unsigned int ulError = 0;

	TCHAR sDllBinFile[]	=_T("XTrap4Server.Dll");

	hXTrap4Server = LoadLibrary(sDllBinFile);

	if (hXTrap4Server == NULL)
	{	
		ulError = GetLastError(); return 0x10000000|ulError; 
	}

	XTrap_S_Start			= (PFN_XTrap_S_Start)			GetProcAddress(hXTrap4Server, "XTrap_S_Start");
	XTrap_S_SessionInit		= (PFN_XTrap_S_SessionInit)		GetProcAddress(hXTrap4Server, "XTrap_S_SessionInit");
	XTrap_CS_Step1			= (PFN_XTrap_CS_Step1)			GetProcAddress(hXTrap4Server, "XTrap_CS_Step1");
	XTrap_CS_Step3			= (PFN_XTrap_CS_Step3)			GetProcAddress(hXTrap4Server, "XTrap_CS_Step3");
	XTrap_S_SetActiveCode	= (PFN_XTrap_S_SetActiveCode)	GetProcAddress(hXTrap4Server, "XTrap_S_SetActiveCode");
	XTrap_S_SetOption		= (PFN_XTrap_S_SetOption)		GetProcAddress(hXTrap4Server, "XTrap_S_SetOption");
	XTrap_S_SetAllowDelay	= (PFN_XTrap_S_SetAllowDelay)	GetProcAddress(hXTrap4Server, "XTrap_S_SetAllowDelay");
	XTrap_S_SendGamePacket  = (PFN_XTrap_S_SendGamePacket)	GetProcAddress(hXTrap4Server, "XTrap_S_SendGamePacket");
	XTrap_S_RecvGamePacket  = (PFN_XTrap_S_RecvGamePacket)	GetProcAddress(hXTrap4Server, "XTrap_S_RecvGamePacket");
	XTrap_S_GetDetectCode   = (PFN_XTrap_S_GetDetectCode)	GetProcAddress(hXTrap4Server, "XTrap_S_GetDetectCode");
	XTrap_S_GetDetectLog	= (PFN_XTrap_S_GetDetectLog)	GetProcAddress(hXTrap4Server, "XTrap_S_GetDetectLog");

	
	if (XTrap_S_Start			== NULL ||
		XTrap_S_SessionInit		== NULL ||
		XTrap_CS_Step1			== NULL ||
		XTrap_CS_Step3			== NULL ||
		XTrap_S_SetOption		== NULL ||
		XTrap_S_SetAllowDelay	== NULL ||
		XTrap_S_SendGamePacket	== NULL	||
		XTrap_S_RecvGamePacket	== NULL	||
		XTrap_S_GetDetectCode	== NULL ||
		XTrap_S_GetDetectLog	== NULL	)
	{
		return	0x20000000;
	}

	return XTRAP_API_RETURN_OK;
}

unsigned int XTrap_S_FreeDll()
{
	if (hXTrap4Server)
		FreeLibrary((HMODULE)hXTrap4Server);

	return XTRAP_API_RETURN_OK;
}

#else

void *hXTrap4Server	= 0;

unsigned int XTrap_S_LoadDll()
{
	unsigned int ulError = 0;

	char sDllBinFile[]	="./libXTrap4Server.so";

	hXTrap4Server = dlopen(sDllBinFile, RTLD_LAZY);

	if (hXTrap4Server == 0) 
	{
		printf("dlopen error %s\n", dlerror());

		return 0x10000000|(unsigned int)ulError;	
	}

	XTrap_S_Start		   = (PFN_XTrap_S_Start)			dlsym(hXTrap4Server, "XTrap_S_Start");
	XTrap_S_SessionInit	   = (PFN_XTrap_S_SessionInit)		dlsym(hXTrap4Server, "XTrap_S_SessionInit");
	XTrap_CS_Step1		   = (PFN_XTrap_CS_Step1)			dlsym(hXTrap4Server, "XTrap_CS_Step1");
	XTrap_CS_Step3		   = (PFN_XTrap_CS_Step3)			dlsym(hXTrap4Server, "XTrap_CS_Step3");
	XTrap_S_SetActiveCode  = (PFN_XTrap_S_SetActiveCode)	dlsym(hXTrap4Server, "XTrap_S_SetActiveCode");
	XTrap_S_SetOption	   = (PFN_XTrap_S_SetOption)		dlsym(hXTrap4Server, "XTrap_S_SetOption");
	XTrap_S_SetAllowDelay  = (PFN_XTrap_S_SetAllowDelay)	dlsym(hXTrap4Server, "XTrap_S_SetAllowDelay");
	XTrap_S_SendGamePacket = (PFN_XTrap_S_SendGamePacket)	dlsym(hXTrap4Server, "XTrap_S_SendGamePacket");
	XTrap_S_RecvGamePacket = (PFN_XTrap_S_RecvGamePacket)	dlsym(hXTrap4Server, "XTrap_S_RecvGamePacket");
	XTrap_S_GetDetectCode  = (PFN_XTrap_S_GetDetectCode)	dlsym(hXTrap4Server, "XTrap_S_GetDetectCode");
	XTrap_S_GetDetectLog   = (PFN_XTrap_S_GetDetectLog)		dlsym(hXTrap4Server, "XTrap_S_GetDetectLog");

	if (XTrap_S_Start			== NULL ||
		XTrap_S_SessionInit		== NULL ||
		XTrap_CS_Step1			== NULL ||
		XTrap_CS_Step3			== NULL ||
		XTrap_S_SetOption		== NULL ||
		XTrap_S_SetAllowDelay	== NULL ||
		XTrap_S_SendGamePacket	== NULL	||
		XTrap_S_RecvGamePacket	== NULL	||
		XTrap_S_GetDetectCode	== NULL	||
		XTrap_S_GetDetectLog	== NULL	)
	{
		return	0x20000000;
	}

	return XTRAP_API_RETURN_OK;
}

unsigned int XTrap_S_FreeDll()
{
	if (hXTrap4Server)
		dlclose(hXTrap4Server);

	return XTRAP_API_RETURN_OK;
}

#endif

unsigned int XTrap_S_LoadCMF(void* pBuf, unsigned int ulSize, unsigned int ulCount, char* pFileName, unsigned int ulMax)
{
	/*
	unsigned int	ulfcnt	= 0;
	unsigned int	ulbcnt	= 0;
	unsigned int	ulRead	= 0;

	FILE	*fi = 0;

	char	sFileName[1024]	= {0,};

	for (ulfcnt = ulMax; ulfcnt >= 0; ulfcnt--)
	{
		sprintf(sFileName, pFileName, ulfcnt);

		if (_access(sFileName, 0) == 0)
		{
			fi = fopen(sFileName, "rb");
			if (fi == NULL) return -1;

			ulRead = fread((unsigned char*)((unsigned char*)pBuf + ulbcnt*ulSize), ulSize, 1, fi);
			if (ulRead != 1) return -1;

			if (fclose(fi))	return -1;

			ulbcnt++;

			if (ulbcnt == ulCount)
				break;
		}
	}
	*/

	return 0;
}

// example)
//
// XTrap_S_LoadCMF(g_sMapBuffer, XTRAP_CS4_BUFSIZE_MAP, 2, "map%d.cs3", 255);
//
// LOAD  map1.cs3 ~ map255.cs3 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For Cloud Service
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define _XTRAP_CLOUDSERVICE_LISTFILE		"XTrap_S_CloudServerList.txt"
#define _XTRAP_CLOUDSERVICE_LIST_COUNT		20

char	g_xtrap_s_cloudservice_list[_XTRAP_CLOUDSERVICE_LIST_COUNT][20]		= {0,};
int		g_xtrap_s_cloudservice_cnt											= -1;

/*
CLIENT의 IP가 CLOUD SERVER의 IP인지 확인하는 함수.
CLIENT의 IP는 게임 서버에서 구해야 합니다.
CLOUN SERVER의 IP리스트는 "XTrap_S_CloudServerList.txt" 파일에 개행으로 관리 되어야 합니다.
example )
211.115.88.88
192.158.123.12
*/
unsigned int XTrap_S_IsCloudServer( char* server_ip )
{
	FILE	*fi = NULL;
	int		n_loop = 0;
	int		n_str_length = 0;

	// #1 FILE IO는 최초 1회만 진행 한다.
	if ( g_xtrap_s_cloudservice_cnt == -1 )
	{
		// #1-1 최초 1회만 실행되기위해 초기값을 0으로 변경해 준다.
		// g_xtrap_s_cloudservice_cnt 변수는 g_xtrap_s_cloudservice_list버퍼의 유효데이터의 갯수 이다.
		g_xtrap_s_cloudservice_cnt = 0;

		// #1-2 파일을 OPEN 한다.
		// 파일 이름은 고정되어 있다.
		fi = fopen(_XTRAP_CLOUDSERVICE_LISTFILE, "r");
		if ( fi == NULL )
		{
			// 파일이 존재하지 않는다.
			// 파일이 존재하지 않기 때문에 모두 클라우드가 아니라고 판단 한다. 그래서 0을 반환 한다.
			return 0;
		}

		// #1-3 파일의 내용을 읽어 g_xtrap_s_cloudservice_list에 기록한다.
		while( fgets( g_xtrap_s_cloudservice_list[g_xtrap_s_cloudservice_cnt], 20, fi ) != NULL )
		{
			// #1-3-1 문자열 뒤에 숫자가 아닌 부분은 제거해 준다.
			for( n_str_length = strlen( g_xtrap_s_cloudservice_list[g_xtrap_s_cloudservice_cnt] );
				 isdigit( g_xtrap_s_cloudservice_list[g_xtrap_s_cloudservice_cnt][n_str_length-1] ) == 0;
				 n_str_length-- )
			{				 
			}
			g_xtrap_s_cloudservice_list[g_xtrap_s_cloudservice_cnt][n_str_length] = NULL;

			// #1-3-2 버퍼의 한계를 계산 한다.
			// 허용가능한 CLOUD SERVER의 수를 늘리기 위해서는 _XTRAP_CLOUDSERVICE_LIST_COUNT의 값을 수정하면 된다.
			if ( g_xtrap_s_cloudservice_cnt >= _XTRAP_CLOUDSERVICE_LIST_COUNT )
			{
				break;
			}

			g_xtrap_s_cloudservice_cnt++;
		}

		// #1-4 파일을 종료 한다.
		fclose(fi);	
	}

	// #2 server_ip가 g_xtrap_s_cloudservice_list에 있는지 확인 한다.
	// 유효 데이터 수 만큼 반복문을 수행 한다.
	for( n_loop=0; n_loop < g_xtrap_s_cloudservice_cnt; n_loop++ )
	{
		// #2-1 문자열 비교를 통해 동일한 문자가 있는지 확인 한다.
		if ( strcmp( g_xtrap_s_cloudservice_list[n_loop], server_ip ) == 0 )
		{
			return 1;
		}
	}	
	return 0;
}


/*
[ CLOUD SERVICE를 지원하는 방법 1 ]
1. 게임 서버에서 CLOUD SERVER인지 판단하여 XTrap_CS_Step1() 함수를 호출하지 않게 합니다.
2. XTrap_CS_Step1() 함수 호출을 통해 나온 결과값 그리고 패킷을 게임 클라이언트에 전송하지 않습니다.

example )
{
	...
	...

	if ( b_is_cloundserver == false )
	{
		n_ret = XTrap_CS_Step1( SessionBuff, PacketBuff );
		send( client_socket, PacketBuff, PacketSize, 0 );
		if ( n_ret != 0 )
		{
			// 결과값이 0이 아니면 비정상 클라이언트로 판단되어 연결을 종료 합니다.
			close( client_socket );
		}
	}
}




[ CLOUD SERVICE를 지원하는 방법 2 ]
1. 제공된 XTrap_S_IsClounServer() 함수를 이용하여 CLOUD SERVER인지 판단을 합니다.
2. XTrap_CS_Step1() 관련된 부분을 호출되지 않게 합니다.

example )
{
	...
	...  
	// 연결된 CLIENT의 IP를 이용하여 등록된 CLOUD SERVER인지 판단을 합니다.
	// CLOUD SERVER이면 XTrap_CS_STep1() 함수를 호출하지 않습니다.
	if ( XTrap_S_IsCloudServer( "211.115.100.100" ) == 0 )
	{
		n_ret = XTrap_CS_Step1( SessionBuff, PacketBuff );
		send( client_socket, PacketBuff, PacketSize, 0 );
		if ( n_ret != 0 )
		{
			// 결과값이 0이 아니면 비정상 클라이언트로 판단되어 연결을 종료 합니다.
			close( client_socket );
		}
	}
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
