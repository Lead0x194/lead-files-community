#include "metin2torrentconsole.h"
#include "helper.h"
#include "StringFromHttp.h"

#include <io.h>
#include <direct.h>
#include <Tlhelp32.h>

#include <boost/filesystem/operations.hpp>

#include <Mantle/MA_StringCharSet.h>
#include <Mantle/MA_Path.h>
#include <YmirBase/tstringbuffer.h>

int KILL_PROC_BY_NAME(const char *szToTerminate);

MA_BOOL EL_KillProcess(MA_LPCTSTR tszImagePath)
{
	EL_TCHAR tszImageName[MA_MAX_PATH];
	MA_PathSplit(tszImagePath, NULL, 0, tszImageName, MA_ARRAYCOUNT(tszImageName));

	Mantle::MA_CW2A szImageName(tszImageName);
	const int iError = KILL_PROC_BY_NAME((LPCSTR) szImageName);
	return (0 == iError);
}

const std::wstring EL_GetBranchPath(EL_C_WCHAR_P FilePath)
{
	EL_WCHAR szDir[MAX_PATH];

	if (MA_PathSplit(FilePath, szDir, MA_ARRAYCOUNT(szDir), NULL, 0))
		return szDir;

	return L"";
}

const std::wstring EL_GetBPSString(EL_INT nBytesPerSeconds)
{
	MA_TCHAR	tszBPS[32];

	tszBPS[0] = _T('\0');

	if( nBytesPerSeconds > (2 * 1024 * 1024) ) {
		MA_StringFormat( tszBPS, MA_ARRAYCOUNT(tszBPS), _T("%.2f MB/s"), MA_FLOAT(nBytesPerSeconds) / (1024.0f * 1024.0f) );
	} else if( nBytesPerSeconds > (2 * 1024) ) {
		MA_StringFormat( tszBPS, MA_ARRAYCOUNT(tszBPS), _T("%.2f KB/s"), MA_FLOAT(nBytesPerSeconds) / 1024.0f );
	} else {
		MA_StringFormat( tszBPS, MA_ARRAYCOUNT(tszBPS), _T("%.3f KB/s"), MA_FLOAT(nBytesPerSeconds) / 1024.0f );
	}

	return tszBPS;
}

const std::wstring EL_GetCurrentDirectory()
{
	MA_TCHAR	tszTemp[MA_MAX_PATH];

	::GetCurrentDirectory( MA_ARRAYCOUNT(tszTemp), tszTemp );
	return tszTemp;
}

const std::wstring EL_MB2W(EL_C_ACHAR_P mbString)
{
	std::wstring ret;

	EL_StringBufferW_M ConvBuf(mbString);
	ret.assign(ConvBuf.GetPtr(), ConvBuf.GetLen());

	return ret;
}

const std::wstring EL_SystemComplete(const std::wstring& Path)
{
	return boost::filesystem::system_complete(Path).string();
}

const std::string EL_ToString(const std::wstring& str)
{
	return (LPSTR) Mantle::MA_CW2A(str.c_str());
}

const std::wstring EL_ToWString(const std::string& str)
{
	return (LPWSTR) Mantle::MA_CA2W(str.c_str());
}

EL_SIZE EL_GetFileSize(EL_C_WCHAR_P Filename)
{
	HANDLE hHandle = ::CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	LARGE_INTEGER size;

	if (INVALID_HANDLE_VALUE == hHandle)
		size.QuadPart = 0;
	else
	{
		size.LowPart = ::GetFileSize(hHandle, (LPDWORD) &(size.HighPart));

		if ((size.LowPart == -1L) && ::GetLastError() != NO_ERROR)
			size.QuadPart = 0;
	}

	CloseHandle(hHandle);

	return static_cast<EL_SIZE>(size.QuadPart);
}

//KILL_PROC_BY_NAME implements...
namespace
{
typedef struct __tagPROCESSENTRY32
{
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;          // this process
    ULONG_PTR th32DefaultHeapID;
    DWORD   th32ModuleID;           // associated exe
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;    // this process's parent process
    LONG    pcPriClassBase;         // Base priority of process's threads
    DWORD   dwFlags;
    CHAR    szExeFile[MAX_PATH];    // Path
} __PROCESSENTRY32;

typedef struct __tagMODULEENTRY32
{
    DWORD   dwSize;
    DWORD   th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} __MODULEENTRY32;
}

