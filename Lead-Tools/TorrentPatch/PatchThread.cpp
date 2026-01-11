#include "Metin2TorrentConsole.h"
#include "PatchThread.h"
#include "TorrentPatchEvent.h"
#include "TorrentPatchPanel.h"
#include "TorrentPatchApp.h"
#include "TorrentPatchTaskBarIcon.h"
#include <Mantle/MA_Path.h>

#include <YmirBase/MappedFile.h>
#include <YmirBase/TStringBuffer.h>
#include <YmirBase/CRC32.h>

#include "Helper.h"
#include "StringFromFile.h"
#include "StringFromHttp.h"

#include "CrcPatcher.h"
#include "TorrentPatch.h"

#include <fstream>
#include <EterBase/FileName.h>
#include <EterBase/MappedFile.h>
#include <EterBase/FileLoader.h>
#include <EterBase/Singleton.h>
#include <EterPack/EterPack.h>
#include <EterPack/EterPackManager.h>

#include <Reducio/Reducio.h>

#include <Mantle/MA_Path.h>
#include <Mantle/MA_String.h>
#include <Mantle/MA_CCommandLine.h>
#include "LauncherDef.h"
extern DWORD g_cmdFlags;
#ifdef _DEBUG
	#pragma comment(lib, "reducio_d.lib")
#else
	#pragma comment(lib, "reducio.lib")
#endif

using namespace Reducio;

WCHAR* TORRENT_STATE_NAMES[TORRENT_STATE_MAX] = {
	L"TORRENT_STATE_QUEUED_FOR_CHECKING", 
	L"TORRENT_STATE_CHECKING", 
	L"TORRENT_STATE_DOWNLOADING_METADATA", 
	L"TORRENT_STATE_DOWNLOADING", 
	L"TORRENT_STATE_FINISHED", 
	L"TORRENT_STATE_SEEDING", 
	L"TORRENT_STATE_ALLOCATING", 
	L"TORRENT_STATE_CHECKING_RESUME_DATA",
};

EL_PatchThread::EL_PatchThread(EL_TorrentPatchPanel* panelPtr, EL_TorrentPatchTaskBarIcon* taskBarIconPtr)
: wxThread()
, m_panelPtr(panelPtr)
, m_taskBarIconPtr(taskBarIconPtr)
, m_pcTorrentPatcher(NULL)
, m_pcCrcPatcher(NULL)
, m_bTerminate(false)
, m_eTransferStage(ETRANSFER_STAGE_FOREGROUND)
{
}

EL_PatchThread::~EL_PatchThread()
{
}

void EL_PatchThread::Terminate()
{
	m_bTerminate = true;
}

void MakeTextureCachePack( EL_PatchThread* pPatchThread, const std::string& strPackFileName, const std::string& strTexCacheFileName, const std::string strFolderName, MA_FLOAT fBaseProgress, MA_FLOAT fStepProgress )
{
	std::ifstream inFile( strTexCacheFileName.c_str(), std::ios_base::binary | std::ios_base::in );

	int  nTexCount;
	int  nNameLength, nMipMapCount;
	char szTexFileName[1024];

	inFile.read((char*)&nTexCount, sizeof(int) );

	std::string strTexCachePackName = strPackFileName + "_texcache";

	CEterFileDict fileDict;
	CEterPack* pPack = new CEterPack;

	static BYTE s_IV[32];
	if (!pPack->Create(fileDict, strTexCachePackName.c_str(), strFolderName.c_str(), false, s_IV))
	{
		delete pPack;
		return;
	}

	for( int i = 0; i < nTexCount; ++i )
	{
		inFile.read((char*)&nNameLength, sizeof(int) );
		inFile.read((char*)&nMipMapCount, sizeof(int) );

		inFile.read(szTexFileName, nNameLength );
		szTexFileName[nNameLength] = 0;

		CMappedFile	kMappedFile;
		LPCVOID		lpStream;
		std::string   wdpFileName(szTexFileName);
		std::string   ddsFileName = CFileNameHelper::NoExtension(wdpFileName) + ".dds";

		if (!CEterPackManager::Instance().Get(kMappedFile, szTexFileName, &lpStream))
		{
			continue;
		}

		ImageHandle h = CreateImageStreamHandle();
		if( eSuccess != DecompressToDDSFromWMPStream( h, (U8*)lpStream, kMappedFile.Size(), nMipMapCount ) )
		{
			DestroyImageStreamHandle(h);	
			continue;
		}

		U8* pStream;
		int nStreamLen, nMipMap;
		GetImageStreamInfo(h, &pStream, nStreamLen, nMipMap ); 

		pPack->Put(ddsFileName.c_str(), (LPCVOID)pStream, nStreamLen, COMPRESSED_TYPE_COMPRESS );

		DestroyImageStreamHandle(h);

		pPatchThread->SetUICurrentProgress( fBaseProgress + fStepProgress * (MA_FLOAT(i+1)/MA_FLOAT(nTexCount)) , MA_FALSE );
		pPatchThread->UpdateUI();
	}

	pPack->DeleteUnreferencedData();
	pPack->EncryptIndexFile();

	delete pPack;
	inFile.close();

	//지운다
	//::DeleteFileA( strTexCacheFileName.c_str() );
}


bool DecompressTextureCache(EL_PatchThread* pPatchThread, const char * c_pszFolder)
{
	static CLZO				lzo;
	static CEterPackManager	EterPackManager;

	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	std::string stFileName(stFolder);
	stFileName += "Index";

	CMappedFile file;
	LPCVOID pvData;

	if (!file.Create(stFileName.c_str(), &pvData, 0, 0))
	{
		return true;
	}

	CMemoryTextFileLoader TextLoader;
	TextLoader.Bind(file.Size(), pvData);

	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(TRUE);

	std::string strPackFileName, strTexCacheName, strTexCachePackName;
	std::vector<std::string> vecPackFileNames, vecTexCacheNames, vecFolderNames;

	DWORD dwTotalLineCount = TextLoader.GetLineCount() - 1;
	for (DWORD i = 1; i < dwTotalLineCount; i += 2)
	{
		const std::string & c_rstFolder = TextLoader.GetLineString(i);
		const std::string & c_rstName = TextLoader.GetLineString(i + 1);

		strPackFileName = stFolder + c_rstName;

		//find texture cache info
		strTexCacheName = strPackFileName + ".rdch";
		strTexCachePackName = strPackFileName + "_texcache.epk";

		if (_access(strTexCacheName.c_str(), 0) == 0 && _access(strTexCachePackName.c_str(), 0) != 0 )
		{
			vecPackFileNames.push_back( strPackFileName );
			vecFolderNames.push_back( c_rstFolder );
			vecTexCacheNames.push_back( strTexCacheName );
		}
	}

	if( vecTexCacheNames.size() == 0 )
		return true;

	MA_FLOAT fStepProgress = 1.0f / MA_FLOAT(vecTexCacheNames.size());
	EL_WCHAR wMakeTextureCache[1024];
	EL_WCHAR wCachePackName[1024];

	for( int i = 0; i < vecTexCacheNames.size(); ++i )
	{
		CEterPackManager::Instance().RegisterPack(vecPackFileNames[i].c_str(), vecFolderNames[i].c_str());

		MA_FLOAT fBaseProgress = MA_FLOAT(i) * fStepProgress;

		mbstowcs( wCachePackName, vecPackFileNames[i].c_str(), 1024 );
		MA_StringFormat(wMakeTextureCache, MA_ARRAYCOUNT(wMakeTextureCache), L"%s:%s", EL_LTEXT("MAKING_TEXTURE_CACHE"), wCachePackName);

		pPatchThread->SetUISubState( wMakeTextureCache, TRUE );

		MakeTextureCachePack( pPatchThread, vecPackFileNames[i], vecTexCacheNames[i], vecFolderNames[i], fBaseProgress, fStepProgress );

		pPatchThread->SetUICurrentProgress( MA_FLOAT(i+1) * fStepProgress , MA_FALSE );
		pPatchThread->UpdateUI();
	}

	return true;
}

