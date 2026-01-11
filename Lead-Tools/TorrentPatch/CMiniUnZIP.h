/**
 * @file	CMiniUnZIP.h
 * @brief	ZIP에서 파일을 추출하는 클래스. YMIR의 SFX 용으로 포팅
 * @author	Standil (cpascal@cpascal.net)
 *
 * @version	2003/03/25 cpascal - First release
 * @version	2009/07/28 cpascal - YMIR의 SFX 용으로 포팅 (CRT 사용)
 */

#ifdef _SFX_ENABLE

#ifndef _CMINIUNZIP_H_
#define _CMINIUNZIP_H_

#include "ZIP.h"

class CMiniUnZIP
{
public:
	CMiniUnZIP( void );
	~CMiniUnZIP( void );

	BOOL	OpenArchive( LPCTSTR IN tszFileName );
	void	CloseArchive( void );
	DWORD	GetFileCount( void );
	DWORD	GetFileIndex( LPCTSTR IN tszFileName );
	DWORD	GetFileName( DWORD dwIndex, LPTSTR OUT tszFileName, DWORD IN dwSize );
	DWORD	GetFileLength( DWORD IN dwIndex );
	BOOL	ReadFile( DWORD IN dwIndex, LPVOID lpBuffer );

public:
	enum {
		FILE_NOT_FOUND	= 0xFFFFFFFF
	};

protected:
	BOOL	SearchArchiveMainHeader( ULONGLONG* plOffsetMainHeader );
	BOOL	ReadArchiveHeader( void );

protected:
	BOOL				m_bOpened;
	TCHAR				m_tszArchiveFileName[ MAX_PATH ];
	FILE*				m_fpArchive;

	BYTE*				m_pDirectoryData;
	ZIP_INDEX_ENTRY**	m_papDirectory;
	DWORD				m_dwEntries;
	LONGLONG			m_llOffsetBeginOfZIP;
};

#endif /* !_CMINIUNZIP_H_ */

#endif