int KILL_PROC_BY_NAME(const char *szToTerminate)
// Created: 6/23/2000  (RK)
// Last modified: 3/10/2002  (RK)
// Please report any problems or bugs to kochhar@physiology.wisc.edu
// The latest version of this routine can be found at:
//     http://www.neurophys.wisc.edu/ravi/software/killproc/
// Terminate the process "szToTerminate" if it is currently running
// This works for Win/95/98/ME and also Win/NT/2000/XP
// The process name is case-insensitive, i.e. "notepad.exe" and "NOTEPAD.EXE"
// will both work (for szToTerminate)
// Return codes are as follows:
//   0   = Process was successfully terminated
//   603 = Process was not currently running
//   604 = No permission to terminate process
//   605 = Unable to load PSAPI.DLL
//   602 = Unable to terminate process for some other reason
//   606 = Unable to identify system type
//   607 = Unsupported OS
//   632 = Invalid process name
//   700 = Unable to get procedure address from PSAPI.DLL
//   701 = Unable to get process list, EnumProcesses failed
//   702 = Unable to load KERNEL32.DLL
//   703 = Unable to get procedure address from KERNEL32.DLL
//   704 = CreateToolhelp32Snapshot failed
// Change history:
//   modified 3/8/2002  - Borland-C compatible if BORLANDC is defined as
//                        suggested by Bob Christensen
//   modified 3/10/2002 - Removed memory leaks as suggested by
//					      Jonathan Richard-Brochu (handles to Proc and Snapshot
//                        were not getting closed properly in some cases)
{
	BOOL bResult,bResultm;
	DWORD aiPID[1000],iCb=1000,iNumProc/*,iV2000=0*/;
	DWORD iCbneeded,i,iFound=0;
	char szName[MAX_PATH],szToTermUpper[MAX_PATH];
	HANDLE hProc,hSnapShot,hSnapShotm;
	OSVERSIONINFO osvi;
	HINSTANCE hInstLib = NULL;
	int iLen,iLenP,indx;
	HMODULE hMod;
	__PROCESSENTRY32 procentry;      
	__MODULEENTRY32 modentry;

	// Transfer Process name into "szToTermUpper" and
	// convert it to upper case
	iLenP=strlen(szToTerminate);
	if(iLenP<1 || iLenP>=MAX_PATH) return 632;
	for(indx=0;indx<iLenP;indx++)
		szToTermUpper[indx]=toupper(szToTerminate[indx]);
	szToTermUpper[iLenP]=0;

	// PSAPI Function Pointers.
	BOOL (WINAPI *lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
	BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *,
		DWORD, LPDWORD );
	DWORD (WINAPI *lpfGetModuleBaseName)( HANDLE, HMODULE,
		LPSTR, DWORD );

	// ToolHelp Function Pointers.
	HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD) ;
	BOOL (WINAPI *lpfProcess32First)(HANDLE,__PROCESSENTRY32*) ;
	BOOL (WINAPI *lpfProcess32Next)(HANDLE,__PROCESSENTRY32*) ;
	BOOL (WINAPI *lpfModule32First)(HANDLE,__MODULEENTRY32*) ;
	BOOL (WINAPI *lpfModule32Next)(HANDLE,__MODULEENTRY32*) ;

	// First check what version of Windows we're in
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	bResult=GetVersionEx(&osvi);
	if(!bResult)     // Unable to identify system version
		return 606;

	// At Present we only support Win/NT/2000/XP or Win/9x/ME
	if((osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) &&
		(osvi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS))
		return 607;

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		// Win/NT or 2000 or XP

		// Load library and get the procedures explicitly. We do
		// this so that we don't have to worry about modules using
		// this code failing to load under Windows 9x, because
		// it can't resolve references to the PSAPI.DLL.
		hInstLib = LoadLibraryA("PSAPI.DLL");
		if(hInstLib == NULL)
			return 605;

		// Get procedure addresses.
		lpfEnumProcesses		= (BOOL(WINAPI *)(DWORD *,DWORD,DWORD*)) GetProcAddress( hInstLib, "EnumProcesses" ) ;
		lpfEnumProcessModules	=	(BOOL(WINAPI *)(HANDLE, HMODULE *, DWORD, LPDWORD)) GetProcAddress( hInstLib, "EnumProcessModules" ) ;
		lpfGetModuleBaseName	=	(DWORD (WINAPI *)(HANDLE, HMODULE, LPSTR, DWORD )) GetProcAddress( hInstLib, "GetModuleBaseNameA" ) ;

		if(lpfEnumProcesses == NULL || lpfEnumProcessModules == NULL || lpfGetModuleBaseName == NULL) {
			FreeLibrary(hInstLib);
			return 700;
		}

		bResult=lpfEnumProcesses(aiPID,iCb,&iCbneeded);
		if(!bResult) {
			// Unable to get process list, EnumProcesses failed
			FreeLibrary(hInstLib);
			return 701;
		}

		// How many processes are there?
		iNumProc=iCbneeded/sizeof(DWORD);

		// Get and match the name of each process
		for(i=0;i<iNumProc;i++)
		{
			// Get the (module) name for this process

			MA_StringCopyA(szName, MA_ARRAYCOUNT(szName), "Unknown");
			// First, get a handle to the process
			hProc=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,
				aiPID[i]);
			// Now, get the process name
			if(hProc) {
				if(lpfEnumProcessModules(hProc,&hMod,sizeof(hMod),&iCbneeded) ) {
					iLen=lpfGetModuleBaseName(hProc,hMod,szName,MAX_PATH);
				}
			}
			CloseHandle(hProc);
			// We will match regardless of lower or upper case
			if( strcmp(_strupr(szName), szToTermUpper) == 0 ) {
				// Process found, now terminate it
				iFound=1;
				// First open for termination
				hProc=OpenProcess(PROCESS_TERMINATE,FALSE,aiPID[i]);
				if(hProc) {
					if(TerminateProcess(hProc,0)) {
						// process terminated
						CloseHandle(hProc);
						//FreeLibrary(hInstLib);
						//return 0;
					} else {
						// Unable to terminate process
						CloseHandle(hProc);
						FreeLibrary(hInstLib);
						return 602;
					}
				} else {
					// Unable to open process for termination
					FreeLibrary(hInstLib);
					return 604;
				}
			}
		}

	}

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
		// Win/95 or 98 or ME

		hInstLib = LoadLibraryA("Kernel32.DLL");
		if( hInstLib == NULL )
			return 702;

		// Get procedure addresses.
		// We are linking to these functions of Kernel32
		// explicitly, because otherwise a module using
		// this code would fail to load under Windows NT,
		// which does not have the Toolhelp32
		// functions in the Kernel 32.
		lpfCreateToolhelp32Snapshot=
			(HANDLE(WINAPI *)(DWORD,DWORD))
			GetProcAddress( hInstLib,
			"CreateToolhelp32Snapshot" ) ;
		lpfProcess32First=
			(BOOL(WINAPI *)(HANDLE,__PROCESSENTRY32*))
			GetProcAddress( hInstLib, "Process32First" ) ;
		lpfProcess32Next=
			(BOOL(WINAPI *)(HANDLE,__PROCESSENTRY32*))
			GetProcAddress( hInstLib, "Process32Next" ) ;
		lpfModule32First=
			(BOOL(WINAPI *)(HANDLE,__MODULEENTRY32*))
			GetProcAddress( hInstLib, "Module32First" ) ;
		lpfModule32Next=
			(BOOL(WINAPI *)(HANDLE,__MODULEENTRY32*))
			GetProcAddress( hInstLib, "Module32Next" ) ;
		if( lpfProcess32Next == NULL ||
			lpfProcess32First == NULL ||
			lpfModule32Next == NULL ||
			lpfModule32First == NULL ||
			lpfCreateToolhelp32Snapshot == NULL )
		{
			FreeLibrary(hInstLib);
			return 703;
		}

		// The Process32.. and Module32.. routines return names in all uppercase

		// Get a handle to a Toolhelp snapshot of all the systems processes.

		hSnapShot = lpfCreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS, 0 ) ;
		if( hSnapShot == INVALID_HANDLE_VALUE )
		{
			FreeLibrary(hInstLib);
			return 704;
		}

		// Get the first process' information.
		procentry.dwSize = sizeof(procentry);
		bResult=lpfProcess32First(hSnapShot,&procentry);

		// While there are processes, keep looping and checking.
		while(bResult)
		{
			// Get a handle to a Toolhelp snapshot of this process.
			hSnapShotm = lpfCreateToolhelp32Snapshot(
				TH32CS_SNAPMODULE, procentry.th32ProcessID) ;
			if( hSnapShotm == INVALID_HANDLE_VALUE )
			{
				CloseHandle(hSnapShot);
				FreeLibrary(hInstLib);
				return 704;
			}
			// Get the module list for this process
			modentry.dwSize=sizeof(modentry);
			bResultm=lpfModule32First(hSnapShotm,&modentry);

			// While there are modules, keep looping and checking
			while(bResultm)
			{
				if(strcmp(modentry.szModule,szToTermUpper)==0)
				{
					// Process found, now terminate it
					iFound=1;
					// First open for termination
					hProc=OpenProcess(PROCESS_TERMINATE,FALSE,procentry.th32ProcessID);
					if(hProc)
					{
						if(TerminateProcess(hProc,0))
						{
							// process terminated
							CloseHandle(hSnapShotm);
							CloseHandle(hSnapShot);
							CloseHandle(hProc);
							//FreeLibrary(hInstLib);
							//return 0;
						} else {
							// Unable to terminate process
							CloseHandle(hSnapShotm);
							CloseHandle(hSnapShot);
							CloseHandle(hProc);
							FreeLibrary(hInstLib);
							return 602;
						}
					}
					else
					{
						// Unable to open process for termination
						CloseHandle(hSnapShotm);
						CloseHandle(hSnapShot);
						FreeLibrary(hInstLib);
						return 604;
					}
				}
				else
				{  // Look for next modules for this process
					modentry.dwSize=sizeof(modentry);
					bResultm=lpfModule32Next(hSnapShotm,&modentry);
				}
			}

			//Keep looking
			CloseHandle(hSnapShotm);
			procentry.dwSize = sizeof(PROCESSENTRY32);
			bResult = lpfProcess32Next(hSnapShot,&procentry);
		}
		CloseHandle(hSnapShot);
	}
	if(iFound==0) {
		FreeLibrary(hInstLib);
		return 603;
	}
	FreeLibrary(hInstLib);
	return 0;
}

