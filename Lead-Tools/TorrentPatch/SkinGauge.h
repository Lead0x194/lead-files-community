#pragma once

#include "SkinWindow.h"

class EL_SkinGauge : public EL_SkinWindow
{
public:
	EL_SkinGauge();

	EL_SkinGauge(wxWindow* parent,
 				int id,
 				int max=100,
  				const wxPoint& pos = wxDefaultPosition,
         		const wxSize& size = wxDefaultSize,
           		long style = wxNO_BORDER,
				const wxString& name = wxT("EL_SkinGauge"));

	~EL_SkinGauge();

	void SetGaugeImage(const wxImage& image = wxNullImage);
	wxImage& GetGaugeImage( void )					{ return m_bmpGauge; }

 	wxSize DoGetBestSize() const;
	int GetRange() const;
	int GetValue() const;
	void SetRange(int);
	void SetValue(int);

	void	OnIdle(wxIdleEvent& evt);
	virtual void DrawCustom(wxDC& dc);

private:
	wxImage m_bmpGauge;
	bool	m_bIsVertical;
	int		m_nMaxValue;
	int		m_nCurrentValue;

	int		m_nPrevValue;
	float	m_fPrevTime;

private:
	DECLARE_DYNAMIC_CLASS(EL_SkinGauge)
	DECLARE_EVENT_TABLE()
};
