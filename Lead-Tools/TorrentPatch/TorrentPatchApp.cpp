#include "Metin2TorrentConsole.h"
#include "TorrentPatchApp.h"
#include "TorrentPatchFrame.h"
#include "PatchThread.h"
#include "StringFromFile.h"

#include "Metin2TorrentConsoleLink.h"

#include "helper.h"

#include "Version.h"
#include "TorrentSkin.h"

#include <wx-2.8.10/cmdline.h>

#include <Earth/EA_CUAC.h>

#include <Mantle/MA_Path.h>
#include <Mantle/MA_String.h>
#include <Mantle/MA_CCommandLine.h>

#include "LauncherDef.h"

DWORD g_cmdFlags = 0;

#pragma data_seg(".SHARED")
MA_BOOL	APP_RUN_DUPLICATED = MA_FALSE;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.SHARED,RWS")
#ifdef _DEBUG
#pragma comment( lib, "cryptlib-5.6.1MTd.lib" )
#else
#pragma comment( lib, "cryptlib-5.6.1MT.lib" )
#endif

// guidgen.exe 로 생성
MA_LPCTSTR TORRENT_PATCH_APP_GUID = _T("D1A41C06-CECA-45b0-A3C0-347271EED100");

HANDLE APP_MUTEX = NULL;

MA_BOOL APP_RUNNING_INIT()
{
	APP_MUTEX = ::CreateMutex(NULL, TRUE, TORRENT_PATCH_APP_GUID);

	if( NULL == APP_MUTEX )
		return MA_FALSE;

	if(::GetLastError() == ERROR_ALREADY_EXISTS )
	{
		::CloseHandle(APP_MUTEX);
		APP_MUTEX = NULL;
		return MA_FALSE;
	}

	return MA_TRUE;
}

void APP_RUNNING_SHUTDOWN()
{
	if( NULL == APP_MUTEX )
		return;

	::ReleaseMutex(APP_MUTEX);		
	::CloseHandle(APP_MUTEX);

	HANDLE hExistingMutex;

	while(TRUE)
	{
		hExistingMutex = OpenMutex(0, FALSE, TORRENT_PATCH_APP_GUID); 
	    if(NULL == hExistingMutex)
			break;

		::Sleep(1000);
	}
}

MA_BOOL APP_RUNNING_IS_DUPLICATED()
{
	return APP_RUN_DUPLICATED;
}

void APP_RUNNING_RESET_DUPLICATED()
{
	APP_RUN_DUPLICATED = false;
}

///////////////////////////////////////////////////////////////////////

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_OPTION, wxT("p"), wxT("prefix"),	wxT("set url prefix"),	wxCMD_LINE_VAL_STRING,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, wxT("u"), wxT("update"),	wxT("no comment"),		wxCMD_LINE_VAL_STRING,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("s"), wxT("spawned"),	wxT("no comment"),		wxCMD_LINE_VAL_NONE,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("v"), wxT("version"),	wxT("get app version"),	wxCMD_LINE_VAL_NONE,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("n"), wxT("no-data-patch"),	wxT("do not patch data but xtrap or hackshield"),			wxCMD_LINE_VAL_NONE,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("a"), wxT("auto-start"),		wxT("start with no button click"),							wxCMD_LINE_VAL_NONE,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("c"), wxT("client-pause"),		wxT("passing argument(--pause-before-create-window) to client"),	wxCMD_LINE_VAL_NONE,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, wxT("param"), wxT("param"),	wxT("passing argument(--openid-authkey) to client"),	wxCMD_LINE_VAL_STRING,	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, wxT("ULL"), wxT("NULL"), wxT("ignore string"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};

IMPLEMENT_APP(EL_TorrentPatchApp);

EL_TorrentPatchApp::EL_TorrentPatchApp()
: m_bDoNotSelfUpdate(MA_FALSE)
{
}

EL_TorrentPatchApp::~EL_TorrentPatchApp()
{
	APP_RUNNING_SHUTDOWN();
}

