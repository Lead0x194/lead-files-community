#pragma once

#include "SkinWindow.h"

class EL_SkinButton : public EL_SkinWindow
{
public:

	EL_SkinButton();

	EL_SkinButton(wxWindow* parent,
		int id,
		const wxString& label = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxNO_BORDER,
		const wxString& name = wxT("EL_SkinButton"));

	~EL_SkinButton();

	void SetPressedImage(const wxImage& image = wxNullImage);

	void SetLabel(const wxString& label);
	bool SetForegroundColour(const wxColour& colour);

	virtual void DrawCustom(wxDC& dc);

protected:
	virtual wxSize DoGetBestSize() const;

private:
	void OnLeftClick(wxMouseEvent& WXUNUSED(event));
	void OnLeftRelease(wxMouseEvent& WXUNUSED(event));

	DECLARE_DYNAMIC_CLASS(EL_SkinButton);
	DECLARE_EVENT_TABLE()

protected:
	wxImage m_bmpPressed;
};