void* EL_PatchThread::Entry()
{
	Earth::EA_CStatus cStatus;

	wxGetApp().ShowFrame();

	//for test
	//if (!DecompressTextureCache(this, "pack"))
	//{
	//	wxTorrentPatchNotifyEvent cEvent( wxEVT_TORRENT_PATCH_ERROR, ID_PANEL );
	//	cEvent.SetText( cStatus.GetString( Earth::EA_STATUS_ERROR ) );
	//	wxPostEvent(m_panelPtr, cEvent);
	//}

	//bool bRet = true;
	MA_BOOL bRet = LoadPatchConfiguration( &cStatus );

	if( bRet )
		bRet = InitNetwork( &cStatus );

	if (IS_SET(g_cmdFlags, NO_DATA_PATCH))
	{
	}
	else
	{
		// 패치를 시작하기 전에 서버에서 지정해 준 지워야 할 파일들을 지움.
		if( bRet )
			std::for_each(m_listDeleteFileInfo.begin(), m_listDeleteFileInfo.end(), boost::bind(&SDeleteFileInfo::DoDelete, _1, SDeleteFileInfo::BEFORE_PATCH));

		if( bRet )
			bRet = ProcessPatchStage( ETRANSFER_STAGE_FOREGROUND, m_vecForegroundPatchInfos, &cStatus );
	}

	if( bRet ) {
#ifdef UPDATE_PATCHER_SELF
		if (!IS_SET(g_cmdFlags, NO_DATA_PATCH) && !__IsNewPatcher() && __LaunchNewPatcher())
		{
			return false; // 패치가 업데이트되면 종료해야함
		}
		else
#endif
		{
			//TODO : 여기서 처리한다
			{
				if (!DecompressTextureCache(this, "pack"))
				{
					wxTorrentPatchNotifyEvent cEvent( wxEVT_TORRENT_PATCH_ERROR, ID_PANEL );
					cEvent.SetText( cStatus.GetString( Earth::EA_STATUS_ERROR ) );
					wxPostEvent(m_panelPtr, cEvent);
				}
			}

			if (IS_SET(g_cmdFlags, AUTO_START))
			{
				Sleep(100); // 패치서버에서 Configuration 읽어오는 시간이 필요함. 대충 100ms 기다리면 충분한듯.
				m_panelPtr->ClientStart();
			}
			else
			{
				// START버튼을 통해 실행해야 한다.
				_OnReadyForGameStart(__IsNewPatcher());
			}

			_OnFinished();
		}
	}

	if( bRet )
		bRet = ProcessPatchStage( ETRANSFER_STAGE_BACKGROUND, m_vecBackgroundPatchInfos, &cStatus );

	// 이 단계에선 모든 패치가 끝난 상태임. 패치과정에서 임시 파일들이 생겼을 수도 있으므로, 패치후 지워야 할 파일들을 지움.
	if( bRet )
		std::for_each(m_listDeleteFileInfo.begin(), m_listDeleteFileInfo.end(), boost::bind(&SDeleteFileInfo::DoDelete, _1, SDeleteFileInfo::AFTER_PATCH));

	if( bRet )
		bRet = ProcessIdleStage();

	CloseNetwork();

	if( bRet == false ) {
		wxTorrentPatchNotifyEvent cEvent( wxEVT_TORRENT_PATCH_ERROR, ID_PANEL );
		if( APP_RUNNING_IS_DUPLICATED() == false )
			cEvent.SetText( cStatus.GetString( Earth::EA_STATUS_ERROR ) );
		wxPostEvent(m_panelPtr, cEvent);
	}

	Sleep(10); // 세마포어가 대기를 기다릴 0.01초의 시간은 주자.
	return NULL;
}

