#include "metin2TorrentConsole.h"
#include "CrcPatcher.h"
#include "TorrentPatch.h"
#include "PatchThread.h"

#include <Earth/EA_FileUtility.h>
#include <Mantle/MA_Path.h>
#include <Mantle/MA_String.h>
#include <Mantle/MA_CURLEncode.h>

#include <minilzo-2.03/minilzo.h>
#include <Wininet.h>

#include "CCrcTable.h"
#include "Helper.h"
#include "StringFromHttp.h"
#include "StringFromFile.h"

#include <EterBase/FileName.h>

#define HTTP_PREFIX		_T("http://")

EL_CrcPatcher::EL_CrcPatcher()
: m_pcPatchThread(NULL)
, m_nCurrentFileIndex(0), m_nFileCount(0)
, m_ullCurrentDownloadedBytes(0ULL), m_ullCurrentFileSize(0ULL)
, m_ullTotalDownloadedBytes(0ULL), m_ullTotalSize(0ULL)
, m_dwStartTime(0), m_ullStartBytes(0ULL)
{
}

EL_CrcPatcher::EL_CrcPatcher(EL_PatchThread* pcPatchThread)
: m_pcPatchThread(pcPatchThread)
, m_nCurrentFileIndex(0), m_nFileCount(0)
, m_ullCurrentDownloadedBytes(0ULL), m_ullCurrentFileSize(0ULL)
, m_ullTotalDownloadedBytes(0ULL), m_ullTotalSize(0ULL)
, m_dwStartTime(0), m_ullStartBytes(0ULL)
{
}

EL_CrcPatcher::~EL_CrcPatcher()
{
}

void EL_CrcPatcher::Clear( void )
{
	m_nFileCount	= 0;
	m_ullTotalSize	= 0;
	m_dwStartTime	= 0;
}

MA_BOOL EL_CrcPatcher::Run(EL_PatchConfig& stConfig, const PatchInfo& stPatchInfo, Earth::EA_CStatus *pcStatus)
{
	if (false == _IsAlive())
		return false;

	MA_TCHAR tszCRCLIST_URL[MA_MAX_PATH];
	MA_TCHAR tszCRCBASE_URL[MA_MAX_PATH];

	MA_PathMerge( tszCRCBASE_URL, MA_ARRAYCOUNT(tszCRCBASE_URL), stPatchInfo.crcpatchUrl.c_str(), stPatchInfo.crcpatchVersion.c_str() );
	MA_PathMerge( tszCRCLIST_URL, MA_ARRAYCOUNT(tszCRCLIST_URL), tszCRCBASE_URL, _T("crclist") );
	MA_StringAppend( tszCRCBASE_URL, MA_ARRAYCOUNT(tszCRCBASE_URL), _T("/") );
	MA_PathStandardizeURI( tszCRCLIST_URL );
	MA_PathStandardizeURI( tszCRCBASE_URL );

	m_pcPatchThread->SetUIMainState( EL_LTEXT("TORRENT_STATE_DOWNLOADING_METADATA") );
	m_pcPatchThread->UpdateUI();

	std::string tableData;
	if (false == EL_StringFromHttpW(tableData, tszCRCLIST_URL, pcStatus))
		return MA_FALSE;

	const std::wstring basePath = stConfig.saveDir + MA_W("/");
	MA_BOOL bRet = Patch(tableData, tszCRCBASE_URL, basePath, basePath, pcStatus);
	return bRet;
}