DWORD EL_FILETIME_TO_time_t( DWORD dwHigh, DWORD dwLow )
{
	ULARGE_INTEGER ull;
	ull.LowPart = dwLow;
	ull.HighPart = dwHigh;

	return (DWORD) (ull.QuadPart / 10000000ULL - 11644473600ULL);
}

bool EL_SetFileModifiedTime(const wchar_t * c_szFileName, DWORD dwHighDate, DWORD dwLowDate)
{
	HANDLE hFile = ::CreateFile( c_szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE) {
		//printf(" METIN_CRCFileDownloader::OnHttpClose(%s) - File 을 여는데 실패 했습니다.\n", c_szFileName);
		return false;
	}

	FILETIME ftCreate, ftAccess, ftModify;
	if (!::GetFileTime((HANDLE)hFile, &ftCreate, &ftAccess, &ftModify))
	{
		//printf(" METIN_CRCFileDownloader::OnHttpClose(%s) - File Date 를 읽는데 실패 했습니다.\n", c_szFileName);
		::CloseHandle(hFile);
		return false;
	}

	ftModify.dwHighDateTime = dwHighDate;
	ftModify.dwLowDateTime = dwLowDate;

	if (!::SetFileTime((HANDLE)hFile, &ftCreate, &ftAccess, &ftModify))
	{
		//printf(" METIN_CRCFileDownloader::OnHttpClose(%s) - File Date 를 쓰는데 실패 했습니다.\n", c_szFileName);
		::CloseHandle(hFile);
		return false;
	}

	::CloseHandle(hFile);

	return true;
}

