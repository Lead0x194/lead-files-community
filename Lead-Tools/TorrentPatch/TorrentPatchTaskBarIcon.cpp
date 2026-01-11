#include "Metin2TorrentConsole.h"
#include "TorrentPatchTaskBarIcon.h"
#include <wx-2.8.10/window.h>
#include <wx-2.8.10/frame.h>
#include <wx-2.8.10/msgdlg.h>
#include <wx-2.8.10/menu.h>
#include "TorrentPatchEvent.h"
#include "TorrentPatchApp.h"

enum {
    PU_SHOWHIDE = 10001,
    //PU_NEW_ICON,
    //PU_OLD_ICON,
    PU_EXIT,
	PU_BANDWIDTH_UNLIMITED,
	PU_BANDWIDTH_START,
	PU_SUBMAIN = PU_BANDWIDTH_START + 1000,
};

BEGIN_EVENT_TABLE(EL_TorrentPatchTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_SHOWHIDE, EL_TorrentPatchTaskBarIcon::OnMenuShowHide)
    EVT_MENU(PU_EXIT, EL_TorrentPatchTaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_DCLICK(EL_TorrentPatchTaskBarIcon::OnLeftButtonDClick)
	EVT_MENU(PU_BANDWIDTH_UNLIMITED, EL_TorrentPatchTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_BANDWIDTH_UNLIMITED, EL_TorrentPatchTaskBarIcon::OnMenuUICheckmark)
	EVT_TORRRNT_PATCH_CONFIG(EL_TorrentPatchTaskBarIcon::OnChangeConfig)
END_EVENT_TABLE()

void EL_TorrentPatchTaskBarIcon::OnMenuShowHide(wxCommandEvent& )
{
	if (m_parent)
	{
		m_parent->Show(!m_parent->IsShown());

		wxFrame * pkFrame = dynamic_cast<wxFrame *>(m_parent);

		if (pkFrame && pkFrame->IsIconized())
			pkFrame->Maximize(false);
	}
}

void EL_TorrentPatchTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
	if (m_parent)
	{
		m_parent->Destroy();
		m_parent = NULL;
	}
}

void EL_TorrentPatchTaskBarIcon::OnMenuCheckmark(wxCommandEvent& evt)
{
	if (evt.GetId() == PU_BANDWIDTH_UNLIMITED)
		m_config.currentBandwidth = 0;
	else
	{
		EL_UINT idx = evt.GetId() - PU_BANDWIDTH_START;

		if (idx < m_config.bandwidths.size())
			m_config.currentBandwidth = m_config.bandwidths[idx] * 1024;
	}

	EL_TorrentPatchApp & app = wxGetApp();
	app.m_changeBandwidthQueue.Enqueue(m_config.currentBandwidth);
}

void EL_TorrentPatchTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &evt)
{
	bool check = false;

	if (evt.GetId() == PU_BANDWIDTH_UNLIMITED)
		check = (0 == m_config.currentBandwidth);
	else
	{
		EL_UINT idx = evt.GetId() - PU_BANDWIDTH_START;

		if (idx < m_config.bandwidths.size())
			check = (((EL_UINT) m_config.bandwidths[idx] * 1024) == m_config.currentBandwidth);
	}

    evt.Check(check);
}

void EL_TorrentPatchTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox(wxT("You clicked on a submenu!"));
}

// Overridables
wxMenu *EL_TorrentPatchTaskBarIcon::CreatePopupMenu()
{
    // Try creating menus different ways
    // TODO: Probably try calling SetBitmap with some XPMs here
    wxMenu *menu = new wxMenu;

	menu->Append(PU_SHOWHIDE, EL_LTEXT("TORRENT_TRAY_SHOWHIDE"));

	wxMenu *submenu = new wxMenu;
	
	if( m_config.bandwidths.size() > 0 ) {
	    menu->AppendSeparator();

		EL_WCHAR buf[128];
		int i;

		submenu->Append(PU_BANDWIDTH_UNLIMITED, EL_LTEXT("TORRENT_TRAY_BANDWIDTH_UNLIMITED"), wxT(""), wxITEM_CHECK);
		submenu->AppendSeparator();

		for (i = 0; i < (int) m_config.bandwidths.size(); ++i)
		{
			swprintf_s(buf, sizeof(buf) / sizeof(EL_WCHAR), L"%ukB/s", m_config.bandwidths[i]);

			submenu->Append(PU_BANDWIDTH_START + i, buf, wxT(""), wxITEM_CHECK);

			Connect(PU_BANDWIDTH_START + i, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
						(wxObjectEventFunction)
						(wxEventFunction)
						(wxCommandEventFunction)&EL_TorrentPatchTaskBarIcon::OnMenuCheckmark);

			Connect(PU_BANDWIDTH_START + i, wxID_ANY, wxEVT_UPDATE_UI,
						(wxObjectEventFunction)
						(wxEventFunction)
						(wxCommandEventFunction)&EL_TorrentPatchTaskBarIcon::OnMenuUICheckmark);
		}

		if (i != 0)
			menu->Append(PU_SUBMAIN, EL_LTEXT("TORRENT_TRAY_BANDWIDTH_ALLOCATION"), submenu);
	}

#ifndef __WXMAC_OSX__ /*Mac has built-in quit menu*/
    menu->AppendSeparator();
    menu->Append(PU_EXIT,    EL_LTEXT("TORRENT_TRAY_EXIT"));
#endif
    return menu;
}

void EL_TorrentPatchTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if (m_parent)
	{
		m_parent->Show(true);

		wxFrame * pkFrame = dynamic_cast<wxFrame *>(m_parent);

		if (pkFrame && pkFrame->IsIconized())
			pkFrame->Maximize(false);
	}
}

void EL_TorrentPatchTaskBarIcon::OnChangeConfig(wxTorrentPatchConfigEvent& evt)
{
	for (int i = 0; i < (int) m_config.bandwidths.size(); ++i)
		Disconnect(PU_BANDWIDTH_START + i, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED);

	m_config = evt.GetConfig();
}