MA_BOOL EL_CrcPatcher::Patch(const EL_CCrcTable& rcTable, const std::wstring& baseUrl, const std::wstring& basePath, const std::wstring& savePath, Earth::EA_CStatus *pcStatus)
{
	if (false == _IsAlive())
		return MA_FALSE;

	Clear();
	EL_SIZE sizeTable = rcTable.Count();
	if (sizeTable == 0)
	{
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("INVALID_CRC_TABLE") );
		return EL_FALSE;
	}

	EL_CCrcTable cDiffTable;

	m_pcPatchThread->SetUIMainState( EL_LTEXT("TORRENT_STATE_CHECKING") );
	m_pcPatchThread->SetUISubState( L"" );
	m_pcPatchThread->SetUINameState( L"" );
	m_pcPatchThread->UpdateUI();

	MA_INT nCheckedFiles = 0;

	BOOST_FOREACH(const EL_FileInfo& info, rcTable.GetFiles())
	{
		if( false == _IsAlive() )
			return MA_FALSE;
		if( MA_FALSE == m_pcPatchThread->UpdateSpawning() )
			return MA_FALSE;

		bool bDiffFile = EL_TRUE;
		const std::wstring path = basePath + info.path;

		HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile != INVALID_HANDLE_VALUE ) {
			LARGE_INTEGER llFileSize;
			FILETIME stModifiedTime;
			if ( ::GetFileSizeEx (hFile, &llFileSize) && ::GetFileTime (hFile, NULL, NULL, &stModifiedTime) ) {
				if (llFileSize.QuadPart == info.size) {
					if( stModifiedTime.dwHighDateTime == info.dwTimeHigh && stModifiedTime.dwLowDateTime == info.dwTimeLow)
						bDiffFile = EL_FALSE;
					else {
						MA_SAFE_CLOSEHANDLE(hFile);
						EL_UINT32 dwLocalCRC32 = 0;
						if (EL_PatchThread::sGetCRC32(path.c_str(), &dwLocalCRC32) == EL_TRUE) {
							if(dwLocalCRC32 == info.CRC32) {
								EL_SetFileModifiedTime(path.c_str(), info.dwTimeHigh, info.dwTimeLow);
								bDiffFile = EL_FALSE;
							}
						}
					}
				}
			}
			MA_SAFE_CLOSEHANDLE(hFile);
		}

		if (bDiffFile) {
			cDiffTable.Add(info);
			m_nFileCount++;
			m_ullTotalSize += info.size;
		}

		++nCheckedFiles;

		m_pcPatchThread->SetUICurrentProgress( MA_FLOAT(nCheckedFiles) / MA_FLOAT(rcTable.Count()) , MA_FALSE );
		m_pcPatchThread->UpdateUI();
	}

	if (m_nFileCount == 0) {
		std::wstring emptyString = L"";
		_OnCompletedDownload(emptyString, 0);
		return EL_TRUE;
	}

	m_pcPatchThread->SetUIMainState( EL_LTEXT("TORRENT_TOTAL_PROGRESS") );
	m_pcPatchThread->SetUISubState( EL_LTEXT("TORRENT_STATE_DOWNLOADING") );
	m_pcPatchThread->SetUINameState( L"" );
	m_pcPatchThread->UpdateUI();

	BOOST_FOREACH(const EL_FileInfo& info, cDiffTable.GetFiles())
	{
		if (false == _IsAlive())
			return EL_FALSE;

		const std::wstring saveFilePath = savePath + info.path;
		const std::wstring downloadURL = baseUrl + info.path + L".lz";

		m_strCurrentFileName		= info.path;
		m_ullCurrentFileSize		= info.size;
		m_ullCurrentDownloadedBytes	= 0;
		UpdateProgress();

		if (false == _DownloadFileFromHttp(downloadURL.c_str(), saveFilePath.c_str(), info.path, pcStatus))
		{
			if (pcStatus && _IsAlive())
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("%s - %s"), EL_LTEXT("FAILED_DOWNLOAD_FILE"), downloadURL.c_str() );
			return EL_FALSE;
		}

		//_OnProgressDownload(info.path, info.size, info.size);

		//패키지가 업데이트 되면 cache 패키지도 지워야 한다.
		char pszDiffFilePath[MAX_PATH+1];
		wcstombs( pszDiffFilePath, saveFilePath.c_str(), MAX_PATH );

		std::string diffFileName( pszDiffFilePath );

		std::string extName = CFileNameHelper::GetExtension(diffFileName);
		if( !extName.compare("epk") )
		{
			std::string cachedPackName = CFileNameHelper::NoExtension(diffFileName) + "_texcache.epk";
			if (_access(cachedPackName.c_str(), 0) == 0 )
			{
				::DeleteFileA( cachedPackName.c_str() );
			}
			std::string cachedPackIVName = CFileNameHelper::NoExtension(diffFileName) + "_texcache.eix";
			if (_access(cachedPackIVName.c_str(), 0) == 0 )
			{
				::DeleteFileA( cachedPackIVName.c_str() );
			}
		}

		_OnCompletedDownload(info.path, info.size);
	}

	_OnFinished();

	return MA_TRUE;
}

