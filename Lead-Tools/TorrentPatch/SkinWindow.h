#pragma once

class EL_SkinWindow : public wxWindow
{
public:
	EL_SkinWindow();

	EL_SkinWindow(wxWindow *parent,
		wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxNO_BORDER,
		const wxString& name = wxPanelNameStr,
		const int iType = -1);

	bool SetShape(const wxRegion& region);
	bool SetShape(const wxImage& img);
	bool SetShape(const wxBitmap& img);

	void SetNormalImage(const wxImage& skin = wxNullImage);
	void SetOverImage(const wxImage& skin = wxNullImage);
	void SetDisabledImage(const wxImage& skin = wxNullImage);

	wxImage& GetNormalImage( void )					{ return m_bmpNormal; }
	wxBitmap& GetBackgroundImage( void )				{ return m_bmpBackground; }

	virtual bool StoreBackgroundBitmap( int nX = -1, int nY = -1, int nWidth = -1, int nHeight = -1 );

	void SetScaleFill(bool bScaleFill)				{ m_bScaleFill = bScaleFill;}

	int GetSkinWindowType()							{ return m_iType; }

    virtual bool Enable( bool enable = true );

	virtual void DrawCustom(wxDC& dc) = 0;

	virtual bool HasTransparentBackground()			{ return true; }

private:
	void OnErase(wxEraseEvent& e);
	void OnSize(wxSizeEvent& e);
	void OnPaint(wxPaintEvent& e);
	void OnEnterWindow(wxMouseEvent& WXUNUSED(event));
	void OnLeaveWindow(wxMouseEvent& WXUNUSED(event));
	
	DECLARE_ABSTRACT_CLASS(wxSkinWindow);
	DECLARE_EVENT_TABLE()

protected:
	int			m_iType;
	wxBitmap	m_bmpBackground;
	wxImage		m_bmpNormal;
	wxImage		m_bmpOver;
	wxImage		m_bmpDisabled;
	bool		m_bScaleFill;
	bool		m_bInside;
};
