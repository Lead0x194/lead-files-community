/**
 * @file	CMiniUnZIP.cpp
 * @brief	ZIP에서 파일을 추출하는 클래스
 * @author	Standil (cpascal@cpascal.net)
 *
 * @version	2003/03/25 cpascal - First release
 * @version	2009/07/28 cpascal - YMIR의 SFX 용으로 포팅 (CRT 사용)
 */

#include "Metin2TorrentConsole.h"

#ifdef _SFX_ENABLE

#include "CMiniUnZIP.h"
#include <stdio.h>
#include <zlib-1.2.3/zlib.h>
#include <atlbase.h>
#include <atlconv.h>

#define ZIP_C_STORED					0	// The file is stored (no compression)
#define ZIP_C_SHRUNK					1	// The file is Shrunk
#define ZIP_C_REDUCED_FACTOR1			2	// The file is Reduced with compression factor 1
#define ZIP_C_REDUCED_FACTOR2			3	// The file is Reduced with compression factor 2
#define ZIP_C_REDUCED_FACTOR3			4	// The file is Reduced with compression factor 3
#define ZIP_C_REDUCED_FACTOR4			5	// The file is Reduced with compression factor 4
#define ZIP_C_IMPLODED					6	// The file is Imploded
#define ZIP_C_RESERVED_TOKENIZING		7	// Reserved for Tokenizing compression algorithm
#define ZIP_C_DEFLATED					8	// The file is Deflated
#define ZIP_C_ENHANCED_DEFLATED			9	// Enhanced Deflating using Deflate64(tm)
#define ZIP_C_PKWARE_DCL_IMPLODING		10	// PKWARE Data Compression Library Imploding
#define ZIP_C_RESERVED_BY_PKWARE		11	// Reserved by PKWARE
#define ZIP_C_BZIP2						12	// File is compressed using BZIP2 algorithm

#define SAFE_FCLOSE(p)					if( (p) != NULL ) { ::fclose( p ); (p) = NULL; }
#define SAFE_DELETE_ARRAY(p)			if( (p) != NULL ) { delete [] p; (p) = NULL; }

CMiniUnZIP::CMiniUnZIP( void )
: m_bOpened( FALSE ), m_fpArchive( NULL ), m_pDirectoryData( NULL ), m_papDirectory( NULL ), m_dwEntries( 0 ), m_llOffsetBeginOfZIP( 0LL )
{
	m_tszArchiveFileName[0] = _T('\0');
}

CMiniUnZIP::~CMiniUnZIP( void )
{
	CloseArchive();
}

BOOL CMiniUnZIP::OpenArchive( LPCTSTR tszFilename )
{
	CloseArchive();

	MA_StringCopy( m_tszArchiveFileName, MA_ARRAYCOUNT(m_tszArchiveFileName), tszFilename );
	errno_t err = _tfopen_s( &m_fpArchive, tszFilename, _T("rb") );
	if( err ) { 
		// ERROR: Can't open %s.
		return FALSE;
	}
	if( ReadArchiveHeader() == FALSE ) {
		CloseArchive();
		return FALSE;
	}
	return TRUE;
}

void CMiniUnZIP::CloseArchive( void )
{
	SAFE_DELETE_ARRAY( m_pDirectoryData );
	SAFE_FCLOSE( m_fpArchive );
}

#define MINIUNZIP_MAX_COMMENT_SIZE	0xFFFF
#define MINIUNZIP_COMMENTBUFFER_SIZE	0x0400

BOOL CMiniUnZIP::SearchArchiveMainHeader( ULONGLONG* plOffsetMainHeader )
{
	BYTE abyCommentBuffer[MINIUNZIP_COMMENTBUFFER_SIZE + 4];
	ULONGLONG ullMaxBack = MINIUNZIP_MAX_COMMENT_SIZE;
	ULONGLONG ullBackRead = 4;
	ULONGLONG ullFound = 0;

	::_fseeki64( m_fpArchive, 0, SEEK_END );

	ULONGLONG ullOffsetEnd = (ULONGLONG) ::_ftelli64( m_fpArchive );

	if( ullMaxBack > ullOffsetEnd )
		ullMaxBack = ullOffsetEnd;
	while( ullBackRead < ullMaxBack ) {
		ULONGLONG ullReadSize, ullReadPos;
		int i;

		if( ullBackRead+MINIUNZIP_COMMENTBUFFER_SIZE > ullMaxBack )
			ullBackRead = ullMaxBack;
		else
			ullBackRead += MINIUNZIP_COMMENTBUFFER_SIZE;
		ullReadPos = ullOffsetEnd-ullBackRead ;
		ullReadSize = ((MINIUNZIP_COMMENTBUFFER_SIZE+4) < (ullOffsetEnd-ullReadPos)) ? (MINIUNZIP_COMMENTBUFFER_SIZE+4) : (ullOffsetEnd-ullReadPos);
		::_fseeki64( m_fpArchive, ullReadPos, SEEK_SET );

		if( ::fread( abyCommentBuffer, ullReadSize, 1, m_fpArchive ) != 1 )
			break;

		for( i=(int)ullReadSize-3; (i--)>0;) {
			if( (abyCommentBuffer[i]==0x50) && (abyCommentBuffer[i+1]==0x4b) && (abyCommentBuffer[i+2]==0x05) && (abyCommentBuffer[i+3]==0x06) ) {
				ullFound = ullReadPos+i;
				break;
			}
		}

		if( ullFound != 0 )
			break;
	}
	if( plOffsetMainHeader )
		*plOffsetMainHeader = ullFound;
	return ullFound != 0;
}

