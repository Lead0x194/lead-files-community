#pragma once

#include <wx-2.8.10/taskbar.h>

class wxTorrentPatchConfigEvent;

class EL_TorrentPatchTaskBarIcon: public wxTaskBarIcon
{
public:
	EL_TorrentPatchTaskBarIcon(wxWindow *parent) : m_parent(parent)
    {}

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuShowHide(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuSetOldIcon(wxCommandEvent&);
    void OnMenuCheckmark(wxCommandEvent&);
    void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
	void OnChangeConfig(wxTorrentPatchConfigEvent&);
    virtual wxMenu *CreatePopupMenu();

protected:
	wxWindow* m_parent;
	EL_PatchConfig m_config;

DECLARE_EVENT_TABLE()
};