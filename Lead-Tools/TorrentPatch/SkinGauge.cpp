#include "Metin2TorrentConsole.h"
#include "SkinGauge.h"
#include "TorrentSkin.h"

BEGIN_EVENT_TABLE(EL_SkinGauge,EL_SkinWindow)
//	EVT_PAINT(EL_SkinGauge::OnPaint)
	EVT_IDLE(EL_SkinGauge::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EL_SkinGauge,EL_SkinWindow)

EL_SkinGauge::EL_SkinGauge()
	: EL_SkinWindow()
{
	m_nCurrentValue	= 0;
	m_nMaxValue		= 0;
	m_bIsVertical	= false;
	m_nPrevValue	= 0;
	m_fPrevTime		= 0.0f;
}

EL_SkinGauge::EL_SkinGauge(wxWindow* parent,
 				int id,
 				int max,
   				const wxPoint& pos,
         		const wxSize& size,
           		long style,
				const wxString& name)
	:EL_SkinWindow(parent,id,pos,size,style,name/*,TYPE_GAUGE*/)
{
	m_nMaxValue		= max;
	m_nCurrentValue	= 0;
	m_bIsVertical	= false;
	m_nPrevValue	= 0;
	m_fPrevTime		= 0.0f;
}

EL_SkinGauge::~EL_SkinGauge()
{
}

void EL_SkinGauge::SetGaugeImage(const wxImage& image)
{
	m_bmpGauge = image;
}

wxSize EL_SkinGauge::DoGetBestSize() const
{
	if(m_bmpBackground.IsOk())
		return wxSize(m_bmpBackground.GetWidth(),m_bmpBackground.GetHeight());
	return wxSize(70,30);
}

void EL_SkinGauge::DrawCustom(wxDC& dc)
{
	if(m_bmpGauge.IsOk())
	{
		int nMaxPos = GetRange();
		int nCurPos = GetValue();

		const bool bSmooth = (0 != (GetWindowStyle() & wxGA_SMOOTH));
		if( bSmooth )
			nCurPos = m_nPrevValue;

		wxSize sz = GetClientSize();
		dc.SetClippingRegion(0, 0, sz.x * nCurPos / nMaxPos, sz.y);
		dc.DrawBitmap(m_bmpGauge, 0, 0, false);
		dc.DestroyClippingRegion();
	}
}

void EL_SkinGauge::OnIdle( wxIdleEvent& /*cEvent*/ )
{
	const bool bSmooth = (0 != (GetWindowStyle() & wxGA_SMOOTH));
	if( bSmooth ) {
		const DWORD REFRESH_FREQ = 100;

		int nCurrentPos = GetValue();
		if( nCurrentPos< 0 )
			nCurrentPos = 0;
		const float fCurrentTime = ::GetTickCount() / 1000.0f;
		const float fDeltaTime = (fCurrentTime - m_fPrevTime) * 2.0f;

		m_nPrevValue += (nCurrentPos - m_nPrevValue) * EL_MINMAX(0.0f, fDeltaTime, 1.0f);
		if( (REFRESH_FREQ / 1000.0f) < fDeltaTime ) {
			m_fPrevTime = fCurrentTime;
			Refresh(true);
		} else {
			Refresh(false);
		}
	}
	Sleep( 1 );
}

int EL_SkinGauge::GetRange() const
{
	return m_nMaxValue;
}

int EL_SkinGauge::GetValue() const
{
	return m_nCurrentValue;
}

void EL_SkinGauge::SetRange(int nRange)
{
	m_nMaxValue = nRange;
}

void EL_SkinGauge::SetValue(int nValue)
{
	if( nValue < 0 )
		nValue = 0;
	if( nValue < m_nPrevValue ) {
		m_nPrevValue	= nValue;
		m_fPrevTime		= 0.0f;
	}
	if( nValue == m_nMaxValue ) {
		m_nPrevValue	= nValue;
		m_fPrevTime		= 0.0f;
	}
	m_nCurrentValue = nValue;
	Refresh();
}

