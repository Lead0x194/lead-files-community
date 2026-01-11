#pragma once

#define EL_SAFE_INTERNETCLOSEHANDLE(x)		if( (x) ) { InternetCloseHandle(x); x = NULL; }

MA_BOOL EL_KillProcess(MA_LPCTSTR tszImagePath);

DWORD EL_GetFileCRC32(EL_C_WCHAR_P Filename);
EL_SIZE EL_GetFileSize(EL_C_WCHAR_P Filename);


const std::wstring EL_GetBPSString(EL_INT nBytesPerSeconds);
const std::wstring EL_GetCurrentDirectory();
const std::wstring EL_MB2W(EL_C_ACHAR_P mbString);
const std::wstring EL_GetBranchPath(EL_C_WCHAR_P FilePath);
const std::wstring EL_SystemComplete(const std::wstring& Path);

const std::wstring EL_ToWString(const std::string& String);
const std::string EL_ToString(const std::wstring& str);

DWORD EL_FILETIME_TO_time_t( DWORD dwHigh, DWORD dwLow );
bool EL_SetFileModifiedTime(const wchar_t * c_szFileName, DWORD dwHighDate, DWORD dwLowDate);
void EL_String_Split(const std::string& src, const std::string& sep, std::vector<std::string>* ret);

MA_BOOL EL_CallProcess( MA_LPCTSTR exePath, MA_LPCTSTR exeArgs, HANDLE* outProcessHandle = NULL, Earth::EA_CStatus* pcStatus = NULL);