void EL_TorrentPatchApp::OnInitCmdLine( wxCmdLineParser& cParser )
{
	cParser.SetDesc(g_cmdLineDesc);
	cParser.SetSwitchChars(wxT("N-"));
}

bool EL_TorrentPatchApp::OnCmdLineParsed( wxCmdLineParser& cParser )
{
	cParser.SetSwitchChars(wxT("-"));
	wxString urlPrefix;
	wxString strNewFileName;
	wxString openidAuthkey;

	if( cParser.Found( wxT("p"), &urlPrefix ) )
		m_strPrefix = urlPrefix;

	if( cParser.Found( wxT("s") ) )
		m_bDoNotSelfUpdate = MA_TRUE;

	if( cParser.Found( wxT("v") ) ) {
		MA_TCHAR versionPath[MA_MAX_PATH];
		MA_PathMerge(versionPath, MA_ARRAYCOUNT(versionPath), EL_GetCurrentDirectory().c_str(), MA_T("temp\\version.txt"));

		FILE* fp = NULL;
		if (_tfopen_s(&fp, versionPath, MA_T("wt")) == 0) {
			fprintf(fp, "r%d\n", APP_GET_VERSION());
			fclose(fp);
		}
		return false;
	}

	if( cParser.Found( wxT("u"), &strNewFileName) ) {
		TCHAR tszSelfFileName[MA_MAX_PATH];
		MA_BOOL bCopied = MA_FALSE;

		::GetModuleFileName( NULL, tszSelfFileName, MA_ARRAYCOUNT(tszSelfFileName) );

		EL_KillProcess( strNewFileName.c_str() );

		for( MA_INT i=0; i < 100; i++ ) {
			if( CopyFile( tszSelfFileName, strNewFileName.c_str(), MA_FALSE ) ) {
				bCopied = MA_TRUE;
				break;
			}
			Sleep( 500 );
		}

		if( bCopied ) {
			MA_TCHAR tszArguments[MA_MAX_PATH] = _T("");

#if defined(USE_FAST_SELFUPDATE)
			MA_StringFormat( tszArguments, MA_ARRAYCOUNT(tszArguments), _T("-s") );
			if( m_strPrefix.size() )
				MA_StringAppendf( tszArguments, MA_ARRAYCOUNT(tszArguments), _T(" -p \"%s\""), m_strPrefix.c_str() );
#endif
			APP_RUNNING_SHUTDOWN();

			EL_CallProcess( strNewFileName.c_str(), tszArguments );
		}
		return false;
	}
	
	if( cParser.Found( wxT("n") ) ) 
	{
		SET_BIT(g_cmdFlags, NO_DATA_PATCH);
	}

	if( cParser.Found( wxT("a") ) ) 
	{
		SET_BIT(g_cmdFlags, AUTO_START);
	}

	if( cParser.Found( wxT("c") ) ) 
	{
		SET_BIT(g_cmdFlags, CLIENT_PAUSE);
	}

	if( cParser.Found( wxT("param"), &openidAuthkey ) )
		m_openidAuthkey = openidAuthkey;
	
	return true;
}

