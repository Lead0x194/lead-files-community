#if !defined(AFX_DLGMOTIONVIEWMODE_H__5D83AE29_15FE_4DF3_A0B7_9B6BDBDF1282__INCLUDED_)
#define AFX_DLGMOTIONVIEWMODE_H__5D83AE29_15FE_4DF3_A0B7_9B6BDBDF1282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMotionViewMode.h : header file
//

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMotionViewMode dialog

class CDlgMotionViewMode : public CDialog, public CManager
{
// Construction
public:
	CDlgMotionViewMode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMotionViewMode)
	enum { IDD = IDD_DIALOG_MOTION_VIEW_MODE };
	CListBox	m_ctrlReserveMotionList;
	CListBox	m_ctrlBasisMotionList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMotionViewMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMotionViewMode)
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void OnChangedModel();
	void OnChangedReserveMotion();

protected:
	class CChangedDataObserver : public CManager::IChangedDataObserver
	{
		public:
			void SetOwner(CDlgMotionViewMode * pOwner) { m_pOwner = pOwner; }
			void OnChangedModel() { m_pOwner->OnChangedModel(); }
			void OnChangedReserveMotion() { m_pOwner->OnChangedReserveMotion(); }
		protected:
			CDlgMotionViewMode * m_pOwner;
	} m_ChangedDataObserver;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOTIONVIEWMODE_H__5D83AE29_15FE_4DF3_A0B7_9B6BDBDF1282__INCLUDED_)