BOOL CMiniUnZIP::ReadArchiveHeader( void )
{
	ZIP_MAIN_HEADER	stHeader;
	ULONGLONG		llOffsetEndOfCentral;

	if( m_fpArchive == NULL ) {
		// ERROR: Archive file is not opened.
		return FALSE;
	}

	//
	// ZIP_MAIN_HEADER
	//
	::_fseeki64( m_fpArchive, - (LONGLONG) sizeof( ZIP_MAIN_HEADER ), SEEK_END );
	llOffsetEndOfCentral = ::_ftelli64( m_fpArchive );
	::fread( &stHeader, sizeof( ZIP_MAIN_HEADER ), 1, m_fpArchive );

	if( stHeader.dwSignature != ZIP_MAIN_HEADER::SIGNATURE ) {
		// Check again if the file has comments
		if( SearchArchiveMainHeader( &llOffsetEndOfCentral ) == FALSE )
			return FALSE;
		::_fseeki64( m_fpArchive, llOffsetEndOfCentral, SEEK_SET );
		::fread( &stHeader, sizeof( ZIP_MAIN_HEADER ), 1, m_fpArchive );
		if( stHeader.dwSignature != ZIP_MAIN_HEADER::SIGNATURE )
			return FALSE;
	}

	m_llOffsetBeginOfZIP	= (LONGLONG) llOffsetEndOfCentral - (stHeader.dwDirectoryOffset + stHeader.dwDirectorySize);
	m_dwEntries				= stHeader.nDirectoryEntries;

	//
	// ZIP_INDEX_ENTRY
	//
	::_fseeki64( m_fpArchive, llOffsetEndOfCentral - stHeader.dwDirectorySize, SEEK_SET );
	m_pDirectoryData = new BYTE[ stHeader.dwDirectorySize + stHeader.nDirectoryEntries * sizeof( ZIP_INDEX_ENTRY ) ];
	if( m_pDirectoryData == NULL )
		return FALSE;
	::fread( m_pDirectoryData, stHeader.dwDirectorySize, 1, m_fpArchive );

	BYTE*	pbyCurrentFileHeader	= m_pDirectoryData;
	m_papDirectory					= (ZIP_INDEX_ENTRY **)(m_pDirectoryData + stHeader.dwDirectorySize);

	int iEntry;
	//TCHAR tszFileName[ MAX_PATH ];

	for( iEntry = 0; iEntry < stHeader.nDirectoryEntries; iEntry++ ) {
		ZIP_INDEX_ENTRY	&stFileHeader = *(ZIP_INDEX_ENTRY *) pbyCurrentFileHeader;

		m_papDirectory[iEntry] = (ZIP_INDEX_ENTRY *) pbyCurrentFileHeader;

		if( stFileHeader.dwSignature != ZIP_INDEX_ENTRY::SIGNATURE )
			break;

		pbyCurrentFileHeader += sizeof( ZIP_INDEX_ENTRY );
		pbyCurrentFileHeader += (stFileHeader.wFileNameLength + stFileHeader.wExtraLength + stFileHeader.wCommentLength );
	}
	if( iEntry != stHeader.nDirectoryEntries ) {
		SAFE_DELETE_ARRAY( m_pDirectoryData );
		return FALSE;
	}

	return TRUE;
}

DWORD CMiniUnZIP::GetFileCount( void )
{
	return m_dwEntries;
}

DWORD CMiniUnZIP::GetFileIndex( LPCTSTR IN tszFileName )
{
	ATL::CT2A	szFileName( tszFileName );

	for( DWORD dwI = 0; dwI < m_dwEntries; dwI++ ) {
		if( _strnicmp( szFileName, m_papDirectory[ dwI ]->GetFileName(), m_papDirectory[ dwI ]->wFileNameLength ) == 0 )
			return dwI;
	}
	return (DWORD) FILE_NOT_FOUND;
}

