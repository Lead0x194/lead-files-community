#pragma once

class EL_TorrentPatchPanel;
class EL_TorrentPatchTaskBarIcon;
class EL_Skin;

class EL_TorrentPatchFrame : public wxFrame
{
public:
	EL_TorrentPatchFrame(const wxString& title);
	virtual ~EL_TorrentPatchFrame();

	virtual bool Destroy();

	void StartPatchThread();
	void Shutdown();

	void OnDynamic(wxCommandEvent& );

private:
	void	OnErase( wxEraseEvent& evt );
	void	OnClose( wxCloseEvent& evt );

	DECLARE_EVENT_TABLE();

private:
	wxThread*					m_patchThreadPtr;
	EL_TorrentPatchPanel*		m_patchPanelPtr;
	EL_TorrentPatchTaskBarIcon*	m_taskBarIcon;
};