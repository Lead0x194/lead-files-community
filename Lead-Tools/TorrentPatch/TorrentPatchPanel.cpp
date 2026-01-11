#include "Metin2TorrentConsole.h"
#include "TorrentPatchPanel.h"
#include "TorrentPatchEvent.h"
#include "TorrentPatchApp.h"

#include "TorrentSkin.h"
#include "SkinGauge.h"
#include "SkinButton.h"
#include "SkinText.h"
#include "WebBrowserPanel.h"

#include "shellapi.h"
#include "helper.h"
#include "CPostIt.h"

#include <YmirBase/DiskUtils.h>
#include <wx-2.8.10/dialog.h>

// for XTrap
// NOTE: 원래는 이렇게 static lib linking할때는 build configuration을 별도로 빼서 해야하나 우리의 목적은
// 보안 Solution을 XTrap으로 일원화 시키는데 있으므로 기존 build configuration에 xtrap을 static link시키기로 한다.

#include <urlmon.h>
#pragma comment(lib, "urlmon")
#pragma comment(lib, "wininet")

#include <Xtrap_L_Interface.h>
#include "LauncherDef.h"
extern DWORD g_cmdFlags;

#pragma comment( lib, "XTrap4Launcher_mt.lib" )

BEGIN_EVENT_TABLE(EL_TorrentPatchPanel, wxPanel)
	EVT_TORRRNT_PATCH_CONFIG(EL_TorrentPatchPanel::__OnChangeConfig)
	EVT_BUTTON(ID_PANEL_CLOSE,  EL_TorrentPatchPanel::__OnClickCloseButton)
	EVT_BUTTON(ID_PANEL_EXIT,  EL_TorrentPatchPanel::__OnClickExitButton)
	EVT_BUTTON(ID_PANEL_START,	EL_TorrentPatchPanel::__OnClickStartButton)
	EVT_TORRENT_PATCH_PROGRESS(ID_PANEL,  EL_TorrentPatchPanel::__OnTorrentPatchProgress)
	EVT_TORRENT_PATCH_SPAWNED (ID_PANEL,  EL_TorrentPatchPanel::__OnTorrentPatchSpawned)
	EVT_TORRENT_PATCH_FINISHED(ID_PANEL,  EL_TorrentPatchPanel::__OnTorrentPatchFinished)
	EVT_TORRENT_PATCH_ERROR(ID_PANEL,	  EL_TorrentPatchPanel::__OnTorrentPatchError)
	EVT_TORRENT_PATCH_NOTICE(ID_PANEL,	  EL_TorrentPatchPanel::__OnTorrentPatchNotice)
	EVT_TORRENT_PATCH_READYFORGAMESTART(ID_PANEL,	EL_TorrentPatchPanel::__OnTorrentPatchReadyForGameStart)
	EVT_ERASE_BACKGROUND(EL_TorrentPatchPanel::__OnErase)
	EVT_PAINT(EL_TorrentPatchPanel::__OnPaint)
	EVT_COMMAND_RANGE(ID_BUTTON_START, ID_BUTTON_END, wxEVT_COMMAND_BUTTON_CLICKED, EL_TorrentPatchPanel::__OnButtonClicked)
	EVT_LEFT_DOWN(EL_TorrentPatchPanel::__OnLeftDown)
	EVT_IDLE(EL_TorrentPatchPanel::__OnIdle)
END_EVENT_TABLE()

void EL_TorrentPatchPanel::__OnLeftDown(wxMouseEvent& evt)
{
	if (g_pcSkin)
		PostMessage((HWND)this->GetParent()->GetHandle(), WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(evt.GetX(), evt.GetY()));
}

