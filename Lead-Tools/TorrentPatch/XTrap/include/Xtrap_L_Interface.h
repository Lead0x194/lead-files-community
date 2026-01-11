
/***********************************************************************************************************/
/* [TAG] Xtrap_L_Interface.h | 2010.03.03 | X-TRAP Interface Library For Launcher                          */
/*                                                                                                         */
/*  Copyright (C)WiseLogic 2005 - 2010 All Rights Reserved                                                 */
/***********************************************************************************************************/

#ifndef __Xtrap_L_Interface_H
#define __Xtrap_L_Interface_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Export Definition (for Launcher)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define XTEK_ARGV_SIZE_WITHOUT_NULL				48
#define XTEK_ARGV_SIZE_WITH_NULL				49

#define XPATCH_SHOW_PATCH						0x00000001
#define XPATCH_SHOW_INTRO						0x00000002
#define XPATCH_IMAGE_PATCH						0x00000004
#define XPATCH_IMAGE_INTRO						0x00000008

#define XPATCH_SHOW_OPTION_DEFAULT				XPATCH_SHOW_PATCH | XPATCH_SHOW_INTRO
#define	XPATCH_SHOW_OPTION_SHOW_PATCH			XPATCH_SHOW_PATCH
#define XPATCH_SHOW_OPTION_SHOW_ALL				XPATCH_SHOW_PATCH | XPATCH_SHOW_INTRO
#define XPATCH_SHOW_OPTION_IMAGE_PATCH			XPATCH_SHOW_PATCH | XPATCH_IMAGE_PATCH
#define XPATCH_SHOW_OPTION_IMAGE_INTRO			XPATCH_SHOW_OPTION_SHOW_ALL | XPATCH_IMAGE_INTRO
#define XPATCH_SHOW_OPTION_IMAGE_ALL			XPATCH_SHOW_OPTION_SHOW_ALL | XPATCH_IMAGE_PATCH | XPATCH_IMAGE_INTRO

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Export Declaration (for Launcher) Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void __cdecl XTrap_L_Patch(
	IN LPCSTR	lpArgv,
	IN LPCSTR	lpGamePath, 
	IN DWORD	dwTimeout
);

void __cdecl XTrap_L_PatchC(
	IN LPCSTR	lpArgv,
	IN LPCSTR	lpGamePath, 
	IN DWORD	dwTimeout,
	IN DWORD	dwShowOption
);

void __cdecl XTrap_L_PatchG(
	IN LPCSTR	lpArgv,
	IN LPCSTR	lpGamePath, 
	IN DWORD	dwTimeout,
	IN DWORD	dwShowOption
);


void __cdecl XTrap_L_PatchEx(
	IN  LPCSTR	lpArgv, 
	IN  LPCSTR	lpGamePath, 
	IN  DWORD	dwTimeout, 
	OUT LPCSTR	pMsg, 
	OUT LPCSTR	pErrCode,
	OUT BOOL   *pErrFlag
);

BOOL __cdecl XTrap_L_ExecKey(
	OUT	void *pXTEKArgvBuff, 
	IN	DWORD dwBuffLength
);



void __cdecl XTrap_L_PatchW(
	IN LPCSTR	lpArgv,
	IN LPCWSTR	lpGamePath, 
	IN DWORD	dwTimeout
);

void __cdecl XTrap_L_PatchCW(
	IN LPCSTR	lpArgv,
	IN LPCWSTR	lpGamePath, 
	IN DWORD	dwTimeout,
	IN DWORD	dwShowOption
);

void __cdecl XTrap_L_PatchGW(
	IN LPCSTR	lpArgv,
	IN LPCWSTR	lpGamePath, 
	IN DWORD	dwTimeout,
	IN DWORD	dwShowOption
);


void __cdecl XTrap_L_PatchExW(
	IN  LPCSTR	lpArgv, 
	IN  LPCWSTR	lpGamePath, 
	IN  DWORD	dwTimeout, 
	OUT LPCWSTR	pMsg, 
	OUT LPCWSTR	pErrCode,
	OUT BOOL   *pErrFlag
);

#ifdef __cplusplus
}
#endif

#endif
