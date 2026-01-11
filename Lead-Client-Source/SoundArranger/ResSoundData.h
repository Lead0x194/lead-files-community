#if !defined(AFX_RESSOUNDDATA_H__105B318D_7F9D_4834_AA40_644F8AB1A655__INCLUDED_)
#define AFX_RESSOUNDDATA_H__105B318D_7F9D_4834_AA40_644F8AB1A655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResSoundData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResSoundData dialog

class CResSoundData : public CDialog
{
// Construction
public:
	CResSoundData(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd * pParent);

// Dialog Data
	//{{AFX_DATA(CResSoundData)
	enum { IDD = IDD_SOUND_DATA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResSoundData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	virtual void OnCancel();
	// Generated message map functions
	//{{AFX_MSG(CResSoundData)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ClearSoundData();
	void UpdateSoundData(float fTime, const char * c_szSoundFileName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESSOUNDDATA_H__105B318D_7F9D_4834_AA40_644F8AB1A655__INCLUDED_)