EL_TorrentPatchPanel::EL_TorrentPatchPanel(wxWindow *parent)
: wxPanel(parent, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN)
, m_subTextPtr(NULL)
, m_mainTextPtr(NULL)
, m_subGaugePtr(NULL)
, m_mainGaugePtr(NULL)
, m_upSpeedTextPtr(NULL)
, m_dnSpeedTextPtr(NULL)
, m_peersCountTextPtr(NULL)
, m_seedsCountTextPtr(NULL)
, m_funcButtonPtr(NULL)
, m_torrentTextPtr(NULL)
, m_startButtonPtr(NULL)
, m_bRunWithExit(false)
, m_runProcessHandle(NULL)
, m_isNoticeAnswer(false)
{
	EL_SkinText& subText  = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_PATCH"));
	EL_SkinGauge& subGauge = *new EL_SkinGauge(this, -1, PROGRESS_MAX, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);

	EL_SkinText& mainText  = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_TOTAL_PROGRESS"));
	EL_SkinGauge& mainGauge = *new EL_SkinGauge(this, -1, PROGRESS_MAX, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);

	//EL_SkinText& torrentText = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_DOWNLOAD_NAME"));
	EL_SkinText& torrentText = *new EL_SkinText(this, -1, EL_LTEXT(""));
	EL_SkinText& dnSpeedText = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_DOWNLOAD_SPEED"));
	EL_SkinText& upSpeedText = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_UPLOAD_SPEED"));
	EL_SkinText& peersCountText = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_PEERS_COUNT"));
	EL_SkinText& seedsCountText = *new EL_SkinText(this, -1, EL_LTEXT("TORRENT_SEEDS_COUNT"));
	EL_SkinText& versionText = *new EL_SkinText(this, -1, APP_VERSION_NAME);

	EL_SkinButton& funcButton = *new EL_SkinButton(this, ID_PANEL_CLOSE, EL_LTEXT("TORRENT_PATCH_CANCEL"));
	EL_SkinButton& startButton = *new EL_SkinButton(this, ID_PANEL_START, EL_LTEXT("TORRENT_PATCH_START"));
	startButton.Enable(false);

	if (g_pcSkin)
	{
		for (EL_SIZE index = 0; index < g_pcSkin->GetControls(); ++index)
		{
			const EL_SkinControlInfo info = g_pcSkin->GetControlInfo(index);
			wxWindow* control = NULL;

			const int id = ID_BUTTON_START + index;
			//const EL_ControlSkin& controlSkin = g_pcSkin->GetControlSkin(index);

			switch (info.type)
			{
			case EL_ESkinControl_Text:
				{
					EL_SkinText* text = new EL_SkinText(this, id, info.text.c_str());
					g_pcSkin->ApplySkinWindow( text, info );
					if (info.color)
						text->SetForegroundColour(wxColor(info.color));
					if (info.bgColor)
						text->SetBackgroundColour(wxColor(info.bgColor));

					control = text;

					if (_T("TEXT_MAIN") == info.identity)
						m_mainTextPtr = text;
					else if (_T("TEXT_SUB") == info.identity)
						m_subTextPtr = text;
					else if (_T("TEXT_TORRENT") == info.identity)
						m_torrentTextPtr = text;
					else if (_T("TEXT_UPSPEED") == info.identity)
						m_upSpeedTextPtr = text;
					else if (_T("TEXT_DNSPEED") == info.identity)
						m_dnSpeedTextPtr = text;
					else if (_T("TEXT_PEERS") == info.identity)
						m_peersCountTextPtr = text;
					else if (_T("TEXT_SEEDS") == info.identity)
						m_seedsCountTextPtr = text;
				}
				break;
			case EL_ESkinControl_Button:
				{
					EL_SkinButton* button = new EL_SkinButton(this, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
					g_pcSkin->ApplySkinWindow( button, info );
					if(info.text.size())
						button->SetLabel(info.text.c_str());
					if(info.color)
						button->SetForegroundColour( wxColour( info.color ) );
					control = button;

					if ((info.attribute & SKINCONTROL_ATTRIBUTE_ENABLEONCOMPLETE) || (_T("START") == info.identity))
					{
						m_startButtonPtr = button;
						m_startButtonPtrs.push_back(button);
					}
				}
				break;
			case EL_ESkinControl_Gauge:
				{
					EL_SkinGauge* gauge = new EL_SkinGauge(this, id, PROGRESS_MAX, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxGA_HORIZONTAL | wxGA_SMOOTH);
					g_pcSkin->ApplySkinWindow( gauge, info );
					gauge->SetValue(0);
					control = gauge;

					if (info.identity == _T("PROGRESS_MAIN"))
						m_mainGaugePtr = gauge;
					else if (info.identity == _T("PROGRESS_SUB"))
						m_subGaugePtr = gauge;
				}
				break;
			case EL_ESkinControl_IFrame:
				{
					EL_WebBrowserPanel* panel = new EL_WebBrowserPanel(this, id, info.linkURL, wxPoint(info.pos.cx, info.pos.cy), wxSize(info.size.cx, info.size.cy));
					control = panel;
				}
			}

			if (control) {
				if (info.font.IsOk())
					control->SetFont(info.font);
				if ( (info.attribute & SKINCONTROL_ATTRIBUTE_ENABLEONCOMPLETE) || (info.identity == _T("START")) )
					control->Enable(false);
				control->Move(info.pos.cx, info.pos.cy);
				if (info.size.cx && info.size.cy)
					control->SetSize(info.size.cx, info.size.cy);
			}

			switch (info.type)
			{
			case EL_ESkinControl_Text:
				((EL_SkinText *) control)->CreateLabel();
				break;
			case EL_ESkinControl_Button:
			case EL_ESkinControl_Gauge:
				((EL_SkinWindow *) control)->StoreBackgroundBitmap();
				break;
			case EL_ESkinControl_IFrame:
				break;
			}

		}

		mainText.Show(false);
		subText.Show(false);
		torrentText.Show(false);
		upSpeedText.Show(false);
		dnSpeedText.Show(false);
		peersCountText.Show(false);
		seedsCountText.Show(false);
		mainGauge.Show(false);	
		subGauge.Show(false);
		funcButton.Show(false);
		versionText.Show(false);
		startButton.Show(false);
	}
	else
	{
		m_startButtonPtrs.push_back(&startButton);

		wxBoxSizer& hsizer = *new wxBoxSizer(wxHORIZONTAL);
		hsizer.Add(&versionText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
		hsizer.AddStretchSpacer();
		hsizer.Add(&startButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
		hsizer.Add(&funcButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

		wxBoxSizer& vsizer = *new wxBoxSizer(wxVERTICAL);
		vsizer.AddSpacer(5);
		vsizer.Add(&subText, 0, wxLEFT|wxRIGHT, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&subGauge, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&mainText, 0, wxLEFT|wxRIGHT, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&mainGauge, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddStretchSpacer();
		vsizer.Add(&torrentText, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&dnSpeedText, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&upSpeedText, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&peersCountText, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&seedsCountText, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		vsizer.Add(&hsizer, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
		vsizer.AddSpacer(5);
		
		SetSizer(&vsizer);	
	}

	if (NULL == m_mainTextPtr)
		m_mainTextPtr = &mainText;

	if (NULL == m_subTextPtr)
		m_subTextPtr = &subText;

	if (NULL == m_torrentTextPtr)
		m_torrentTextPtr = &torrentText;

	if (NULL == m_upSpeedTextPtr)
		m_upSpeedTextPtr = &upSpeedText;

	if (NULL == m_dnSpeedTextPtr)
		m_dnSpeedTextPtr = &dnSpeedText;

	if (NULL == m_peersCountTextPtr)
		m_peersCountTextPtr = &peersCountText;

	if (NULL == m_seedsCountTextPtr)
		m_seedsCountTextPtr = &seedsCountText;

	if (NULL == m_mainGaugePtr)
		m_mainGaugePtr = &mainGauge;	

	if (NULL == m_subGaugePtr)
		m_subGaugePtr = &subGauge;

	if (NULL == m_startButtonPtr)
		m_startButtonPtr = &startButton;

	BOOST_FOREACH(EL_SkinButton* startButtonPtr, m_startButtonPtrs)
		startButtonPtr->Enable(false);

	m_funcButtonPtr = &funcButton;

	m_hNoticeAnswerEvent = ::CreateEvent(NULL, true, false, L"TORRENT_PATCH_NOTICE_EVENT");
}

EL_TorrentPatchPanel::~EL_TorrentPatchPanel()
{
	MA_SAFE_CLOSEHANDLE(m_hNoticeAnswerEvent);
}

void EL_TorrentPatchPanel::__OnButtonClicked(wxCommandEvent& evt)
{
	if (g_pcSkin && ID_BUTTON_START <= evt.GetId() && ID_BUTTON_END >= evt.GetId())
	{
		int index = evt.GetId() - ID_BUTTON_START;

		const EL_SkinControlInfo info = g_pcSkin->GetControlInfo(index);
		// 과거 호환성을 위하여, QUIT 도 지원
		if (_T("CLOSE") == info.identity || _T("QUIT") == info.identity)
		{
			__OnClickCloseButton(evt);
		}
		if (_T("EXIT") == info.identity || _T("TERMINATE") == info.identity)
		{
			__OnClickExitButton(evt);
		}
		else if (_T("START") == info.identity)
		{
			if( !info.execArgs.empty() )
				m_runArgs = info.execArgs;

			if( false == info.secKey.empty() ) {
				CPostIt	cPostIt( "VOLUME1" );
				CHAR szTime[64];

				time_t timeNow = time(NULL);
				MA_StringFormatA( szTime, MA_ARRAYCOUNT(szTime), "%I64u", timeNow );
				cPostIt.Set( "SEC_KEY", info.secKey.c_str() );
				cPostIt.Set( "SEC_TIME", szTime );
			}

			__OnClickStartButton(evt);
		}
		else if (_T("MINIMIZE") == info.identity)
		{
			EL_TorrentPatchApp& app = wxGetApp();
			app.ShowFrame(false);
		}
		else if (false == info.linkURL.empty())
		{
			ShellExecute(NULL, _T("open"), info.linkURL.c_str(), NULL, NULL, SW_NORMAL);
		}
		else if (false == info.execPath.empty())
		{
			EL_CallProcess(info.execPath.c_str(), info.execArgs.c_str());
		}
	}
}

void EL_TorrentPatchPanel::__OnErase(wxEraseEvent& /*cEvent*/)
{
	//if (g_pcSkin)
	//{
	//	wxClientDC* clientDC = NULL;
	//	if (NULL == evt.GetDC())
	//		clientDC = new wxClientDC(this);

	//	wxDC* dc = clientDC ? clientDC : evt.GetDC();
	//	wxSize sz = GetClientSize();

	//	dc->DrawBitmap(g_pcSkin->GetMainBitmap(), 0, 0, false);
	//}
	//else
	//{
	//	evt.Skip();
	//}
}

void EL_TorrentPatchPanel::__OnPaint(wxPaintEvent& cEvent)
{
	if( g_pcSkin == NULL ) {
		cEvent.Skip();
		return;
	}

	wxPaintDC dc(this);

	dc.DrawBitmap(g_pcSkin->GetMainBitmap(), 0, 0, false);
}

void EL_TorrentPatchPanel::__OnClickCloseButton(wxCommandEvent& WXUNUSED(evt))
{
	EL_TorrentPatchApp& app = wxGetApp();
	wxWindow* pkTopWindow = app.GetTopWindow();

	if (pkTopWindow)
		pkTopWindow->Show(false);
}

void EL_TorrentPatchPanel::__OnClickExitButton(wxCommandEvent& WXUNUSED(evt))
{
	if (m_subTextPtr)
		m_subTextPtr->SetLabel(EL_LTEXT("TORRENT_STATE_CANCELING"));

	EL_TorrentPatchApp& app = wxGetApp();
	wxWindow* pkTopWindow = app.GetTopWindow();

	if (pkTopWindow)
		pkTopWindow->Destroy();
}

void EL_TorrentPatchPanel::ClientStart(DWORD cmdFlag)
{
	if( false == m_runPath.empty() ) {
		if (m_runProcessHandle) {
			CloseHandle(m_runProcessHandle);
			m_runProcessHandle = NULL;
		}

		MA_BOOL bSuccess = MA_FALSE;
		
		if (IS_SET(g_cmdFlags, CLIENT_PAUSE))
		{
			m_runArgs += std::wstring(L" --pause-before-create-window ");
		}

		EL_TorrentPatchApp& app = wxGetApp();
		std::wstring openidAuthkey = app.GetOpenidAuthkey();
		if( openidAuthkey.size() )
			m_runArgs += std::wstring(L" --openid-authkey ") + openidAuthkey;


		//char szXTrapArg[] = "660970B4483BDC32C5216D9844CFEF62EF7E8096F192E613B87958FCF1C17E66A1D394369127D9ABC78480D940BFD8554465ECDE99543E48CB547705D81248EF0F7D04245246E417A4610E5695477E1EF47455B309945CB0D27B7C7D383D0C40DBC997C22DB0B6C35D7EEE51FE06A38ED0CE0B7514";
		if( !m_xTrapArgument.empty() )
		{
			BYTE  szKey[XTEK_ARGV_SIZE_WITH_NULL] = {0,};
			XTrap_L_ExecKey(szKey, XTEK_ARGV_SIZE_WITH_NULL);

			XTrap_L_Patch( m_xTrapArgument.c_str(), NULL, 60 );

			EL_WCHAR wszKey[XTEK_ARGV_SIZE_WITH_NULL];
			mbstowcs( wszKey,(const char*)szKey, XTEK_ARGV_SIZE_WITH_NULL );

			std::wstring newRunArgs = m_runArgs + std::wstring(wszKey);

			bSuccess = EL_CallProcess(m_runPath.c_str(), newRunArgs.c_str(), &m_runProcessHandle);
		}
		else
		{
			bSuccess = EL_CallProcess(m_runPath.c_str(), m_runArgs.c_str(), &m_runProcessHandle);
		}
		
		if (bSuccess)
		{
			wxTorrentPatchNotifyEvent cEvent;

			__OnTorrentPatchSpawned(cEvent);
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
			MA_StringFormat(buf, MA_ARRAYCOUNT(buf), 
				MA_T("%s (code=%d, msg='%s', runPath='%s')"), EL_LTEXT("ERROR_EXECUTE_PROCESS"),
				error, lpMsgBuf, m_runPath.c_str());

			LocalFree(lpMsgBuf);

			wxTorrentPatchNotifyEvent errorEvt;
			errorEvt.SetText(buf);
			__OnTorrentPatchError(errorEvt);

			// 실행 파일에 문제가 생겼다면 템프 폴더를 삭제한다
			EL_Disk_RemoveDirectory(L"temp");
		}
	}
	else
	{
		wxTorrentPatchNotifyEvent errorEvt;
		errorEvt.SetText(EL_LTEXT("RUN_PATH_EMPTY"));
		__OnTorrentPatchError(errorEvt);
	}
}

void EL_TorrentPatchPanel::__OnClickStartButton(wxCommandEvent& WXUNUSED(evt))
{
	ClientStart();
}

void EL_TorrentPatchPanel::__OnTorrentPatchProgress(wxTorrentPatchProgressEvent& evt)
{
	static EL_PatchStatus oldProgressStatus;
	const EL_PatchStatus& newProgressStatus = evt.GetStatus();
	TCHAR tszTempString[256];

	if( m_mainTextPtr && ((oldProgressStatus.mainState != newProgressStatus.mainState) || (newProgressStatus.mainRatio != oldProgressStatus.mainRatio)) ) {
		oldProgressStatus.mainState = newProgressStatus.mainState;

		if( newProgressStatus.m_bDisplayMainRatioText && newProgressStatus.mainRatio >= 0.0f ) {
			MA_StringFormat( tszTempString, MA_ARRAYCOUNT(tszTempString), _T("%s %.1f%%"), newProgressStatus.mainState.c_str(), newProgressStatus.mainRatio * 100.0f );
			m_mainTextPtr->SetLabel( wxString( tszTempString ) );
		} else
			m_mainTextPtr->SetLabel(wxString(newProgressStatus.mainState.c_str(), newProgressStatus.mainState.length()));
	}

	if( m_subTextPtr && ((oldProgressStatus.subState != newProgressStatus.subState) || (newProgressStatus.subRatio != oldProgressStatus.subRatio)) ) {
		oldProgressStatus.subState = newProgressStatus.subState;
		if( newProgressStatus.m_bDisplaySubRatioText && newProgressStatus.subRatio >= 0.0f ) {
			MA_StringFormat( tszTempString, MA_ARRAYCOUNT(tszTempString), _T("%s %.1f%%"), newProgressStatus.subState.c_str(), newProgressStatus.subRatio * 100.0f );
			m_subTextPtr->SetLabel( wxString( tszTempString ) );
		} else
			m_subTextPtr->SetLabel(wxString(newProgressStatus.subState.c_str(), newProgressStatus.subState.length()));
	}

	if( m_torrentTextPtr && oldProgressStatus.m_strNameState != newProgressStatus.m_strNameState ) {
		oldProgressStatus.m_strNameState = newProgressStatus.m_strNameState;
		m_torrentTextPtr->SetLabel( wxString(newProgressStatus.m_strNameState.c_str(), newProgressStatus.m_strNameState.length()) );
	}

	if( m_dnSpeedTextPtr && oldProgressStatus.dnBytesPerSec != newProgressStatus.dnBytesPerSec ) {
		oldProgressStatus.dnBytesPerSec = newProgressStatus.dnBytesPerSec;

		EL_WCHAR dnSpeedTextBuf[1024];
		if( newProgressStatus.mainRatio >= 1.0f ) {
			dnSpeedTextBuf[0] = _T('\0');
		} else if( newProgressStatus.dnBytesPerSec < 0 ) {
			MA_StringFormat(dnSpeedTextBuf, MA_ARRAYCOUNT(dnSpeedTextBuf), L"%s: %s", EL_LTEXT("TORRENT_DOWNLOAD_SPEED"), L"∞");
		} else {			
			const std::wstring strBPS = EL_GetBPSString( newProgressStatus.dnBytesPerSec );

			MA_StringFormat(dnSpeedTextBuf, MA_ARRAYCOUNT(dnSpeedTextBuf), L"%s %s", EL_LTEXT("TORRENT_DOWNLOAD_SPEED"), strBPS.c_str() );
		}
		m_dnSpeedTextPtr->SetLabel(wxString(dnSpeedTextBuf));
	}

	if (m_upSpeedTextPtr && oldProgressStatus.upBytesPerSec != newProgressStatus.upBytesPerSec)
	{
		oldProgressStatus.upBytesPerSec = newProgressStatus.upBytesPerSec;

		EL_WCHAR upSpeedTextBuf[1024];
		if (newProgressStatus.upBytesPerSec < 0) {
			MA_StringFormat( upSpeedTextBuf, sizeof(upSpeedTextBuf)/sizeof(EL_WCHAR)-1, L"%s %s", EL_LTEXT("TORRENT_UPLOAD_SPEED"), L"∞" );
		} else {		
			const std::wstring strBPS = EL_GetBPSString( newProgressStatus.upBytesPerSec );

			MA_StringFormat( upSpeedTextBuf, sizeof(upSpeedTextBuf)/sizeof(EL_WCHAR)-1, L"%s %s", EL_LTEXT("TORRENT_UPLOAD_SPEED"), strBPS.c_str() );
		}
		m_upSpeedTextPtr->SetLabel(wxString(upSpeedTextBuf));
	}

	if (m_peersCountTextPtr &&
		(oldProgressStatus.peersCount != newProgressStatus.peersCount ||
		 oldProgressStatus.totalPeersCount != newProgressStatus.totalPeersCount))
	{
		oldProgressStatus.peersCount = newProgressStatus.peersCount;
		oldProgressStatus.totalPeersCount = newProgressStatus.totalPeersCount;

		EL_WCHAR peersCountTextBuf[1024];
		MA_INT peersCountTextLen;
		
		peersCountTextLen = MA_StringFormat( peersCountTextBuf, MA_ARRAYCOUNT(peersCountTextBuf), L"%s %d/%d", EL_LTEXT("TORRENT_PEERS_COUNT"), newProgressStatus.peersCount, newProgressStatus.totalPeersCount);
		m_peersCountTextPtr->SetLabel(wxString(peersCountTextBuf, peersCountTextLen));
	}

	if (m_seedsCountTextPtr &&
		(oldProgressStatus.seedsCount != newProgressStatus.seedsCount ||
		 oldProgressStatus.totalSeedsCount != newProgressStatus.totalSeedsCount))
	{
		oldProgressStatus.seedsCount = newProgressStatus.seedsCount;
		oldProgressStatus.totalSeedsCount = newProgressStatus.totalSeedsCount;

		EL_WCHAR seedsCountTextBuf[1024];

#ifdef _DEBUG
		MA_StringFormat(seedsCountTextBuf,
			MA_ARRAYCOUNT(seedsCountTextBuf),
			L"%s %d/%d (%d)", EL_LTEXT("TORRENT_SEEDS_COUNT"), newProgressStatus.seedsCount, newProgressStatus.totalSeedsCount, newProgressStatus.webSeedsCount);
#else
		MA_StringFormat(seedsCountTextBuf,
			MA_ARRAYCOUNT(seedsCountTextBuf),
			L"%s %d/%d", EL_LTEXT("TORRENT_SEEDS_COUNT"), newProgressStatus.seedsCount, newProgressStatus.totalSeedsCount);
#endif
		m_seedsCountTextPtr->SetLabel(wxString(seedsCountTextBuf));
	}

	if (m_subGaugePtr && oldProgressStatus.subRatio != newProgressStatus.subRatio)
	{
		oldProgressStatus.subRatio = newProgressStatus.subRatio;

		m_subGaugePtr->SetValue(PROGRESS_MAX * newProgressStatus.subRatio);		
	}

	if (m_mainGaugePtr && oldProgressStatus.mainRatio != newProgressStatus.mainRatio)
	{
		oldProgressStatus.mainRatio = newProgressStatus.mainRatio;

		m_mainGaugePtr->SetValue(PROGRESS_MAX * newProgressStatus.mainRatio);
	}
	this->InvalidateBestSize();
}

void EL_TorrentPatchPanel::__OnTorrentPatchSpawned(wxTorrentPatchNotifyEvent& WXUNUSED(evt))
{
	BOOST_FOREACH(EL_SkinButton* startButtonPtr, m_startButtonPtrs)
		startButtonPtr->Enable(false);

	EL_TorrentPatchApp& app = wxGetApp();
	app.ShowFrame(false);

	const bool bNeedTerminate = m_bRunWithExit;
	if (bNeedTerminate)
	{
		wxWindow* pkTopWindow = app.GetTopWindow();

		if (pkTopWindow)
			pkTopWindow->Destroy();	
	}
}

void EL_TorrentPatchPanel::__OnTorrentPatchFinished(wxTorrentPatchNotifyEvent& WXUNUSED(evt))
{
	if( m_subTextPtr )
		m_subTextPtr->SetLabel( EL_LTEXT("TORRENT_STATE_FINISHED") );

	if( m_startButtonPtr->IsEnabled() )
		m_bRunWithExit = true;
	else {
		EL_TorrentPatchApp& app = wxGetApp();
		wxWindow* pkTopWindow = app.GetTopWindow();

		if (pkTopWindow)
			pkTopWindow->Destroy();	
	}
}

void EL_TorrentPatchPanel::__OnChangeConfig(wxTorrentPatchConfigEvent& evt)
{
	m_runPath		= evt.GetConfig().runPath;
	m_xTrapArgument = evt.GetConfig().xTrapPatcherArg;
}

#include <shellapi.h>

void EL_TorrentPatchPanel::__OnTorrentPatchError(wxTorrentPatchNotifyEvent& evt)
{
	//Silent mode
	if (false == evt.GetText().empty())
	{
		if (evt.GetText().empty()) {
			// AGREENMENT REFUSE 시는 에러 텍스트가 없다
		} else {
			wxMessageBox( evt.GetText().c_str(), EL_LTEXT("TORRENT_PATCH_ERROR"), wxOK | wxICON_INFORMATION, NULL );
		}

		SHFILEOPSTRUCT fo;
		ZeroMemory(&fo, sizeof(fo));
		fo.wFunc = FO_DELETE;
		fo.pFrom = L".\\Temp\0";
		fo.fFlags = (FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR);
		fo.lpszProgressTitle = L"Delete fast resume data\0";
		SHFileOperation(&fo);
	}

	EL_TorrentPatchApp& app = wxGetApp();
	wxWindow* pkTopWindow = app.GetTopWindow();

	if (pkTopWindow)
		pkTopWindow->Destroy();	
}

void EL_TorrentPatchPanel::__OnTorrentPatchReadyForGameStart(wxTorrentPatchNotifyEvent& evt)
{
	m_bRunWithExit = EL_LTEXT("terminate") == evt.GetText();

	BOOST_FOREACH(EL_SkinButton* startButtonPtr, m_startButtonPtrs)
		startButtonPtr->Enable(true);
}

void EL_TorrentPatchPanel::__OnIdle(wxIdleEvent& /*evt*/)
{
	if (m_runProcessHandle && false == m_startButtonPtr->IsEnabled())
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_runProcessHandle, 20))
		{
			BOOST_FOREACH(EL_SkinButton* startButtonPtr, m_startButtonPtrs)
				startButtonPtr->Enable();

			CloseHandle(m_runProcessHandle);
			m_runProcessHandle = NULL;
		}
	}
}

class EL_TorrentNoticeDialog : public wxDialog
{
public:
	EL_TorrentNoticeDialog(wxWindow* parent, const std::wstring& url, const wxSize& size);
};

EL_TorrentNoticeDialog::EL_TorrentNoticeDialog(wxWindow* parent, const std::wstring& url, const wxSize& size)
: wxDialog(parent, -1, EL_LTEXT("TORRENT_NOTICE_TITLE"), wxDefaultPosition, size)
{	
	wxBoxSizer& vsizer = *new wxBoxSizer(wxVERTICAL);

	wxPanel& panel = *new EL_WebBrowserPanel(this, wxID_ANY, url.c_str(), wxDefaultPosition, wxDefaultSize);
	vsizer.Add(&panel, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer& hsizer = *new wxBoxSizer(wxHORIZONTAL);	
	hsizer.Add(new wxButton(this, wxID_CANCEL, EL_LTEXT("TORRENT_NOTICE_REFUSE")), 0, wxALL, 5);
	hsizer.Add(new wxButton(this, wxID_OK,EL_LTEXT("TORRENT_NOTICE_ACCEPT")), 0, wxALL, 5);
	vsizer.Add(&hsizer, 0, wxCENTER);

	SetSizer(&vsizer);
}

void EL_TorrentPatchPanel::__OnTorrentPatchNotice(wxTorrentPatchNotifyEvent& evt)
{
	EL_TorrentNoticeDialog dlg(this, evt.GetText(), evt.GetSize());
	m_isNoticeAnswer = dlg.ShowModal() == wxID_OK;
	dlg.Destroy();

	::SetEvent(m_hNoticeAnswerEvent);
}

bool EL_TorrentPatchPanel::WaitForNoticeAnswer()
{
	if (::WaitForSingleObject(m_hNoticeAnswerEvent, 60*60*1000) == WAIT_OBJECT_0)
		return m_isNoticeAnswer;
	else
		return false;
}
