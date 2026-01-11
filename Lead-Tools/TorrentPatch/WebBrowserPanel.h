#pragma once

#include <wx-2.8.10/panel.h>

class EL_WebBrowserPanel : public wxPanel
{
public:
	DECLARE_EVENT_TABLE();

public:
	EL_WebBrowserPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& URL = TEXT("about:blank"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~EL_WebBrowserPanel();

	void Go(EL_C_WCHAR_P URL);

protected:
	void __OnSize(wxSizeEvent& evt);

private:
	class CWebControlImpl* m_webControl;
};