bool EL_TorrentPatchApp::OnInit()
{
	TCHAR szPath[1024];
	if (0 == GetModuleFileName(NULL, szPath, sizeof(szPath)))
	{
		return false;
	}

	const std::wstring cwdPath = std::wstring(szPath).substr(0, std::wstring(szPath).find_last_of(_T("\\")));

	::SetCurrentDirectoryW(cwdPath.c_str());

	Mantle::MA_SetLocale( NULL );

	if( !wxApp::OnInit() )
		return false;

	if (APP_RUNNING_INIT() == EL_FALSE)
		return false;

	if (_taccess(DIR_LOG, 02) != 0)
		_tmkdir(DIR_LOG);

	// 로그 파일 열기
	{
		MA_TCHAR modPath[MAX_PATH];
		MA_TCHAR modName[MAX_PATH];
		MA_TCHAR modHead[MAX_PATH];
		MA_TCHAR logPrefix[MA_MAX_PATH];

		::GetModuleFileName( NULL, modPath, MA_ARRAYCOUNT(modPath) );
		MA_PathSplit( modPath, NULL, 0, modName, MA_ARRAYCOUNT(modName) );
		MA_PathSplitFileName( modName, modHead, MA_ARRAYCOUNT(modHead), NULL, 0 );
		MA_StringFormat( logPrefix, MA_ARRAYCOUNT(logPrefix), _T("%s\\%s"), DIR_LOG, modHead );

		EL_Log_OpenFile( logPrefix );
	}

	Earth::EA_CStatus cStatus;

	if( LoadURLSetting( m_strPrefix, &cStatus ) == false ) {
		wxMessageBox( cStatus.GetString(Earth::EA_STATUS_ERROR), TAPP_NAME, wxOK | wxICON_INFORMATION );
		CLocaleManager::sDestroyActive();
		return false;
	}

	cStatus.Clear();
	if( CLocaleManager::sGetActive()->Load( m_languageXmlPath.c_str(), &cStatus ) == false ) {
		wxMessageBox( cStatus.GetString(Earth::EA_STATUS_ERROR), TAPP_NAME, wxOK | wxICON_INFORMATION );
		CLocaleManager::sDestroyActive();
		return false;
	}

	CLocaleManager::sGetActive()->Add( _T("TEXT_PATCHER_VER"), APP_VERSION_NAME );

#ifndef _DEBUG
	Earth::EA_CUAC	cUAC;

	// 관리자 권한이 있는지 검사
	if( cUAC.HasAdminRights() == MA_FALSE ) {
		Mantle::MA_CCommandLine cCommandLine( argc, argv );

		// 관리자 권한이 없다면, UAC가 지원되는 시스템인지 검사
		if( cUAC.IsUACSupported() == MA_FALSE) {
			wxMessageBox(EL_LTEXT("NOT_SUPPORT_UAC"), EL_LTEXT("TORRENT_PATCH"), wxOK | wxICON_ERROR);
			CLocaleManager::sDestroyActive();
			return false;
		}

		// 관리자권한으로 실행
		if( cUAC.RunElevated( cCommandLine.GetProgramName(), cCommandLine.GetCommandLine() ) == MA_FALSE ) {
			wxMessageBox(EL_LTEXT("LAUNCH_ERROR_WITH_ADMIN_PRIVILEGES"), EL_LTEXT("TORRENT_PATCH"), wxOK | wxICON_ERROR);
			CLocaleManager::sDestroyActive();
			return false;
		}

		// 일반권한 프로그램 종료
		return false;
	}
#endif

	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxICOHandler);

	cStatus.Clear();
	if( SkinInitialize( EL_LTEXT("LauncherSkin"), &cStatus ) == false ) {
		wxMessageBox( cStatus.GetString(Earth::EA_STATUS_ERROR), TAPP_NAME, wxOK | wxICON_INFORMATION );
		CLocaleManager::sDestroyActive();
		return false;
	}

	EL_TorrentPatchFrame* pcNewFrame = new EL_TorrentPatchFrame( EL_LTEXT("TORRENT_PATCH") );
	pcNewFrame->StartPatchThread();
	pcNewFrame->Show(true);

	SetTopWindow(pcNewFrame);

	return true;
}

int EL_TorrentPatchApp::OnExit()
{
	CLocaleManager::sDestroyActive();
	return 0;
}

void EL_TorrentPatchApp::RegisterThread(wxThread* newThread)
{
	wxCriticalSectionLocker locker(m_cs);
	m_threads.Add(newThread);
}

void EL_TorrentPatchApp::__KillAllThreads()
{
	m_cs.Enter();

	while (!m_threads.IsEmpty())
	{
		wxThread* thread = m_threads.Last();

		m_cs.Leave();

		// wxThread::Kill을 부르는 것은 안전하지 못한 방법이나
		// libtorrent의 응답이 없으므로 인해 종료가 안되는 문제를
		// 해결하기 위해서 어쩔 수 없이 이렇게 처리함
		thread->Kill();

		m_cs.Enter();
	}

	m_cs.Leave();
}