void EL_String_Split(const std::string& src, const std::string& sep, std::vector<std::string>* ret)
{   
	if (src.empty())
		return;
    int base = 0;
    int end = 0;
    int sep_len = sep.length();

	while ( (end = src.find_first_of(sep, base)) != std::string::npos)
    {
        ret->push_back(src.substr(base, end - base));
        base = end + sep_len;
    }
    ret->push_back(src.substr(base, end - base));
}

MA_BOOL EL_CallProcess( MA_LPCTSTR exePath, MA_LPCTSTR exeArgs, HANDLE* OutProcessHandle, Earth::EA_CStatus* pcStatus )
{
	if( exePath == NULL || (!(*exePath)) ) {
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, _T("RUN_PATH_EMPTY") );

		wxMessageBox(wxString(MA_T("RUN_PATH_EMPTY")), EL_LTEXT("NOTICE"));
		return false;
	}

	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb			= sizeof(STARTUPINFO);
	si.dwFlags		= STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_SHOW;

	const std::wstring runPath = EL_GetBranchPath(exePath);

	MA_WCHAR cmdLine[MA_MAX_PATH];
	MA_StringFormatW(cmdLine, MA_ARRAYCOUNT(cmdLine), MA_T("\"%s\" %s"), exePath, exeArgs); 
	
	LRESULT result = CreateProcessW(
		NULL, // 커맨드라인에 실행 경로가 있을 경우 어플 경로를 넣으면 실행이 제대로 안됨
		(LPWSTR)cmdLine,
		NULL,
		NULL,
		0,
		0,
		NULL,
		runPath.empty() ? NULL : runPath.c_str(),
		&si,
		&pi);

	if(0 != result) { // 성공
		if( OutProcessHandle )
			*OutProcessHandle = pi.hProcess;
		else
			CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} else {
		DWORD error = GetLastError();
		LPVOID lpMsgBuf;
		//LPVOID lpDisplayBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL);

		EL_TCHAR buf[2048];
		MA_StringFormat(buf, MA_ARRAYCOUNT(buf), MA_T("%s(code=%d, msg='%s', runAppPath='%s')"), EL_LTEXT("ERROR_EXECUTE_PROCESS"), error, lpMsgBuf, exePath);

		LocalFree(lpMsgBuf);
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, buf );

		wxMessageBox(wxString(buf), EL_LTEXT("NOTICE"));
		

		return false;
	}

	return 0 != result;
}