MA_BOOL EL_PatchThread::LoadPatchConfiguration( Earth::EA_CStatus* pcStatus )
{
	//const std::wstring cwdPath = EL_GetCurrentDirectory() + _T("/");
	TCHAR szPath[1024];
	if (0 == GetModuleFileName(NULL, szPath, sizeof(szPath)))
	{
		return false;
	}
	
	const std::wstring cwdPath = std::wstring(szPath).substr(0, std::wstring(szPath).find_last_of(_T("\\"))) + _T("/");

	EL_TorrentPatchApp& cApp = wxGetApp();

	SetUIMainState( EL_LTEXT("TORRENT_PATCH_INIT") );
	UpdateUI();

	std::wstring remoteConfigXmlPath = cApp.GetRemoteConfigPath();
	std::wstring localConfigXmlPath = cApp.GetLocalConfigPath();

	EL_PatchConfig defaultConfig;
	EL_PatchConfig remoteConfig;
	EL_PatchConfig localConfig;

	std::string localConfigXmlData;

	if (EL_StringFromFileW(localConfigXmlData, localConfigXmlPath.c_str(), pcStatus)) {
		//EL_LOG_INFO(MA_T("load_local_config: %s"), localConfigXmlPath.c_str());
		if (!__ParsePatchInfoXmlData(localConfigXmlData, &localConfig, NULL, NULL, pcStatus)) {
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("XML_PARSING_ERROR") );
			return false;
		}
	} else {
		// pcStatus->AddNR( Earth::EA_STATUS_WARNING, EL_LTEXT("LOCAL_CONFIG_OPEN_ERROR") );
		pcStatus->Clear();
	}

	SetUISubState( EL_LTEXT("CONFIG_LOADING") );
	SetUICurrentProgress( 0.5f, MA_FALSE );
	UpdateUI();

	if (!_IsAlive())
		return false;

	std::string remoteConfigXmlData;

	if (EL_StringFromHttpW(remoteConfigXmlData, remoteConfigXmlPath.c_str(), pcStatus)) {			
		//EL_LOG_INFO(MA_T("load_remote_config: %s"), remoteConfigXmlPath.c_str());
		if( !__ParsePatchInfoXmlData(remoteConfigXmlData, &remoteConfig, &m_vecForegroundPatchInfos, &m_vecBackgroundPatchInfos, pcStatus) ) {
			pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("%s: %s"), EL_LTEXT("XML_PARSING_ERROR"), remoteConfigXmlPath.c_str() );
			return false;
		}
	} else {
		pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("REMOTE_CONFIG_OPEN_ERROR") );
		return false;
	}

	//EL_LOG_INFO(MA_T("config_applying"));

	SetUISubState( EL_LTEXT("CONFIG_APPYING") );
	SetUICurrentProgress( 1.0f, MA_FALSE );
	UpdateUI();

	if(!_IsAlive()) {
		//EL_LOG_ERROR(MA_T("config_applying.cancel"));
		return false;
	}

	if( remoteConfig.eulaVer > localConfig.eulaVer ) {
		if (!_OnNoticeEULA(remoteConfig.eulaUrl, wxSize(remoteConfig.eulaWidth, remoteConfig.eulaHeight)))
			return false;

		FILE* fp;
		if (_wfopen_s(&fp, localConfigXmlPath.c_str(), L"wt") == 0)
		{
			fprintf(fp, "<torrent_config notice_ver='%d'/>", remoteConfig.eulaVer);
			fclose(fp);
		}
	}

	// 설정 선택
	m_stPatchConfig.foregroundBandwidth = (localConfig.foregroundBandwidth == defaultConfig.foregroundBandwidth) ? remoteConfig.foregroundBandwidth : localConfig.foregroundBandwidth;
	m_stPatchConfig.backgroundBandwidth = (localConfig.backgroundBandwidth == defaultConfig.backgroundBandwidth) ? remoteConfig.backgroundBandwidth : localConfig.backgroundBandwidth;
	m_stPatchConfig.minPort = (localConfig.minPort == defaultConfig.minPort) ? remoteConfig.minPort : localConfig.minPort;
	m_stPatchConfig.maxPort = (localConfig.maxPort == defaultConfig.maxPort) ? remoteConfig.maxPort : localConfig.maxPort;
	m_stPatchConfig.maxHalfOpenConnections = remoteConfig.maxHalfOpenConnections;
	m_stPatchConfig.maxUploadsPerTorrent = remoteConfig.maxUploadsPerTorrent;
	m_stPatchConfig.maxConnectionsPerTorrent = remoteConfig.maxConnectionsPerTorrent;
	m_stPatchConfig.maxUploadsPerSession = remoteConfig.maxUploadsPerSession;
	m_stPatchConfig.maxConnectionsPerSession = remoteConfig.maxConnectionsPerSession;
	m_stPatchConfig.isWebSeedEnable = remoteConfig.isWebSeedEnable;
	m_stPatchConfig.isRunPathEnable = remoteConfig.isRunPathEnable;
	m_stPatchConfig.isPostPatchEnable = remoteConfig.isPostPatchEnable;
	m_stPatchConfig.bandwidths = remoteConfig.bandwidths;
	m_stPatchConfig.saveDir = EL_SystemComplete(cwdPath + remoteConfig.saveDir);
	m_stPatchConfig.runPath = EL_SystemComplete(m_stPatchConfig.saveDir + remoteConfig.runPath);
	m_stPatchConfig.seedingDuration = remoteConfig.seedingDuration;
	m_stPatchConfig.newPatcherCRC32 = remoteConfig.newPatcherCRC32;
	m_stPatchConfig.newPatcherURL = remoteConfig.newPatcherURL;
	m_stPatchConfig.xTrapPatcherArg = remoteConfig.xTrapPatcherArg;

	MA_TCHAR newPatcherPath[MA_MAX_PATH];
	MA_StringCopy(newPatcherPath, MA_ARRAYCOUNT(newPatcherPath), EL_SystemComplete(cwdPath + remoteConfig.newPatcherPath).c_str());
	MA_PathStandardizeWindows(newPatcherPath);
	m_stPatchConfig.newPatcherPath = newPatcherPath;

	MA_TCHAR curPatcherPath[MA_MAX_PATH];
	MA_StringCopy(curPatcherPath, MA_ARRAYCOUNT(curPatcherPath), EL_SystemComplete(cwdPath + remoteConfig.curPatcherPath).c_str());
	MA_PathStandardizeWindows(curPatcherPath);
	m_stPatchConfig.curPatcherPath = curPatcherPath;

	_OnConfig(m_stPatchConfig);

	if( cApp.IsGivenSwitchS() )
		::DeleteFile( m_stPatchConfig.newPatcherPath.c_str() );

	bool bSpawnMode = __IsNewPatcher();

#ifdef UPDATE_PATCHER_SELF
	if (IS_SET(g_cmdFlags, NO_DATA_PATCH))
	{
		bSpawnMode = false;
	}
	else
	{
		if (__DownloadNewPatcherFromWeb( __IsNewPatcher() ) == true)
			if (__LaunchNewPatcher() == true)
				bSpawnMode = true;
	}
#else
	bSpawnMode = false;
#endif

	// NewPatcher로 실행되었다면 바로 게임실행단계로 넘어간다.
	if (bSpawnMode)
	{
		if (__IsNewPatcher())
		{		
			//EL_LOG_INFO(MA_T("mode: newpatcher"));

			// NewPatcher임으로 oldPatcher에 replace가 되어야 한다.
			if (__BackToMainPatcher())
				_OnSpawned();
			else
				_OnError(MA_T("NEW_PATCH_UNKNOWN_ERROR"));
		} else {
			_OnSpawned();
		}
	}
	return true;
}

MA_BOOL EL_PatchThread::InitNetwork( Earth::EA_CStatus* pcStatus )
{
	MA_BOOL bRet = MA_TRUE;

	BOOST_FOREACH(const PatchInfo& stPatchInfo, m_vecForegroundPatchInfos)
	{
		if( _IsAlive() == false )
			break;
		if( stPatchInfo.IsCrcPatchInfo() ) {
			if( m_pcCrcPatcher == NULL )
				m_pcCrcPatcher = new EL_CrcPatcher( this );
		}
#ifndef NO_LIBTORRENT
		if( stPatchInfo.IsTorrentInfo() ) {
			if( m_pcTorrentPatcher == NULL ) {
				m_pcTorrentPatcher = new EL_TorrentPatcher( this );
				bRet = m_pcTorrentPatcher->Begin( m_stPatchConfig, pcStatus );
			}
		}
#endif
	}
	if( bRet == MA_FALSE )
		return bRet;
	BOOST_FOREACH(const PatchInfo& stPatchInfo, m_vecBackgroundPatchInfos)
	{
		if( _IsAlive() == false )
			break;
		if( stPatchInfo.IsCrcPatchInfo() ) {
			if( m_pcCrcPatcher == NULL )
				m_pcCrcPatcher = new EL_CrcPatcher( this );
		}
#ifndef NO_LIBTORRENT
		if( stPatchInfo.IsTorrentInfo() ) {
			if( m_pcTorrentPatcher == NULL ) {
				m_pcTorrentPatcher = new EL_TorrentPatcher( this );
				bRet = m_pcTorrentPatcher->Begin( m_stPatchConfig, pcStatus );
			}
		}
#endif
	}
	return bRet;
}

