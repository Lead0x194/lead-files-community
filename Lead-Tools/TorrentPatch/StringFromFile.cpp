#include "Metin2TorrentConsole.h"
#include "StringFromFile.h"
#include "CMiniUnZIP.h"

bool EL_StringFromDiskFileW(std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus)
{	
	static EL_C_WCHAR_P MODE = L"rb";
	FILE* fp;
	errno_t error = _wfopen_s(&fp, filePathz, MODE);
	if( error ) {
		if( pcStatus )
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("_WFOPEN_S_ERROR"), filePathz, MODE);
		return false;
	}

	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	self.resize(fileSize);
	fread(&self[0], fileSize, 1, fp);
	fclose(fp);
	return true;
}

// zip 파일에서 읽기
bool EL_StringFromZipFileW(std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus)
{
	BOOL		bLoaded = FALSE;

#ifdef _SFX_ENABLE
	CMiniUnZIP	cZIP;

	TCHAR		tszSelfFileName[MAX_PATH];
	::GetModuleFileName(NULL, tszSelfFileName, MAX_PATH);

	if (cZIP.OpenArchive(tszSelfFileName))
	{
		DWORD dwFileIndex = cZIP.GetFileIndex(filePathz);

		if (dwFileIndex != CMiniUnZIP::FILE_NOT_FOUND)
		{
			DWORD dwFileSize = cZIP.GetFileLength(dwFileIndex);
			self.resize(dwFileSize);
			cZIP.ReadFile(dwFileIndex, (&self[0]));
			bLoaded	= TRUE;
		} else {
			if( pcStatus )
				pcStatus->AddNRf( Earth::EA_STATUS_ERROR, EL_LTEXT("LOAD_FILE_FROM_SFX_ZIP_FAILED"), tszSelfFileName );
		}
		cZIP.CloseArchive();
	}
#endif 
	return bLoaded != FALSE;
}

// zip 파일에서 먼저 읽고 파일에서 읽기
bool EL_StringFromFileW(std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus)
{
#ifdef _SFX_ENABLE
	if( EL_StringFromZipFileW(self, filePathz, pcStatus) )
		return true;
#endif 
	return EL_StringFromDiskFileW(self, filePathz, pcStatus);
}
