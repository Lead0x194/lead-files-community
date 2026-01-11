// SoundArranger.h : main header file for the SOUNDARRANGER application
//

#if !defined(AFX_SOUNDARRANGER_H__F3734219_CC5E_4942_A36C_940BDA48E21D__INCLUDED_)
#define AFX_SOUNDARRANGER_H__F3734219_CC5E_4942_A36C_940BDA48E21D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerApp:
// See SoundArranger.cpp for the implementation of this class
//

class CSoundArrangerApp : public CWinApp
{
public:
	CSoundArrangerApp();
	~CSoundArrangerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundArrangerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSoundArrangerApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDARRANGER_H__F3734219_CC5E_4942_A36C_940BDA48E21D__INCLUDED_)