void EL_PatchThread::CloseNetwork()
{
	MA_SAFE_DELETE(m_pcCrcPatcher);
#ifndef NO_LIBTORRENT
	if(m_pcTorrentPatcher) {
		m_pcTorrentPatcher->End();
		MA_SAFE_DELETE(m_pcTorrentPatcher);
	}
#endif
}

MA_BOOL EL_PatchThread::ProcessPatchStage( ETransferStage eStage, PatchInfos& vecPatchInfo, Earth::EA_CStatus* pcStatus )
{
	MA_BOOL				bRet = MA_TRUE;
	MA_INT				iPatchCount = PatchInfos_GetPatchCount( vecPatchInfo );

	m_eTransferStage	= eStage;
	if( iPatchCount > 0 ) {
		m_stPatchStatus.Clear();
		m_stPatchStatus.m_iPatchCount	= iPatchCount;
	}

	BOOST_FOREACH(const PatchInfo& stPatchInfo, vecPatchInfo)
	{
		if( _IsAlive() == false )
			break;
		if( stPatchInfo.IsCrcPatchInfo() ) {
			if( m_pcCrcPatcher->Run( m_stPatchConfig, stPatchInfo, pcStatus ) ) {
				SetUICurrentProgress( 1.0f );
				SetUINextSubState();
				UpdateUI();
			} else {
				bRet = MA_FALSE;
				break;
			}
		}
#ifndef NO_LIBTORRENT
		if( stPatchInfo.IsTorrentInfo() ) {
			if( m_pcTorrentPatcher->Patch( m_stPatchConfig, vecPatchInfo, pcStatus ) == MA_FALSE ) {
				bRet = MA_FALSE;
				break;
			}
		}
#endif
	}

	if( bRet ) {
		SetUITotalProgress( 1.0f );
		SetUICurrentProgress( 1.0f, MA_FALSE );
		UpdateUI();
	}

	return bRet;
}

MA_BOOL EL_PatchThread::ProcessIdleStage( void )
{
	m_eTransferStage = ETRANSFER_STAGE_IDLE;

#ifndef NO_LIBTORRENT
	if( m_pcTorrentPatcher ) {
		if( m_pcTorrentPatcher->Seeding( m_stPatchConfig ) == MA_FALSE )
			return MA_FALSE;
	}

	SetUIMainState( EL_LTEXT("TORRENT_STATE_FINISHED") );
	UpdateUI();
#endif

	// 씨딩이 끝나고 난 후 재시작 처리
	while (_IsAlive()) {
		if( _PollRunEvent() )
			return MA_FALSE;
		Sleep(50);
	}
	return MA_TRUE;
}

void EL_PatchThread::OnExit()
{
	EL_TorrentPatchApp& app = wxGetApp();
	app.PostThreadDone(this);
}

bool EL_PatchThread::_IsAlive()
{
	if (m_bTerminate)
		return false;
	return IsAlive();
}

MA_BOOL EL_PatchThread::_PollRunEvent()
{
	if( APP_RUNNING_IS_DUPLICATED() ) {
		_PopRunEvent();
		return MA_TRUE;
	}
	return MA_FALSE;
}

void EL_PatchThread::_PopRunEvent()
{
	APP_RUNNING_RESET_DUPLICATED();
}

void EL_PatchThread::_OnConfig(const EL_PatchConfig& config)
{
	wxTorrentPatchConfigEvent evt(wxEVT_TORRENT_PATCH_CONFIG);
	evt.SetConfig(config);
	wxPostEvent(m_taskBarIconPtr, evt);
	wxPostEvent(m_panelPtr, evt);
}

void EL_PatchThread::_OnSpawned()
{
	wxTorrentPatchNotifyEvent evt(wxEVT_TORRENT_PATCH_SPAWNED, ID_PANEL);
	wxPostEvent(m_panelPtr, evt);
}

void EL_PatchThread::_OnFinished()
{
	wxTorrentPatchNotifyEvent evt(wxEVT_TORRENT_PATCH_FINISHED, ID_PANEL);
	wxPostEvent(m_panelPtr, evt);
}

void EL_PatchThread::_OnReadyForGameStart(bool bWithTerminate)
{
	wxTorrentPatchNotifyEvent evt(wxEVT_TORRENT_PATCH_READYFORGAMESTART, ID_PANEL);

	if (bWithTerminate)
		evt.SetText(EL_LTEXT("terminate"));

	wxPostEvent(m_panelPtr, evt);
}

void EL_PatchThread::_OnError(const std::wstring& text)
{
	//EL_LOG_ERROR(MA_T("ERROR_BOX: %s"), text.c_str());

	wxTorrentPatchNotifyEvent evt(wxEVT_TORRENT_PATCH_ERROR, ID_PANEL);
	evt.SetText(text);
	wxPostEvent(m_panelPtr, evt);
}

bool EL_PatchThread::_OnNoticeEULA(const std::wstring& url, const wxSize& size)
{
	wxTorrentPatchNotifyEvent evt(wxEVT_TORRENT_PATCH_NOTICE, ID_PANEL);
	evt.SetText(url);
	evt.SetSize(size);
	wxPostEvent(m_panelPtr, evt);

	return m_panelPtr->WaitForNoticeAnswer();
}

void EL_PatchThread::_Notify(const std::wstring& text)
{
	//EL_LOG_ERROR(MA_T("NOTIFY_BOX: %s"), text.c_str());
	wxMessageBox(wxString(text.c_str()), EL_LTEXT("NOTICE"));
}

bool EL_PatchThread::__IsNewPatcher()
{
	EL_TCHAR runPatcherPath[EL_MAX_PATH];
	::GetModuleFileName(NULL, runPatcherPath, MA_ARRAYCOUNT(runPatcherPath));

	return MA_StringCompareCI(runPatcherPath, m_stPatchConfig.newPatcherPath.c_str()) == 0;
}

