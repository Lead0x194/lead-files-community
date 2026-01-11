#include "metin2TorrentConsole.h"
#include "skinText.h"
#include <wx-2.8.10/dcclient.h>
#include <list>
#include <map>
#include "torrentSkin.h"

IMPLEMENT_DYNAMIC_CLASS(EL_SkinText,EL_SkinWindow)

//Default
EL_SkinText::EL_SkinText()
	: EL_SkinWindow()
{
}
//Passing images
EL_SkinText::EL_SkinText(wxWindow* parent,
 				int id,
 				const wxString& label,
  				const wxPoint& pos,
         		const wxSize& size,
           		long style,
				const wxString& name)
	: EL_SkinWindow(parent,id,pos,size,style,name /*,TYPE_TEXT*/)
{
   	SetLabel(label);
}

EL_SkinText::~EL_SkinText()
{
}

wxSize EL_SkinText::DoGetBestSize() const
{	
	return wxSize(m_bmpBackground.GetWidth(),m_bmpBackground.GetHeight());
}

void EL_SkinText::SetLabel(const wxString& label)
{
	EL_SkinWindow::SetLabel(label);
	CreateLabel();
	SetSize(m_bmpBackground.GetWidth(),m_bmpBackground.GetHeight());
	Refresh();
}

void EL_SkinText::CreateLabel()
{
	int nWidth, nHeight;
	wxMemoryDC cDC;
	wxString strLabel = GetLabel();
	cDC.SetFont( GetFont() );
	cDC.GetTextExtent( strLabel, &nWidth, &nHeight );

	StoreBackgroundBitmap( -1, -1, nWidth, nHeight );

	cDC.SelectObject(GetBackgroundImage());
	cDC.SetTextForeground(GetForegroundColour());
	cDC.DrawText(strLabel,0,0);
	cDC.SelectObject(wxNullBitmap);
}

void EL_SkinText::DrawCustom(wxDC& WXUNUSED(dc))
{
}
