#pragma once

#include <wx-2.8.10/event.h>

enum
{
	ID_NONE,
	ID_FRAME,
	ID_PANEL,
	ID_PANEL_CLOSE,
	ID_PANEL_EXIT,
	ID_PANEL_START,
};

enum
{
	ID_BUTTON_START = 0x100,
	ID_BUTTON_END = 0x200
};

DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_PROGRESS, -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_SPAWNED,  -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_FINISHED, -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_ERROR, -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_NOTICE, -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_CONFIG, -1 )
DECLARE_EVENT_TYPE( wxEVT_TORRENT_PATCH_READYFORGAMESTART, -1 )

class wxTorrentPatchProgressEvent : public wxEvent
{
public:
    wxTorrentPatchProgressEvent(wxEventType type = wxEVT_NULL, int winid = 0) : wxEvent(winid, type)
	{}

    wxTorrentPatchProgressEvent(const wxTorrentPatchProgressEvent & evt)
	: wxEvent(evt)
	, m_status(evt.m_status)
	{}
    
    virtual wxEvent *Clone() const
	{ return new wxTorrentPatchProgressEvent(*this); }

	void SetStatus(const EL_PatchStatus& status)
	{m_status = status;}
	
	const EL_PatchStatus& GetStatus()
	{return m_status;}

private:
	EL_PatchStatus m_status;	

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxTorrentPatchProgressEvent)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////

class wxTorrentPatchNotifyEvent : public wxEvent
{
public:
    wxTorrentPatchNotifyEvent(wxEventType type = wxEVT_NULL, int winid = 0) : wxEvent(winid, type)
	{}

    wxTorrentPatchNotifyEvent(const wxTorrentPatchNotifyEvent & evt)
	: wxEvent(evt)
	, m_text(evt.m_text)
	, m_size(evt.m_size)
	{}
    
    virtual wxEvent *Clone() const
	{ return new wxTorrentPatchNotifyEvent(*this); }	

	void SetText(const std::wstring& text)
	{m_text = text;}

	const std::wstring& GetText()
	{return m_text;}

	void SetSize(const wxSize& size)
	{m_size = size;}

	const wxSize& GetSize()
	{return m_size;}


private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxTorrentPatchNotifyEvent)

private:
	std::wstring m_text;
	wxSize m_size;
};

class wxTorrentPatchConfigEvent : public wxEvent
{
public:
    wxTorrentPatchConfigEvent(wxEventType type = wxEVT_NULL, int winid = 0) : wxEvent(winid, type)
	{}

    wxTorrentPatchConfigEvent(const wxTorrentPatchConfigEvent & evt)
	: wxEvent(evt)
	, m_config(evt.m_config)
	{}
    
    virtual wxEvent *Clone() const
	{ return new wxTorrentPatchConfigEvent(*this); }

	void SetConfig(const EL_PatchConfig& config)
	{m_config = config;}
	
	const EL_PatchConfig& GetConfig()
	{return m_config;}

private:
	EL_PatchConfig m_config;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxTorrentPatchConfigEvent)
};

typedef void (wxEvtHandler::*wxTorrentPatchProgressEventFunction)(wxTorrentPatchProgressEvent&);
typedef void (wxEvtHandler::*wxTorrentPatchNotifyEventFunction)(wxTorrentPatchNotifyEvent&);
typedef void (wxEvtHandler::*wxTorrentPatchConfigEventFunction)(wxTorrentPatchConfigEvent&);

#define wxTorrentPatchProgressEventHandler(func) (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTorrentPatchProgressEventFunction, &func)
#define wxTorrentPatchNotifyEventHandler(func) (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTorrentPatchNotifyEventFunction, &func)
#define wxTorrentPatchConfigEventHandler(func) (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTorrentPatchConfigEventFunction, &func)

#define EVT_TORRENT_PATCH_PROGRESS(winid, func) wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_PROGRESS, winid, wxTorrentPatchProgressEventHandler(func))
#define EVT_TORRENT_PATCH_SPAWNED(winid, func)  wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_SPAWNED, winid, wxTorrentPatchNotifyEventHandler(func))
#define EVT_TORRENT_PATCH_FINISHED(winid, func) wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_FINISHED, winid, wxTorrentPatchNotifyEventHandler(func))
#define EVT_TORRENT_PATCH_ERROR(winid, func) wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_ERROR, winid, wxTorrentPatchNotifyEventHandler(func))
#define EVT_TORRENT_PATCH_NOTICE(winid, func) wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_NOTICE, winid, wxTorrentPatchNotifyEventHandler(func))
#define EVT_TORRRNT_PATCH_CONFIG(func) wx__DECLARE_EVT0(wxEVT_TORRENT_PATCH_CONFIG, wxTorrentPatchConfigEventHandler(func))
#define EVT_TORRENT_PATCH_READYFORGAMESTART(winid, func) wx__DECLARE_EVT1(wxEVT_TORRENT_PATCH_READYFORGAMESTART, winid, wxTorrentPatchNotifyEventHandler(func))