MA_BOOL EL_CrcPatcher::Patch(const std::string& tableData, const std::wstring& baseUrl, const std::wstring& basePath, const std::wstring& savePath, Earth::EA_CStatus *pcStatus)
{
	if (false == _IsAlive())
		return MA_FALSE;

	EL_CCrcTable cTable;
	if (false == cTable.Load(tableData))
	{
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("INVALID_CRC_TABLE") );
		return MA_FALSE;
	}

	return Patch(cTable, baseUrl, basePath, savePath, pcStatus); 
}

EL_INT EL_CrcPatcher::_GetDataFromHttpW( std::vector<EL_BYTE>* outBuffer, const std::wstring& fileUrl, Earth::EA_CStatus *pcStatus, const std::wstring& /*fileName*/ )
{
	static const EL_WCHAR_P PROTOCOL_SEP = L"://";
	static const EL_SIZE    PROTOCOL_SEP_LEN = wcslen(PROTOCOL_SEP);

	const EL_INT protocolSepPos = fileUrl.find(PROTOCOL_SEP, 0, PROTOCOL_SEP_LEN);
	const EL_INT protocolLen = (protocolSepPos >= 0) ? protocolSepPos + PROTOCOL_SEP_LEN : 0;

	MA_U64 ullBackupCurrentDownloadedBytes	= m_ullCurrentDownloadedBytes;
	MA_U64 ullBackupTotalDownloadedBytes	= m_ullTotalDownloadedBytes;
	MA_U64 ullStartOfFile					= 0;
	outBuffer->clear();

	const std::wstring protocol = fileUrl.substr(0, protocolLen);	
	if (protocolLen > 0)
	{
		if( MA_StringNCompare( protocol.c_str(), HTTP_PREFIX, protocolLen ) != 0 ) {
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("CHECK_PROTOCOL_ERROR"), protocol.c_str(), HTTP_PREFIX );
			return -1;
		}
	}
	
	std::wstring fileUrlAddr;
	std::wstring fileUrlPath;

	const EL_INT firstSepPos = fileUrl.find(L'/', protocolLen);
	if (firstSepPos > 0)
	{
		fileUrlAddr.assign(fileUrl.substr(protocolLen, firstSepPos-protocolLen));
		fileUrlPath.assign(fileUrl.substr(firstSepPos));
	}
	else
	{
		fileUrlAddr.assign(fileUrl.substr(protocolLen));
	}

	int nRetryPerFile = 5;
	MA_FSIZE sizeFileSize = 0;
	BOOL bRet = FALSE;
	HINTERNET hRequest = NULL;
	HINTERNET hConnection = NULL;
	Mantle::MA_CURLEncode cURLEncoder;
	Mantle::MA_CW2E eszFileUrlPath( fileUrlPath.c_str() );

	HINTERNET hSession = InternetOpenW(TAPP_FULLNAME, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if( !hSession ) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_OPEN_ERROR"), GetLastError() );
		return FALSE;
	}

	//DWORD dwTimeout = 20000;

	//InternetSetOption(hSession, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, (void *)&dwTimeout, sizeof(dwTimeout));
	//InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, (void*)&dwTimeout, sizeof(dwTimeout));
	//InternetSetOption(hSession, INTERNET_OPTION_DATA_SEND_TIMEOUT, (void*)&dwTimeout, sizeof(dwTimeout));
	//InternetSetOption(hSession, INTERNET_OPTION_SEND_TIMEOUT, (void*)&dwTimeout, sizeof(dwTimeout));