bool EL_PatchThread::__BackToMainPatcher()
{
	if (m_stPatchConfig.newPatcherCRC32 == 0)
		return EL_FALSE;
	if (m_stPatchConfig.newPatcherPath.empty())
	{
		_Notify(MA_T("NO_NEW_PATCHER_PATH"));
		return EL_FALSE;
	}

	EL_UINT selfCRC32;

	if (!sGetSelfCRC32(&selfCRC32))
	{
		_Notify(MA_T("CUR_PATCHER_SELF_OPEN_ERROR"));
		return EL_FALSE;
	}

	// 이전 패처의 종료를 시도한다 (혹시 남아 있을 수도 있음)
	EL_TCHAR szCurPatcherImageName[EL_MAX_PATH];
	MA_PathSplit(m_stPatchConfig.curPatcherPath.c_str(), NULL, 0, szCurPatcherImageName, MA_ARRAYCOUNT(szCurPatcherImageName));
	EL_KillProcess(szCurPatcherImageName);

	// 자신의 CRC32와 새로운 패처의 CRC32값 정보가 다르고, 사용자가 강제로 덮어쓰기 허가를 하지 않을 경우 종료.
	if (selfCRC32 =! m_stPatchConfig.newPatcherCRC32 && wxMessageBox(EL_LTEXT("NEW_PATCHER_MANUAL_CHANGING"), EL_LTEXT("NEW_PATCHER_RUNNING"), wxYES_NO|wxICON_INFORMATION, NULL) == wxYES)
	{
		_Notify(MA_T("NEW_PATCHER_CHANGE_ERROR"));
		return EL_FALSE;
	}

	const EL_UINT uiNumTry = 5;
	EL_UINT i = 0;

	while (i != uiNumTry)
	{
		if (CopyFileW(m_stPatchConfig.newPatcherPath.c_str(), m_stPatchConfig.curPatcherPath.c_str(), EL_FALSE))
		{
			//EL_LOG_INFO(MA_T("copy_ok: %s -> %s"), m_stPatchConfig.newPatcherPath.c_str(), m_stPatchConfig.curPatcherPath.c_str());							
			break;
		}
		else
		{
			//EL_LOG_ERROR(MA_T("COPY_ERROR: %s -> %s"), m_stPatchConfig.newPatcherPath.c_str(), m_stPatchConfig.curPatcherPath.c_str());
			Sleep(1000);	// 기다린 후 재시도
		}

		++i;
	}

	if (i == uiNumTry) {
		_Notify(MA_T("NEW_PATCHER_CHANGE_ERROR"));
		return false;
	}

	MA_TCHAR tszArguments[MA_MAX_PATH] = _T("");

#if defined(USE_FAST_SELFUPDATE)
	EL_TorrentPatchApp& cApp = wxGetApp();
	MA_LPCTSTR c_tszConfigPrefix = cApp.GetConfigPrefix();

	MA_StringFormat( tszArguments, MA_ARRAYCOUNT(tszArguments), _T("-s") );
	if( c_tszConfigPrefix && *c_tszConfigPrefix )
		MA_StringAppendf( tszArguments, MA_ARRAYCOUNT(tszArguments), _T(" -p \"%s\""), c_tszConfigPrefix );
#endif
	APP_RUNNING_SHUTDOWN();

	if( EL_CallProcess( m_stPatchConfig.curPatcherPath.c_str(), tszArguments ) == false ) {
		_Notify( MA_T("NEW_PATCHER_RUN_ERROR") );
		return false;
	}

	_OnSpawned();
	return true;
}

bool EL_PatchThread::__LaunchNewPatcher()
{
	if( m_stPatchConfig.newPatcherCRC32 == 0 )
		return EL_FALSE;
	if( m_stPatchConfig.newPatcherPath.empty() ) {
		_Notify(MA_T("NO_NEW_PATCHER_PATH"));
		return EL_FALSE;
	}

	EL_UINT selfCRC32;

	if( !sGetSelfCRC32(&selfCRC32) ) {
		_Notify(MA_T("CUR_PATCHER_SELF_OPEN_ERROR"));
		return EL_FALSE;
	}

	// 실행중인 패처가 새로운 패처의 CRC32 와 같다면 패처 업데이트 안함
	if( selfCRC32 == m_stPatchConfig.newPatcherCRC32 ) {
		//EL_LOG_INFO(MA_T("runpatcher_newpatcher_is_same"));
		return EL_FALSE;
	}

	EL_UINT newPatcherCRC32;

	if( !sGetCRC32(m_stPatchConfig.newPatcherPath.c_str(), &newPatcherCRC32) ) {
		// 파일이 없으면 그냥 종료해도 됨
		// _Notify(MA_T("NEW_PATCHER_SELF_OPEN_ERROR"));
		return EL_FALSE;
	}

	//EL_LOG_INFO(MA_T("local_crc32 : 0x%.8x: %s"), newPatcherCRC32, m_stPatchConfig.newPatcherPath.c_str());
	//EL_LOG_INFO(MA_T("remote_crc32: 0x%.8x"), m_stPatchConfig.newPatcherCRC32);		

	if( newPatcherCRC32 != m_stPatchConfig.newPatcherCRC32 ) {
		//_Notify(MA_T("NEW_PATCHER_FILE_CRC32_ERROR"));
		return EL_FALSE;
	}

	_Notify(EL_LTEXT("NEW_PATCHER_START"));

	MA_TCHAR tszArguments[MA_MAX_PATH] = _T("");

#if defined(USE_FAST_SELFUPDATE)
	EL_TorrentPatchApp& cApp = wxGetApp();
	MA_LPCTSTR c_tszConfigPrefix = cApp.GetConfigPrefix();

	MA_StringFormat( tszArguments, MA_ARRAYCOUNT(tszArguments), _T("-u \"%s\""), m_stPatchConfig.curPatcherPath.c_str() );
	if( c_tszConfigPrefix && *c_tszConfigPrefix )
		MA_StringAppendf( tszArguments, MA_ARRAYCOUNT(tszArguments), _T(" -p \"%s\""), c_tszConfigPrefix );
#endif
	APP_RUNNING_SHUTDOWN();

	if( EL_CallProcess( m_stPatchConfig.newPatcherPath.c_str(), tszArguments ) == false ) {
		_Notify( MA_T("NEW_PATCHER_RUN_ERROR") );
		return false;
	}
	_OnSpawned();
	return EL_TRUE;
}

