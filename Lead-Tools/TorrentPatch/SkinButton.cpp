#include "Metin2TorrentConsole.h"
#include "SkinButton.h"
#include "TorrentSkin.h"

BEGIN_EVENT_TABLE(EL_SkinButton,EL_SkinWindow)
	EVT_LEFT_DOWN(EL_SkinButton::OnLeftClick)
	EVT_LEFT_UP(EL_SkinButton::OnLeftRelease)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EL_SkinButton,EL_SkinWindow)

EL_SkinButton::EL_SkinButton()
	: EL_SkinWindow()
{
}

EL_SkinButton::EL_SkinButton(wxWindow* parent,
 				int id,
 				const wxString& label,
  				const wxPoint& pos,
         		const wxSize& size,
           		long style,
				const wxString& name)
	:EL_SkinWindow(parent,id,pos,size,style,name,EL_ESkinControl_Button)
{
	wxString tmp_lab = label;
	tmp_lab.Replace(wxT("&"),wxT(""));
	SetLabel(tmp_lab);
}

EL_SkinButton::~EL_SkinButton()
{
}

void EL_SkinButton::SetPressedImage(const wxImage& image)
{
	m_bmpPressed = image;
}

wxSize EL_SkinButton::DoGetBestSize() const
{	
	if(!GetLabel().IsEmpty())
	{	int x, y;
		wxClientDC dc((wxWindow*)this);
		dc.GetTextExtent(GetLabel(),&x,&y);
	
		return	wxSize( x+6, y+6 );
	}
	else
		if(m_bmpNormal.IsOk())
			return wxSize(m_bmpNormal.GetWidth(),m_bmpNormal.GetHeight());

	return wxSize(10,10);
}

bool EL_SkinButton::SetForegroundColour(const wxColour& colour)
{
	bool bRet = EL_SkinWindow::SetForegroundColour(colour);
	Refresh();
	return bRet;
}

void EL_SkinButton::SetLabel(const wxString& label)
{
	EL_SkinWindow::SetLabel(label);
	Refresh();
}

void EL_SkinButton::OnLeftClick(wxMouseEvent& WXUNUSED(event))
{	
	if( IsEnabled() && m_bmpPressed.IsOk())
	{	
		wxClientDC dc(this);

		int h, w;
		GetSize(&h,&w);
		wxBitmap temp;
		temp = wxBitmap(m_bmpPressed.Scale(h, w));
		dc.DrawBitmap(temp,0,0);
	}
}

void EL_SkinButton::OnLeftRelease(wxMouseEvent& WXUNUSED(event))
{
	if(IsEnabled())
	{	
		Refresh();
		if(HasCapture())
			ReleaseMouse();	
    	wxCommandEvent ev( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
		ev.SetEventObject(this);
    	GetEventHandler()->ProcessEvent( ev );
	}
}

void EL_SkinButton::DrawCustom(wxDC& dc)
{	
	wxString label = GetLabel();
	if(!label.IsEmpty())
	{	int h, w;
		GetSize(&h,&w);

		dc.SetFont(GetFont());
		
		int x, y;
		dc.GetTextExtent(label,&x,&y);
		x = (h - x ) / 2;
		y = (w - y) / 2;
		
		if ( IsEnabled() )
			dc.SetTextForeground(GetForegroundColour());
		else
			dc.SetTextForeground(wxGREY_PEN->GetColour());

		dc.DrawText(label, x , y );
	}
}