TryResume:
	EL_SAFE_INTERNETCLOSEHANDLE(hRequest);
	EL_SAFE_INTERNETCLOSEHANDLE(hConnection);

	hConnection = InternetConnect(hSession, fileUrlAddr.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if( !hConnection ) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_CONNECT_ERROR"), fileUrlAddr.c_str(), GetLastError() );
		goto Cleanup;
	}

	MA_SIZE sizeRequestPath = cURLEncoder.CalculateEncodeLength( MA_StringLengthE( eszFileUrlPath ) );
	MA_LPTSTR tszRequestPath = MA_StringAlloc( sizeRequestPath );
	cURLEncoder.Encode( tszRequestPath, sizeRequestPath, (MA_LPCESTR) eszFileUrlPath, MA_StringLengthE( eszFileUrlPath ) );

	hRequest = HttpOpenRequestW(hConnection,
		L"GET",
		tszRequestPath,
		_T("HTTP/1.1"),
		NULL,
		NULL,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE,
		0);

	MA_StringFree(tszRequestPath);
	if (!hRequest) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_OPEN_REQUEST_ERROR"), fileUrlPath.c_str(), GetLastError() );
		goto Cleanup;
	}

	// Send request
	{
		MA_WCHAR wszExtraHeader[256];
		DWORD dwExtraHeaderSize = 0;

		memset(wszExtraHeader, 0, sizeof(wszExtraHeader));
		if(ullStartOfFile != 0)
			dwExtraHeaderSize = MA_StringFormatW(wszExtraHeader, MA_ARRAYCOUNT(wszExtraHeader), _T("Range: bytes=%I64u-\r\n"), ullStartOfFile);
		if( HttpSendRequestW(hRequest, wszExtraHeader, dwExtraHeaderSize, NULL, 0) == FALSE )
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_SEND_REQUEST_ERROR"), fileUrlPath.c_str(), GetLastError() );
			goto Cleanup;
		}
	}

	// Query http status code
	{
		DWORD dwStatus = 0;
		DWORD dwStatusSize = sizeof(dwStatus);
		if (!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwStatusSize, NULL))
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_QUERY_STATUS_CODE_ERROR"), fileUrlPath.c_str(), GetLastError() );
			goto Cleanup;
		}

		if( (HTTP_STATUS_OK != dwStatus) && (HTTP_STATUS_PARTIAL_CONTENT != dwStatus) ) {
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_QUERY_STATUS_NOT_OK"), fileUrlPath.c_str(), GetLastError(), dwStatus );
			goto Cleanup;
		}
	}

	// Query content-length
	{
		MA_TCHAR tszContentLength[32];
		DWORD dwQuerySize = sizeof(tszContentLength);

		if( HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, tszContentLength, &dwQuerySize, NULL) == FALSE ) {
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("%s (path='%s', error=%d)"), EL_LTEXT("HTTP_QUERY_CONTENT_LENGTH_ERROR"), fileUrlPath.c_str(), GetLastError() );
			goto Cleanup;
		}

		sizeFileSize = (EL_SIZE)_ttoi64( tszContentLength );
	}

	if(sizeFileSize != outBuffer->size())
		outBuffer->resize(sizeFileSize);
	
	MA_FSIZE ullReceivedPosition = ullStartOfFile;
	DWORD dwBytesAvailable = 0;
	while( ullReceivedPosition < sizeFileSize )
	{
		if (false == _IsAlive())
			goto Cleanup;
		//if (FALSE == InternetQueryDataAvailable(hRequest, &dwBytesAvailable, 0, 0)) {
		//		if( pcStatus )
		//			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_QUERY_DATA_AVAILABLE_ERROR"), fileUrlPath.c_str(), GetLastError() );
		//		goto Cleanup;
		//}
		//if (false == _IsAlive())
		//	return FALSE;

		dwBytesAvailable = EL_MIN<MA_FSIZE>( sizeFileSize - ullReceivedPosition, 4096 );

		DWORD dwReadSize = 0;
		if (dwBytesAvailable == 0)
			break;
		if (!InternetReadFile(hRequest, &*(outBuffer->begin()) + ullReceivedPosition, dwBytesAvailable, &dwReadSize))
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_READ_FILE_ERROR"), fileUrlPath.c_str(), GetLastError(), ullReceivedPosition, sizeFileSize );
			::Sleep( 1000 );
			if(nRetryPerFile <= 0)
				goto Cleanup;
			nRetryPerFile--;
			ullStartOfFile = ullReceivedPosition;
			goto TryResume;
		}
		if (dwReadSize == 0) {
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_READ_FILE_ERROR"), fileUrlPath.c_str(), GetLastError(), ullReceivedPosition, sizeFileSize );
			::Sleep( 1000 );
			if(nRetryPerFile <= 0)
				goto Cleanup;
			nRetryPerFile--;
			ullStartOfFile = ullReceivedPosition;
			goto TryResume;
		}
		ullReceivedPosition			+= dwReadSize;
		m_ullCurrentDownloadedBytes	+= dwReadSize;
		m_ullTotalDownloadedBytes	+= dwReadSize;

		CalculateSpeed( ullReceivedPosition, sizeFileSize );
	}

	bRet = TRUE;