bool EL_PatchThread::__DownloadNewPatcherFromWeb(bool bToMainPatcher)
{
	EL_CrcPatcher cCRCPatcher(this);

	MA_BOOL bIsNewPatcher = bToMainPatcher;

	if (m_stPatchConfig.newPatcherURL.empty())
		return EL_FALSE;
	if (m_stPatchConfig.newPatcherPath.empty())
		return false;

	//MA_LPCTSTR	ptszSelfModulePath = bToMainPatcher ? config.newPatcherPath.c_str() : config.curPatcherPath.c_str();
	MA_LPCTSTR	ptszNewModulePath  = bToMainPatcher ? m_stPatchConfig.curPatcherPath.c_str() : m_stPatchConfig.newPatcherPath.c_str();
	MA_TCHAR	tszNewModuleFileName[MA_MAX_PATH];

	MA_PathSplit(ptszNewModulePath , NULL, 0, tszNewModuleFileName, MA_ARRAYCOUNT(tszNewModuleFileName));
	if (EL_KillProcess(tszNewModuleFileName))
		::Sleep(1000);

	EL_UINT dwSelfCRC32 = 0;
	EL_UINT dwNewPatcherCRC32 = 0;

	if (!sGetSelfCRC32(&dwSelfCRC32))
		return EL_FALSE;

	// 실행중인 패처가 새로운 패처의 CRC32 와 같다면 패처 업데이트 안함
	if (bIsNewPatcher == MA_FALSE) {
		if (dwSelfCRC32 == m_stPatchConfig.newPatcherCRC32)
			return EL_FALSE;
	}

	if (sGetCRC32(ptszNewModulePath, &dwNewPatcherCRC32)) {
		// 이미 완벽하게 받았다면, 업데이트 시작함
		if (dwNewPatcherCRC32 == m_stPatchConfig.newPatcherCRC32)
			return EL_TRUE;
	}

	return cCRCPatcher._DownloadFileFromHttp(m_stPatchConfig.newPatcherURL.c_str(), ptszNewModulePath, tszNewModuleFileName, NULL );
}

bool EL_PatchThread::sGetCRC32(MA_LPCTSTR fileName, EL_UINT* puCRC32)
{
	EL_MappedFile mappedFile;
	if (!mappedFile.Open(fileName, EL_File::FILEMODE_READ))
		return EL_FALSE;
	if (puCRC32)
		*puCRC32 = EL_CRC32_Make(mappedFile.GetDataPtr(), mappedFile.GetSize());
	mappedFile.Close();	
	return EL_TRUE;
}

bool EL_PatchThread::sGetSelfCRC32(EL_UINT* puCRC32)
{
	EL_TCHAR tszSelfModulePath[EL_MAX_PATH];

	::GetModuleFileName (::GetModuleHandle(NULL), tszSelfModulePath, MA_ARRAYCOUNT(tszSelfModulePath));	
	return sGetCRC32(tszSelfModulePath, puCRC32);
}

//void EL_PatchThread::__SetForegroundTransferState(libtorrent::session& ses, EL_PatchConfig& config)
//{
//	m_eTransferStage = ETRANSFER_STAGE_FOREGROUND;
//	config.currentBandwidth = config.foregroundBandwidth * 1024;
//	_OnConfig(config);
//	__SetBandwidth(ses, config.currentBandwidth);
//}
//
//void EL_PatchThread::__SetBackgroundTransferState(libtorrent::session& ses, EL_PatchConfig& config)
//{
//	m_eTransferStage = ETRANSFER_STAGE_BACKGROUND;	
//	config.currentBandwidth = config.backgroundBandwidth * 1024;
//	_OnConfig(config);
//	__SetBandwidth(ses, config.currentBandwidth);
//}

bool EL_PatchThread::__ParsePatchElement(TiXmlElement* pkFirstElement, const std::string& xmlElementName, EL_PatchConfig* /*retConfig*/, PatchInfos* retPatchInfos, Earth::EA_CStatus* /*pcStatus*/)
{
	TiXmlHandle firstHandle(pkFirstElement);

	{
		TiXmlElement* pkTorrentNode = firstHandle.FirstChildElement(xmlElementName.c_str()).Element();
		for (; pkTorrentNode != NULL; pkTorrentNode = pkTorrentNode->NextSiblingElement(xmlElementName.c_str()))
		{				
			EL_C_ECHAR_P torrentUrl = (EL_C_ECHAR_P) pkTorrentNode->Attribute("torrent_url");
			if (torrentUrl)
			{
				PatchInfo patchInfo;
				
				Mantle::MA_CE2W wtorrentUrl(torrentUrl);
				patchInfo.torrentUrl = wtorrentUrl;

				// 설정이 있을 경우 최소 10
				EL_C_ACHAR_P randomStart = pkTorrentNode->Attribute("random_start");
				if (randomStart)
					patchInfo.randomStart = EL_MAX(10, atoi(randomStart));
				else
					patchInfo.randomStart = 0;

				// 웹시드
				EL_C_ACHAR_P webseedUrl = pkTorrentNode->Attribute("webseed_url");
				if (webseedUrl)
					patchInfo.webseedUrls.push_back(webseedUrl);
				EL_C_ACHAR_P triggerTime = pkTorrentNode->Attribute("webseed_trigger_time");
				if (triggerTime)
					patchInfo.triggerTime = atoi(triggerTime);
				EL_C_ACHAR_P triggerBandwidth = pkTorrentNode->Attribute("webseed_trigger_bandwidth");
				if (triggerBandwidth)
					patchInfo.triggerBandwidth = atoi(triggerBandwidth);

				// 웹 시드 노드
				TiXmlHandle torrentHandle(pkTorrentNode);
				TiXmlElement* pkWebseedNode = torrentHandle.FirstChildElement("webseed").Element();
				for (; pkWebseedNode != NULL; pkWebseedNode = pkWebseedNode->NextSiblingElement("webseed"))
				{
					EL_C_ACHAR_P url = pkWebseedNode->Attribute("url");
					if (url)
					{
						patchInfo.webseedUrls.push_back(url);
					}
				}

				retPatchInfos->push_back(patchInfo);
			}

			EL_C_ECHAR_P crcpatchUrl = (EL_C_ECHAR_P) pkTorrentNode->Attribute("crcpatch_url");
			if (crcpatchUrl)
			{
				PatchInfo patchInfo;
				
				Mantle::MA_CE2W wcrcpatchUrl(crcpatchUrl);
				patchInfo.crcpatchUrl = wcrcpatchUrl;

				// CRC version
				EL_C_ACHAR_P crcpatchVersion = pkTorrentNode->Attribute("crcversion");
				if (crcpatchVersion) {
					Mantle::MA_CE2W wcrcpatchVersion((MA_LPESTR)crcpatchVersion);
					patchInfo.crcpatchVersion = wcrcpatchVersion;
				}

				retPatchInfos->push_back(patchInfo);
			}
		}
	}
	return true;
}

bool EL_PatchThread::__ParseDeleteFileInfoElement(TiXmlElement* pkFirstElement)
{
	TiXmlHandle firstHandle(pkFirstElement);

	{
		TiXmlElement* pkNode = firstHandle.FirstChildElement("delete_info").Element();
		for (; pkNode != NULL; pkNode = pkNode->NextSiblingElement("delete_info"))
		{
			// "after_patch" attribute로 파일 삭제 시기 결정. 만약 after_patch 속성이 있으면 모든 패치과정이 끝나고 지우고, 없으면 패치파일 받기 전에 지움 (디폴트)
			SDeleteFileInfo::EDeleteTime delTime = pkNode->Attribute("after_patch") ? SDeleteFileInfo::AFTER_PATCH : SDeleteFileInfo::BEFORE_PATCH;
			const char* filePath = pkNode->Attribute("path");

			if (filePath)
				m_listDeleteFileInfo.push_back(SDeleteFileInfo(filePath, delTime));
		}
	}

	return true;
}


