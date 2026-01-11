#if !defined(AFX_PAGEMOTIONPROPERTY_H__271F65D1_7783_438A_AAD3_09ABC2BFA2A7__INCLUDED_)
#define AFX_PAGEMOTIONPROPERTY_H__271F65D1_7783_438A_AAD3_09ABC2BFA2A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMotionProperty.h : header file
//

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMotionProperty dialog

class CPageMotionProperty : public CPageCtrl, public CManager
{
// Construction
public:
	CPageMotionProperty(CWnd* pParent = NULL);   // standard constructor

	BOOL Create(CWnd * pParent);
	void UpdateUI();

// Dialog Data
	//{{AFX_DATA(CPageMotionProperty)
	enum { IDD = IDD_PAGE_MOTION_PROPERTY };
	CListBox	m_ctrlShapeList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageMotionProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageMotionProperty)
	afx_msg void OnChangeShape();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void OnChangedModel();
	void OnChangedMotion();

protected:
	class CChangedModelObserver : public CManager::IChangedDataObserver
	{
		public:
			void SetOwner(CPageMotionProperty * pOwner) { m_pOwner = pOwner; }
			void OnChangedModel() { m_pOwner->OnChangedModel(); }
			void OnChangedMotion() { m_pOwner->OnChangedMotion(); }
		protected:
			CPageMotionProperty * m_pOwner;
	} m_ChangedModelObserver;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMOTIONPROPERTY_H__271F65D1_7783_438A_AAD3_09ABC2BFA2A7__INCLUDED_)
