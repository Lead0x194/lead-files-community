#include "Metin2TorrentConsole.h"
#include "StringFromHttp.h"

#include <Wininet.h>

#pragma comment(lib, "wininet.lib")

bool EL_StringFromHttpW(std::string& self, const std::wstring& fileUrl, Earth::EA_CStatus* pcStatus)
{	
	static const EL_WCHAR_P PROTOCOL_SEP = L"://";
	static const EL_SIZE    PROTOCOL_SEP_LEN = wcslen(PROTOCOL_SEP);

	const EL_INT protocolSepPos = fileUrl.find(PROTOCOL_SEP, 0, PROTOCOL_SEP_LEN);

	const EL_INT protocolLen = (protocolSepPos >= 0) ? protocolSepPos + PROTOCOL_SEP_LEN : 0;
	
	const std::wstring protocol = fileUrl.substr(0, protocolLen);	
	if (protocolLen > 0)
	{
		static const EL_WCHAR_P HTTP_PREFIX = L"http://";
		if (_wcsnicmp(protocol.c_str(), HTTP_PREFIX, protocolLen) != 0)
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("CHECK_PROTOCOL_ERROR"), protocol.c_str(), HTTP_PREFIX);
			return false;
		}
	}
	
	std::wstring fileUrlAddr;
	std::wstring fileUrlPath;

	const EL_INT firstSepPos = fileUrl.find('/', protocolLen);
	if (firstSepPos > 0)
	{
		fileUrlAddr.assign(fileUrl.substr(protocolLen, firstSepPos-protocolLen));
		fileUrlPath.assign(fileUrl.substr(firstSepPos));
	}
	else
	{
		fileUrlAddr.assign(fileUrl.substr(protocolLen));
	}

	HINTERNET hSession = InternetOpenW(TAPP_FULLNAME,
               INTERNET_OPEN_TYPE_DIRECT, 
               NULL,NULL,NULL);

	if (!hSession)
	{	
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_OPEN_ERROR"), GetLastError() );
		return false;
	}
	
	HINTERNET hConnection = InternetConnect(hSession,
		fileUrlAddr.c_str(),
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0);

	if (!hConnection)
	{
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_CONNECT_ERROR"), fileUrlAddr.c_str(), GetLastError() );
		return false;
	}

	HINTERNET hRequest = HttpOpenRequestW(hConnection,
		L"GET",
		fileUrlPath.c_str(),
		HTTP_VERSION,
		NULL,
		NULL,
		INTERNET_FLAG_RELOAD,
		0);

	if (!hRequest)
	{
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_OPEN_REQUEST_ERROR"), fileUrlPath.c_str(), GetLastError() );
		return false;
	}

	EL_SIZE resumeSize = 0;
	{
		EL_WCHAR extraHeader[256];	
		memset(extraHeader, 0, sizeof(extraHeader));

		EL_INT extraHeaderLen = 0;
		if (!HttpSendRequestW(hRequest, extraHeader, extraHeaderLen, NULL, 0))
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_SEND_REQUEST_ERROR"), fileUrlPath.c_str(), GetLastError() );
			return false;
		}
	}
	
	EL_SIZE recvSize = 0;
	{
		{
			EL_WCHAR query[32];
			memset(query, 0, sizeof(query));

			DWORD dwQuerySize = sizeof(query);
			DWORD dwResultSize = 0;
			if (!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, query, &dwQuerySize, &dwResultSize))
			{
				if( pcStatus )
					pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_QUERY_STATUS_CODE_ERROR"), fileUrlPath.c_str(), GetLastError() );
				return false;
			}

			EL_INT status = _wtol(query);
			if (HTTP_STATUS_OK != status)
			{
				if( pcStatus )
					pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("HTTP_QUERY_STATUS_NOT_OK"), fileUrlPath.c_str(), GetLastError(), status );
				return false;
			}
		}
		
		{
			EL_WCHAR query[32];
			memset(query, 0, sizeof(query));

			DWORD dwQuerySize = sizeof(query);
			DWORD dwResultSize = 0;

			if (!HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, query, &dwQuerySize, &dwResultSize)) {
				if( pcStatus )
					pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("%s (path='%s', error=%d)"), EL_LTEXT("HTTP_QUERY_CONTENT_LENGTH_ERROR"), fileUrlPath.c_str(), GetLastError() );
				return false;
			}

			recvSize = (EL_SIZE)_wtoi(query);
		}		
	}

	// 메모리 확보
	self.reserve(recvSize);
	self.clear();
	
	EL_SIZE recvPos = 0;
	DWORD dwBytesAvailable;
	while (InternetQueryDataAvailable(hRequest, &dwBytesAvailable, 0, 0))
	{
		EL_ACHAR buf[64*1024];
		DWORD dwBufferSize = EL_MIN((DWORD) sizeof(buf), dwBytesAvailable);

		DWORD dwReadSize;
		if (!InternetReadFile(hRequest, buf, dwBufferSize, &dwReadSize))
		{
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("INTERNET_READ_FILE_ERROR"), fileUrlPath.c_str(), GetLastError(), recvPos, recvSize );
			return false;
		}

		if (dwReadSize == 0)
			break;

		self.append(buf, dwReadSize);

		recvPos += dwReadSize;
			
		if (recvPos >= recvSize + resumeSize)
			break;
		
	}

	if (NULL!=hRequest)
		InternetCloseHandle(hRequest);

	if (NULL!=hConnection)
		InternetCloseHandle(hConnection);

	if (NULL!=hSession)
		InternetCloseHandle(hSession);

	return true;
}
