#pragma once

#include <wx-2.8.10/app.h>

WX_DEFINE_ARRAY_PTR(wxThread *, wxThreads);

#include <YmirBase/TLockFreeQueueSPSC.h>
#include <YmirBase/Environment.h>

class EL_TorrentPatchApp : public wxApp
{
public:
	EL_TorrentPatchApp();
	virtual ~EL_TorrentPatchApp();

	virtual bool OnInit();
	virtual int OnExit();

    virtual void OnInitCmdLine( wxCmdLineParser& cParser );
    virtual bool OnCmdLineParsed( wxCmdLineParser& cParser );

	void	IconizeFrame();
	void	ShowFrame( bool isShow = true );

	void	RegisterThread( wxThread* newThread );
	void	PostThreadDone( wxThread* delThread );	
	
	void	WaitUntilAllThreadsDone();
	void	RequestTerminate();

	MA_BOOL	IsGivenSwitchS( void ) const	{ return m_bDoNotSelfUpdate; }

	bool	LoadURLSetting( const std::wstring& strPrefix, Earth::EA_CStatus* pcStatus );

	MA_LPCWSTR GetLocalConfigPath()			{ return m_localConfigXmlPath.c_str(); }
	MA_LPCWSTR GetRemoteConfigPath()		{ return m_remoteConfigXmlPath.c_str(); }
	MA_LPCWSTR GetOpenidAuthkey()			{ return m_openidAuthkey.c_str(); }

public:
	TLockFreeQueueSPSC<EL_UINT> m_changeBandwidthQueue;

public:
	MA_LPCTSTR			GetConfigPrefix();

private:
	void __KillAllThreads();

	std::wstring		m_strPrefix;
	std::wstring		m_localConfigXmlPath;
	std::wstring		m_remoteConfigXmlPath;
	std::wstring		m_languageXmlPath;
	MA_BOOL				m_bDoNotSelfUpdate;
	std::wstring		m_openidAuthkey;

private:
	wxThreads			m_threads;
	wxCriticalSection	m_cs;
	wxSemaphore			m_semAllThreadDone;

	// Singleton (YmirBase)
	EL_Environment		m_cEnvironment;
};

DECLARE_APP(EL_TorrentPatchApp);

extern MA_BOOL APP_RUNNING_INIT();
extern void APP_RUNNING_SHUTDOWN();
extern MA_BOOL APP_RUNNING_IS_DUPLICATED();
extern void APP_RUNNING_RESET_DUPLICATED();
