#pragma once

#include "SkinWindow.h"

class EL_SkinText : public EL_SkinWindow
{
public:
	EL_SkinText();
	EL_SkinText(wxWindow* parent,
 				int id,
				const wxString& label = wxEmptyString,
  				const wxPoint& pos = wxDefaultPosition,
         		const wxSize& size = wxDefaultSize,
           		long style = wxNO_BORDER,
				const wxString& name = wxT("EL_SkinText"));

 	~EL_SkinText();

	///Set the text label
	void			SetLabel(const wxString& label);
		
	virtual void	DrawCustom(wxDC& dc);

	void			CreateLabel();
protected:
	virtual wxSize	DoGetBestSize() const;

private:
	DECLARE_DYNAMIC_CLASS(EL_SkinText);
};