bool EL_PatchThread::__ParsePatchInfoXmlData(const std::string& xmlData, EL_PatchConfig* retConfig, PatchInfos* retFgPatchInfos, PatchInfos* retBgPatchInfos, Earth::EA_CStatus *pcStatus)
{
	TiXmlDocument CXMLDocument;

	CXMLDocument.Parse(xmlData.c_str());
	if( CXMLDocument.Error() ) {
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("XML_PARSING_ERROR") );
		return false;
	}

	TiXmlHandle xmlHandle(&CXMLDocument);
	TiXmlElement* pkFirstElement = xmlHandle.FirstChildElement().Element();
	if (!pkFirstElement)
	{
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("XML_FIRST_ELEMENT_ERROR") );
		return false;
	}

	// 포트 범위 얻기
	EL_C_ACHAR_P portRange = pkFirstElement->Attribute("port_range");
	if (portRange)
	{
		std::vector<std::string> ports;
		EL_String_Split(portRange, "~", &ports);
		if (!ports.empty())
		{
			const EL_SIZE portCount = ports.size();		
			if (portCount == 1)
			{
				retConfig->minPort = atoi(ports[0].c_str());
				retConfig->maxPort = retConfig->minPort;
			}
			else if (portCount >= 2)
			{
				retConfig->minPort = atoi(ports[0].c_str());
				retConfig->maxPort = atoi(ports[1].c_str());
			}		
		}
	}

	// 개수 제한
	EL_C_ACHAR_P maxHalfOpenConnections = pkFirstElement->Attribute("max_half_open_connections");
	if (maxHalfOpenConnections)
		retConfig->maxHalfOpenConnections = atoi(maxHalfOpenConnections);
	
	EL_C_ACHAR_P maxUploadsPerSession = pkFirstElement->Attribute("max_uploads_per_session");
	if (maxUploadsPerSession)
		retConfig->maxUploadsPerSession = atoi(maxUploadsPerSession);

	EL_C_ACHAR_P maxConnectionsPerSession = pkFirstElement->Attribute("max_connections_per_session");
	if (maxConnectionsPerSession)
		retConfig->maxConnectionsPerSession = atoi(maxConnectionsPerSession);

	EL_C_ACHAR_P maxUploadsPerTorrent = pkFirstElement->Attribute("max_uploads_per_torrent");
	if (maxUploadsPerTorrent)
		retConfig->maxUploadsPerTorrent = atoi(maxUploadsPerTorrent);

	EL_C_ACHAR_P maxConnectionsPerTorrent = pkFirstElement->Attribute("max_connections_per_torrent");
	if (maxConnectionsPerTorrent)
		retConfig->maxConnectionsPerTorrent = atoi(maxConnectionsPerTorrent);


	// 대역폭 리스트 얻기
	EL_C_ACHAR_P bandwidthRange = pkFirstElement->Attribute("bandwidth_range");

	if (bandwidthRange)
	{
		std::vector<std::string> bandwidths;
		EL_String_Split(bandwidthRange, ":", &bandwidths);
		if (!bandwidths.empty())
		{
			retConfig->bandwidths.clear();
			BOOST_FOREACH(std::string bandwidth, bandwidths)
			{
				retConfig->bandwidths.push_back(atoi(bandwidth.c_str()));
			}
		}
	} else {
		retConfig->bandwidths.reserve(5);
		retConfig->bandwidths.push_back(10);
		retConfig->bandwidths.push_back(30);
		retConfig->bandwidths.push_back(50);
		retConfig->bandwidths.push_back(80);
		retConfig->bandwidths.push_back(100);
	}

	// 속도 얻기
	EL_C_ACHAR_P foregroundSpeed = pkFirstElement->Attribute("foreground_speed");
	if (foregroundSpeed)
		retConfig->foregroundBandwidth = atoi(foregroundSpeed);

	EL_C_ACHAR_P backgroundSpeed = pkFirstElement->Attribute("background_speed");
	if (backgroundSpeed)
		retConfig->backgroundBandwidth = atoi(backgroundSpeed);

	// SEEDING 기간
	EL_C_ACHAR_P seedingDuration = pkFirstElement->Attribute("seeding_duration");
	if (seedingDuration)
		retConfig->seedingDuration = atoi(seedingDuration);


	// WEB_SEED 가능
	EL_C_ACHAR_P webSeedEnable = pkFirstElement->Attribute("web_seed_enable");
	if (webSeedEnable)
		retConfig->isWebSeedEnable = (atoi(webSeedEnable) != 0);

	// RUN_PATH 가능
	EL_C_ACHAR_P runPathEnable = pkFirstElement->Attribute("run_path_enable");
	if (runPathEnable)
		retConfig->isRunPathEnable = (atoi(runPathEnable) != 0);

	// POST_CRC_PATCH 가능
	EL_C_ACHAR_P postPatchEnable = pkFirstElement->Attribute("post_patch_enable");
	if (postPatchEnable)
		retConfig->isPostPatchEnable = (atoi(postPatchEnable) != 0);

	// 패처 CRC32
	EL_C_ACHAR_P newPatcherCRC32 = pkFirstElement->Attribute("new_patcher_crc32");
	if (newPatcherCRC32)
		retConfig->newPatcherCRC32 = MA_StringToUInt32A(newPatcherCRC32, MA_StringLengthA(newPatcherCRC32));

	// 패처 경로
	EL_ECHAR_P newPatcherPath = (EL_ECHAR_P) pkFirstElement->Attribute("new_patcher_path");
	if (newPatcherPath)
	{
		Mantle::MA_CE2W wnewPatcherPath(newPatcherPath);
		retConfig->newPatcherPath = wnewPatcherPath;
	}

	// 패처 경로
	EL_ECHAR_P curPatcherPath = (EL_ECHAR_P) pkFirstElement->Attribute("cur_patcher_path");
	if (curPatcherPath)
	{
		Mantle::MA_CE2W wcurPatcherPath(curPatcherPath);
		retConfig->curPatcherPath = wcurPatcherPath;
	}

	// 패처 URL
	EL_ECHAR_P newPatcherURL = (EL_ECHAR_P) pkFirstElement->Attribute("new_patcher_url");
	if (newPatcherURL)
	{
		Mantle::MA_CE2W wnewPatcherURL(newPatcherURL);
		retConfig->newPatcherURL = wnewPatcherURL;
	}

	// 저장 경로
	EL_ECHAR_P saveDir = (EL_ECHAR_P) pkFirstElement->Attribute("save_dir");
	if (saveDir)
	{
		Mantle::MA_CE2W wsaveDir(saveDir);
		retConfig->saveDir = wsaveDir;
	}

	// 실행 
	EL_ECHAR_P runPath = (EL_ECHAR_P) pkFirstElement->Attribute("run_path");
	if (runPath)
	{
		Mantle::MA_CE2W wrunPath(runPath);
		retConfig->runPath = wrunPath;
	}

	// 공지
	EL_C_ACHAR_P eulaVer = pkFirstElement->Attribute("notice_ver");
	if (eulaVer)
		retConfig->eulaVer = atoi(eulaVer);

	EL_C_ACHAR_P eulaWidth = pkFirstElement->Attribute("notice_width");
	if (eulaWidth)
		retConfig->eulaWidth = atoi(eulaWidth);

	EL_C_ACHAR_P eulaHeight = pkFirstElement->Attribute("notice_height");
	if (eulaHeight)
		retConfig->eulaHeight = atoi(eulaHeight);

	EL_C_ACHAR_P eulaUrl = pkFirstElement->Attribute("notice_url");
	if (eulaUrl)
	{
		EL_StringBufferW_M eulaUrl_UCS2(eulaUrl);
		retConfig->eulaUrl.assign(eulaUrl_UCS2.GetPtr(), eulaUrl_UCS2.GetLen());
	}

	// 설정 URL 
	EL_ECHAR_P configUrl = (EL_ECHAR_P) pkFirstElement->Attribute("config_url");
	if (configUrl)
	{
		Mantle::MA_CE2W wconfigUrl(configUrl);
		retConfig->remoteConfigXmlPath = wconfigUrl;
	}

	EL_C_ACHAR_P xtrapArg = pkFirstElement->Attribute("xtrap_arg");
	if (xtrapArg)
	{
		EL_StringBufferA_M XTrapArgBuffer(xtrapArg);
		retConfig->xTrapPatcherArg.assign(XTrapArgBuffer.GetPtr(), XTrapArgBuffer.GetLen());
	}

	if( retFgPatchInfos )
		__ParsePatchElement(pkFirstElement, "foreground_patch", retConfig, retFgPatchInfos, pcStatus);
	if( retBgPatchInfos )
		__ParsePatchElement(pkFirstElement, "background_patch", retConfig, retBgPatchInfos, pcStatus);

	// Patcher가 지워야 할 파일의 목록을 parsing
	__ParseDeleteFileInfoElement(pkFirstElement);

	return true;
}