Cleanup:
	EL_SAFE_INTERNETCLOSEHANDLE(hRequest);
	EL_SAFE_INTERNETCLOSEHANDLE(hConnection);
	EL_SAFE_INTERNETCLOSEHANDLE(hSession);

	if( bRet == FALSE ) {
		m_ullCurrentDownloadedBytes		= ullBackupCurrentDownloadedBytes;
		m_ullTotalDownloadedBytes		= ullBackupTotalDownloadedBytes;
	}
	return bRet;
}

#define MAX_HTTP_RETRY	3

bool EL_CrcPatcher::_DownloadFileFromHttp(MA_LPCWSTR wszURL, MA_LPCWSTR wszLocalPath, const std::wstring& /*fileName*/, Earth::EA_CStatus *pcStatus)
{
	std::vector<EL_BYTE> data;

	EL_INT nRet = 0;
	
	for( EL_INT i=0; i < MAX_HTTP_RETRY; i++ ) {
		if (false == _IsAlive())
			return true;
		nRet = _GetDataFromHttpW(&data, wszURL, pcStatus, wszLocalPath);
		if (nRet != 0)
			break;
		::Sleep( 200 );
	}
	if (nRet < 1)
		return false;

	WCHAR wszDirectory[MA_MAX_PATH];

	MA_PathSplit(wszLocalPath, wszDirectory, MA_ARRAYCOUNT(wszDirectory), NULL, 0);
	EA_MakeDirectory(wszDirectory);

	bool bFoundLZOnURL	= (MA_StringComparePosCI(wszURL, -3, L".lz" ) == 0);
	bool bFoundLZOnPath	= (MA_StringComparePosCI(wszLocalPath, -3, L".lz" ) == 0);

	EL_BYTE* pbyDownloadedData = NULL;
	EL_SIZE sizeDownloaded = 0;

	LPBYTE pbyOutBuffer = NULL;

	// .lz 파일인 경우 압축을 풀고 파일 이름에서 .lz를 제거한 뒤 저장
	if (bFoundLZOnURL && !bFoundLZOnPath)
	{
		DWORD realSize = 0;
		size_t headerSize = sizeof(realSize);
		memcpy(&realSize, &*data.begin(), headerSize);

		const unsigned char* srcData = reinterpret_cast<const unsigned char*>(&*data.begin()) + headerSize;
		lzo_uint srcSize = data.size() - headerSize;

		pbyOutBuffer = new BYTE[realSize];
		pbyDownloadedData = (EL_BYTE*) pbyOutBuffer;

		if (LZO_E_OK != lzo1x_decompress_safe(srcData, srcSize, (unsigned char *) pbyDownloadedData, &realSize, 0))
		{
			if( pcStatus )
				pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("ERROR_LZO_DECOMPRESS") );
			return false;
		}

		sizeDownloaded = realSize;
	} else {
		pbyDownloadedData = (EL_BYTE*) &(data[0]);
		sizeDownloaded = data.size();
	}

	HANDLE hHandle = ::CreateFile(wszLocalPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hHandle)
	{
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("FAILED_CREATE_FILE") );
		return false;
	}
	DWORD writtenBytes = 0;
	if (FALSE == ::WriteFile(hHandle, pbyDownloadedData, sizeDownloaded, &writtenBytes, NULL)) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("FAILED_WRITE_FILE"), ::GetLastError() );
		::CloseHandle(hHandle);
		return false;
	}
	::CloseHandle(hHandle);
	EL_SAFE_DELETE_ARRAY(pbyOutBuffer);
	if (writtenBytes != sizeDownloaded) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("FAILED_WRITE_FILE"), ::GetLastError() );
		return false;
	}
	return true;
}

