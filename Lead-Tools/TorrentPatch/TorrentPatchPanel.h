#pragma once

#include <wx-2.8.10/panel.h>

class EL_SkinButton;
class EL_SkinText;
class EL_SkinGauge;
class wxTorrentPatchProgressEvent;
class wxTorrentPatchNotifyEvent;
class wxTorrentPatchConfigEvent;

class EL_TorrentPatchPanel : public wxPanel
{
public:
	DECLARE_EVENT_TABLE();

public:
	enum
	{
		PROGRESS_MAX = 10000,
	};

	EL_TorrentPatchPanel(wxWindow *parent);		

	bool WaitForNoticeAnswer();

public:
	virtual ~EL_TorrentPatchPanel();

	void ClientStart(DWORD cmdFlag = 0);

private:
	void __OnChangeConfig(wxTorrentPatchConfigEvent&);
	void __OnClickCloseButton(wxCommandEvent& WXUNUSED(evt));
	void __OnClickExitButton(wxCommandEvent& WXUNUSED(evt));
	void __OnClickStartButton(wxCommandEvent& WXUNUSED(evt));
	void __OnTorrentPatchProgress(wxTorrentPatchProgressEvent& evt);
	void __OnTorrentPatchSpawned(wxTorrentPatchNotifyEvent& evt);
	void __OnTorrentPatchFinished(wxTorrentPatchNotifyEvent& evt);
	void __OnTorrentPatchError(wxTorrentPatchNotifyEvent& evt);
	void __OnTorrentPatchNotice(wxTorrentPatchNotifyEvent& evt);
	void __OnTorrentPatchReadyForGameStart(wxTorrentPatchNotifyEvent& evt);
	void __OnErase(wxEraseEvent& evt);
	void __OnPaint(wxPaintEvent& evt);
	void __OnButtonClicked(wxCommandEvent& evt);
	void __OnLeftDown(wxMouseEvent& evt);
	void __OnIdle(wxIdleEvent& evt);

private:
	EL_SkinText* m_subTextPtr;	
	EL_SkinText* m_mainTextPtr;
	EL_SkinText* m_torrentTextPtr;
	EL_SkinText* m_upSpeedTextPtr;
	EL_SkinText* m_dnSpeedTextPtr;
	EL_SkinText* m_peersCountTextPtr;
	EL_SkinText* m_seedsCountTextPtr;

	EL_SkinGauge* m_subGaugePtr;	
	EL_SkinGauge* m_mainGaugePtr;

	EL_SkinButton* m_funcButtonPtr;
	EL_SkinButton* m_startButtonPtr;
	std::vector<EL_SkinButton *> m_startButtonPtrs;
	std::wstring m_runPath;
	std::wstring m_runArgs;
	bool m_bRunWithExit;
	HANDLE m_runProcessHandle;

	HANDLE m_hNoticeAnswerEvent;

	std::string m_xTrapArgument;

private:
	bool m_isNoticeAnswer;
};