void EL_TorrentPatchApp::RequestTerminate()
{
	m_cs.Enter();
	for (int i = 0; i < (int) m_threads.GetCount(); ++i) {
		wxThread* thread = m_threads[i];
		m_cs.Leave();
		if (EL_PatchThread * t = dynamic_cast<EL_PatchThread*>(thread))
			t->Terminate();
		m_cs.Enter();
	}
	m_cs.Leave();
}

void EL_TorrentPatchApp::WaitUntilAllThreadsDone()
{
	m_semAllThreadDone.WaitTimeout(10000); // 최대 10초 대기
	__KillAllThreads();
}

void EL_TorrentPatchApp::PostThreadDone(wxThread* delThread)
{
	wxCriticalSectionLocker locker(m_cs);

	m_threads.Remove(delThread);

	// 모든 쓰레드가 전부 종료되었다면
	if (m_threads.IsEmpty())
		m_semAllThreadDone.Post();
}

void EL_TorrentPatchApp::IconizeFrame()
{
	wxFrame* pcTopFrame = dynamic_cast<wxFrame*>( GetTopWindow() );

	if( pcTopFrame != NULL )
		pcTopFrame->Iconize();
}

void EL_TorrentPatchApp::ShowFrame(bool isShow)
{
	wxFrame* pcTopFrame = dynamic_cast<wxFrame*>( GetTopWindow() );

	if( pcTopFrame != NULL  ) {
		pcTopFrame->Show(isShow);
		if( isShow && pcTopFrame->IsIconized() )
			pcTopFrame->Maximize( false );
		if( isShow )
			::SetForegroundWindow( HWND(pcTopFrame->GetHandle()) );
	}
}

MA_LPCTSTR EL_TorrentPatchApp::GetConfigPrefix()
{
	return m_strPrefix.c_str();
}

bool EL_TorrentPatchApp::LoadURLSetting(const std::wstring& strPrefix, Earth::EA_CStatus* pcStatus)
{
	std::wstring strLocaleFileName;

	if(strPrefix.empty() == false)
		strLocaleFileName = strPrefix;
	strLocaleFileName += FILENAME_PATCH_URL_XML;

	pcStatus->AddNRf( Earth::EA_STATUS_INFO, _T("Loading URL setting file (path='%s')"), strLocaleFileName.c_str() );

	std::string strXMLContents;
	if (false == EL_StringFromDiskFileW(strXMLContents, strLocaleFileName.c_str(), pcStatus)) {
		if (false == EL_StringFromZipFileW(strXMLContents, strLocaleFileName.c_str(), pcStatus)) {
			pcStatus->Clear();
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, _T("URL setting file not found") );
			return false;
		}
	}

	TiXmlDocument CXMLDocument;

	CXMLDocument.Parse( strXMLContents.c_str() );

	if( CXMLDocument.Error() ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Error while parsing XML (%s)"), strLocaleFileName.c_str() );
		return false;
	}

	TiXmlHandle CXMLHandle( &CXMLDocument );
	TiXmlElement* pkFirstElement = CXMLHandle.FirstChildElement().Element();
	if( NULL == pkFirstElement ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("No first element in XML (%s)"), strLocaleFileName.c_str() );
		return false;
	}

	MA_LPCSTR szValue = pkFirstElement->Attribute("localConfigPath");
	if( szValue == NULL ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Cannot find localConfigPath setting! (%s)"), strLocaleFileName.c_str() );
		return false;
	}
	m_localConfigXmlPath = EL_ToWString(szValue);

	szValue = pkFirstElement->Attribute("remoteConfigPath");
	if( szValue == NULL ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Cannot find remoteConfigPath setting! (%s)"), strLocaleFileName.c_str() );
		return false;
	}
	m_remoteConfigXmlPath = EL_ToWString(szValue);

	szValue = pkFirstElement->Attribute("languagePath");
	if( szValue == NULL ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Cannot find languagePath setting! (%s)"), strLocaleFileName.c_str() );
		return false;
	}
	m_languageXmlPath = EL_ToWString(szValue);

	return true;
}