DWORD CMiniUnZIP::GetFileName( DWORD dwIndex, LPTSTR OUT tszFileName, DWORD dwSize )
{
	if( (m_pDirectoryData == NULL) || (dwIndex >= m_dwEntries) )
		return 0;
	CHAR szLocalFileName[MAX_PATH];
	strncpy_s( szLocalFileName, MAX_PATH, m_papDirectory[ dwIndex ]->GetFileName(), m_papDirectory[ dwIndex ]->wFileNameLength );
	szLocalFileName[ m_papDirectory[ dwIndex ]->wFileNameLength - 1 ] = '\0';
	ATL::CA2T	tszLocalFileName( szLocalFileName );

	MA_StringCopy( tszFileName, dwSize, tszLocalFileName );
	return _tcslen( tszFileName );
}

DWORD CMiniUnZIP::GetFileLength( DWORD dwIndex )
{
	if( (m_pDirectoryData == NULL) || (dwIndex >= m_dwEntries) )
		return 0;
	return m_papDirectory[ dwIndex ]->wUncompressedSize;
}

BOOL CMiniUnZIP::ReadFile( DWORD dwIndex, LPVOID lpBuffer )
{
	if( (m_pDirectoryData == NULL) || (dwIndex >= m_dwEntries) )
		return FALSE;
	ZIP_LOCAL_FILE_HEADER	stLocalFileHeader;
	BOOL bResult = TRUE;
	BYTE *pbyCompressedBuffer = NULL;

	::_fseeki64( m_fpArchive, m_llOffsetBeginOfZIP + m_papDirectory[ dwIndex ]->dwOffsetOfLocalHeader, SEEK_SET );
	::fread( &stLocalFileHeader, sizeof( ZIP_LOCAL_FILE_HEADER ), 1, m_fpArchive );

	if( stLocalFileHeader.dwSignature != ZIP_LOCAL_FILE_HEADER::SIGNATURE )
		return FALSE;
	if( stLocalFileHeader.wCompressedSize == 0 )
		return FALSE;

	::_fseeki64( m_fpArchive, (LONGLONG) stLocalFileHeader.wFileNameLength + stLocalFileHeader.wExtraLength, SEEK_CUR );

	switch( stLocalFileHeader.wCompression ) {
	case ZIP_C_STORED :
		if( ::fread( lpBuffer, stLocalFileHeader.wCompressedSize, 1, m_fpArchive ) != 1 )
			return FALSE;
		break;
	case ZIP_C_SHRUNK :
	case ZIP_C_REDUCED_FACTOR1 :
	case ZIP_C_REDUCED_FACTOR2 :
	case ZIP_C_REDUCED_FACTOR3 :
	case ZIP_C_REDUCED_FACTOR4 :
	case ZIP_C_IMPLODED :
	case ZIP_C_RESERVED_TOKENIZING :
		return FALSE;
	case ZIP_C_DEFLATED : {
		pbyCompressedBuffer = new BYTE[ stLocalFileHeader.wCompressedSize ];

		if( pbyCompressedBuffer == NULL )
			return FALSE;
		if( ::fread( pbyCompressedBuffer, stLocalFileHeader.wCompressedSize, 1, m_fpArchive ) != 1 ) {
			bResult = FALSE;
			break;
		}

		z_stream stream;

		stream.next_in		= (Bytef*) pbyCompressedBuffer;
		stream.avail_in		= (uInt) stLocalFileHeader.wCompressedSize;
		stream.next_out		= (Bytef*) lpBuffer;
		stream.avail_out	= stLocalFileHeader.wUncompressedSize;
		stream.zalloc		= (alloc_func) 0;
		stream.zfree		= (free_func) 0;

		// Perform inflation. wbits < 0 indicates no zlib header inside the data.
		int err = inflateInit2( &stream, -MAX_WBITS );
		if( err == Z_OK ) {
			err = inflate(&stream, Z_FINISH);
			inflateEnd(&stream);
			if( err == Z_STREAM_END )
				err = Z_OK;
			inflateEnd(&stream);
		}
		if( err != Z_OK )
			bResult = FALSE;
	}
	break;
	case ZIP_C_ENHANCED_DEFLATED :
	case ZIP_C_PKWARE_DCL_IMPLODING :
	case ZIP_C_RESERVED_BY_PKWARE :
	case ZIP_C_BZIP2 :
	default :
		return FALSE;
	}
	SAFE_DELETE_ARRAY( pbyCompressedBuffer );

	return bResult;
}

#endif