void EL_CrcPatcher::CalculateSpeed(const MA_U64 ullDownloadedBytes, const MA_U64 ullFileSize)
{
	MA_BOOL bUpdateNow = MA_FALSE;

	if (0 == m_dwStartTime) {
		m_dwStartTime	= GetTickCount();
		m_ullStartBytes	= ullDownloadedBytes;
	}

	static DWORD s_dwLastTime = ::GetTickCount();
	const DWORD dwCurrentTime = ::GetTickCount();
	const DWORD dwDiffTime = dwCurrentTime - s_dwLastTime;

	if( dwDiffTime >= 1000 || ullDownloadedBytes == ullFileSize )
		bUpdateNow = MA_TRUE;

	if( bUpdateNow ) {
		if( 0LL < ((MA_S64) ullDownloadedBytes - (MA_S64) m_ullStartBytes) ) {
			MA_FLOAT	fDownloadSpeed = (ullDownloadedBytes - m_ullStartBytes) / ((dwCurrentTime - m_dwStartTime) / 1000.0f);

			m_pcPatchThread->SetUICurrentSpeed( fDownloadSpeed, -1.0f );
		}
		s_dwLastTime = dwCurrentTime;
	}
	UpdateProgress();
}

void EL_CrcPatcher::_OnCompletedDownload( const std::wstring& /*fileName*/, const EL_SIZE /*fileSize*/ )
{
	++m_nCurrentFileIndex;
	m_strCurrentFileName		= L"";
	m_dwStartTime				= 0;
	m_ullCurrentDownloadedBytes	= m_ullCurrentFileSize;
	UpdateProgress();
}

void EL_CrcPatcher::_OnFinished(void)
{
	m_strCurrentFileName		= L"";
	m_ullCurrentDownloadedBytes	= m_ullCurrentFileSize;
	m_ullTotalDownloadedBytes	= m_ullTotalSize;
	UpdateProgress();
}

bool EL_CrcPatcher::_IsAlive() const
{
	if (m_pcPatchThread)
		return m_pcPatchThread->_IsAlive(); 
	return true;
}

void EL_CrcPatcher::UpdateProgress()
{
	if( NULL == m_pcPatchThread )
		return;

	MA_FLOAT fTotalSizeRatio = (MA_FLOAT) (MA_DOUBLE(m_ullTotalDownloadedBytes) / MA_DOUBLE(m_ullTotalSize));
	MA_FLOAT fCurrentFileRatio = (MA_FLOAT) (MA_DOUBLE(m_ullCurrentDownloadedBytes) / MA_DOUBLE(m_ullCurrentFileSize));

	if( _isnan( fTotalSizeRatio ) )
		fTotalSizeRatio = 0.0f;
	if( _isnan( fCurrentFileRatio ) )
		fCurrentFileRatio = 0.0f;
	m_pcPatchThread->SetUICurrentFileProgress( fTotalSizeRatio, fCurrentFileRatio );
	m_pcPatchThread->SetUINameState( m_strCurrentFileName.c_str() );
	m_pcPatchThread->UpdateUI();
}