MA_BOOL	EL_PatchThread::UpdateSpawning( void )
{
	if( _PollRunEvent() )
		return false;
	return true;
}

/* UI Updates */
void EL_PatchThread::UpdateUI()
{
	wxTorrentPatchProgressEvent cEvent( wxEVT_TORRENT_PATCH_PROGRESS, ID_PANEL );
	cEvent.SetStatus( m_stPatchStatus );
	wxPostEvent( m_panelPtr, cEvent );
}

void EL_PatchThread::SetUIMainState( MA_LPCWSTR wszText, MA_BOOL bDisplayMainRatioText )
{
	m_stPatchStatus.mainState				= wszText;
	m_stPatchStatus.m_bDisplayMainRatioText	= bDisplayMainRatioText;
}

void EL_PatchThread::SetUISubState( MA_LPCWSTR wszText, MA_BOOL bDisplaySubRatioText )
{
	m_stPatchStatus.subState				= wszText;
	m_stPatchStatus.m_bDisplaySubRatioText	= bDisplaySubRatioText;
}

void EL_PatchThread::SetUINameState( MA_LPCWSTR wszText )
{
	m_stPatchStatus.m_strNameState = wszText;
}

void EL_PatchThread::SetUITotalProgress( MA_FLOAT fRatio )
{
	m_stPatchStatus.mainRatio = fRatio;
}

void EL_PatchThread::SetUICurrentProgress( MA_FLOAT fRatio, MA_BOOL bAffectMainRatio )
{
	m_stPatchStatus.subRatio = fRatio;
	if( bAffectMainRatio && m_stPatchStatus.m_iPatchCount > 0 ) {
		MA_FLOAT fStep = 1.0f / MA_FLOAT(m_stPatchStatus.m_iPatchCount);
		m_stPatchStatus.mainRatio	= MA_FLOAT(m_stPatchStatus.m_iPatchIndex) / MA_FLOAT(m_stPatchStatus.m_iPatchCount) + (fRatio * fStep);
	}
}

void EL_PatchThread::SetUICurrentFileProgress( MA_FLOAT fTotalSizeRatio, MA_FLOAT fCurrentFileRatio )
{
	MA_FLOAT fStep = 1.0f / MA_FLOAT(m_stPatchStatus.m_iPatchCount);

	m_stPatchStatus.subRatio = fCurrentFileRatio;
	if( m_stPatchStatus.m_iPatchCount > 0 )
		m_stPatchStatus.mainRatio	= MA_FLOAT(m_stPatchStatus.m_iPatchIndex) / MA_FLOAT(m_stPatchStatus.m_iPatchCount) + (fStep * fTotalSizeRatio);
}

void EL_PatchThread::SetUICurrentSpeed( MA_FLOAT fDownloadBPS, MA_FLOAT fUploadBPS )
{
	m_stPatchStatus.dnBytesPerSec = fDownloadBPS;
	m_stPatchStatus.upBytesPerSec = fUploadBPS;
}

void EL_PatchThread::SetUICurrentPeers( MA_INT nActiveSeeds, MA_INT nSeeds, MA_INT nActivePeers, MA_INT nPeers )
{
	m_stPatchStatus.peersCount		= nActivePeers;
	m_stPatchStatus.seedsCount		= nActiveSeeds;
	m_stPatchStatus.totalPeersCount	= nPeers;
	m_stPatchStatus.totalSeedsCount	= nSeeds;
}

void EL_PatchThread::SetUINextSubState( void )
{
	if (m_stPatchStatus.m_iPatchIndex < m_stPatchStatus.m_iPatchCount)
		m_stPatchStatus.m_iPatchIndex++;
}


void SDeleteFileInfo::DoDelete(SDeleteFileInfo::EDeleteTime delTime)
{
	if (this->deleteTime != delTime)
		return;

	Mantle::MA_CA2W path(this->filePath.c_str());

	// NOTE: DeleteFile(..) 안쓰고 이렇게 지우는 이유는 폴더도 지우기 위함임. (물론 폴더 안에는 파일이 없어야 함)
    HANDLE hFile = CreateFile(path, 0, FILE_SHARE_READ|FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_DELETE_ON_CLOSE, NULL);

	if (hFile)
		CloseHandle(hFile);
}
