#include "Metin2TorrentConsole.h"
#include "SkinWindow.h"
#include "TorrentSkin.h"
#include <wx-2.8.10/dcbuffer.h>

BEGIN_EVENT_TABLE(EL_SkinWindow,wxWindow)
	EVT_ERASE_BACKGROUND(EL_SkinWindow::OnErase)
	EVT_PAINT(EL_SkinWindow::OnPaint)
	EVT_SIZE(EL_SkinWindow::OnSize)
	EVT_ENTER_WINDOW(EL_SkinWindow::OnEnterWindow)
	EVT_LEAVE_WINDOW(EL_SkinWindow::OnLeaveWindow)
END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(EL_SkinWindow,wxWindow);

EL_SkinWindow::EL_SkinWindow()
{
	m_iType			= -1;
	m_bScaleFill	= false;
	m_bInside		= false;
}

EL_SkinWindow::EL_SkinWindow(wxWindow *parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name,
	const int iType)
	: wxWindow(parent,id,pos,size,style,name)
{
	m_iType			= iType;
	m_bScaleFill	= false;
	m_bInside		= false;
}

void EL_SkinWindow::SetNormalImage(const wxImage& skin)
{ 
	m_bmpNormal = skin; 
}

void EL_SkinWindow::SetOverImage(const wxImage& skin)
{
	m_bmpOver = skin;
}

void EL_SkinWindow::SetDisabledImage(const wxImage& skin)
{ 
	m_bmpDisabled = skin; 
}

bool EL_SkinWindow::SetShape(const wxImage& imgorg)
{
	wxImage img = imgorg;
	wxRegion region;
	if(img.HasAlpha())
		img.ConvertAlphaToMask();

	region = wxRegion(wxBitmap(img));

	return SetShape(region);
}

bool EL_SkinWindow::SetShape(const wxBitmap& img)
{
	wxRegion region;
	region = wxRegion(img);
	
	return SetShape(region);
}

bool EL_SkinWindow::SetShape(const wxRegion& region)
{
	// The empty region signifies that the shape should be removed from the window.
    if ( region.IsEmpty() )
    {
        if (::SetWindowRgn(GetHwnd(), NULL, TRUE) == 0)
        {
            wxLogLastError(_T("SetWindowRgn"));
            return false;
        }
        return true;
    }

    DWORD noBytes = ::GetRegionData(GetHrgnOf(region), 0, NULL);
    RGNDATA *rgnData = (RGNDATA*) new char[noBytes];
    ::GetRegionData(GetHrgnOf(region), noBytes, rgnData);
    HRGN hrgn = ::ExtCreateRegion(NULL, noBytes, rgnData);
    delete[] (char*) rgnData;
   
    RECT rect;
    DWORD dwStyle =   ::GetWindowLong(GetHwnd(), GWL_STYLE);
    DWORD dwExStyle = ::GetWindowLong(GetHwnd(), GWL_EXSTYLE);
    ::GetClientRect(GetHwnd(), &rect);
    ::AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
    ::OffsetRgn(hrgn, -rect.left, -rect.top);

    if (::SetWindowRgn(GetHwnd(), hrgn, TRUE) == 0)
    {
        wxLogLastError(_T("SetWindowRgn"));
        return false;
    }
    return true;
}

void EL_SkinWindow::OnSize( wxSizeEvent& /*cEvent*/ )
{
	//StoreBackgroundBitmap();

	if( m_bmpNormal.IsOk() ) {
		//int h, w;

		//GetSize(&h,&w);

		//if( w>0 && h>0 ) {
		//	wxBitmap temp = wxBitmap(m_bmpNormal.Scale(h+1, w+1));
		//	SetShape(temp);
		//}
		//Refresh();
	}
	Refresh();
}

void EL_SkinWindow::OnErase(wxEraseEvent& /*event*/)
{
}

void EL_SkinWindow::OnPaint(wxPaintEvent& /*event*/)
{
	wxBufferedPaintDC dcp(this);

	if( m_bmpBackground.IsOk() ) {
		dcp.DrawBitmap(m_bmpBackground, 0, 0, false);
	}
	if( m_bScaleFill ) {
		int h, w;
		GetSize(&h,&w);
		
		if(m_bInside && m_bmpOver.IsOk())
			dcp.DrawBitmap(wxBitmap(m_bmpOver.Scale(h, w)),0,0,true);
		else if(IsEnabled() && m_bmpNormal.IsOk())
			dcp.DrawBitmap(wxBitmap(m_bmpNormal.Scale(h, w)),0,0,true);
		else if( m_bmpDisabled.IsOk() )
			dcp.DrawBitmap(wxBitmap(m_bmpDisabled.Scale(h, w)),0,0,true);
	} else {
		if(m_bInside && m_bmpOver.IsOk())
			dcp.DrawBitmap(m_bmpOver,0,0,true);
		if(IsEnabled() && m_bmpNormal.IsOk())
			dcp.DrawBitmap(m_bmpNormal,0,0,true);
		else if( m_bmpDisabled.IsOk() )
			dcp.DrawBitmap(m_bmpDisabled,0,0,true);
	}

	DrawCustom(dcp);
}

void EL_SkinWindow::OnEnterWindow(wxMouseEvent& WXUNUSED(event))
{
	m_bInside = true;
	if(m_bmpOver.IsOk())
		Refresh();
}

bool EL_SkinWindow::Enable( bool bEnable )
{
	bool bRet = wxWindow::Enable( bEnable );
	Refresh();
	return bRet;
}

void EL_SkinWindow::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
	m_bInside = false;
	if(HasCapture())
		ReleaseMouse();	
	Refresh();
}

bool EL_SkinWindow::StoreBackgroundBitmap( int nX, int nY, int nWidth, int nHeight )
{
	if( nX < 0 || nY < 0 )
		GetPosition( &nX, &nY );
	if( nWidth < 0 || nHeight < 0 )
		GetSize( &nWidth, &nHeight );

	wxMemoryDC cDC;

	m_bmpBackground.Create(nWidth, nHeight, -1);

	cDC.SelectObject(m_bmpBackground);
	if( g_pcSkin && g_pcSkin->GetMainBitmap().IsOk() )
		cDC.DrawBitmap( g_pcSkin->GetMainBitmap(), -nX, -nY );
	cDC.SelectObject(wxNullBitmap);

	return true;
}
