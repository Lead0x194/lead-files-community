#if !defined(AFX_CResMotionList_H__5C5858FF_EAD5_49DE_A499_9285854C7ADD__INCLUDED_)
#define AFX_CResMotionList_H__5C5858FF_EAD5_49DE_A499_9285854C7ADD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CResMotionList.h : header file
//

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CResMotionList window

class CResMotionList : public CListCtrl, public CManager
{
// Construction
public:
	CResMotionList();

// Attributes
public:

// Operations
public:
	void OnChangedModel();
	void OnChangedMotion();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResMotionList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResMotionList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResMotionList)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	class CChangedModelObserver : public CManager::IChangedDataObserver
	{
		public:
			void SetOwner(CResMotionList * pOwner) { m_pOwner = pOwner; }
			void OnChangedModel() { m_pOwner->OnChangedModel(); }
			void OnChangedMotion() { m_pOwner->OnChangedMotion(); }
		protected:
			CResMotionList * m_pOwner;
	} m_ChangedModelObserver;
	std::map<std::string, DWORD> m_ItemDataMap;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CResMotionList_H__5C5858FF_EAD5_49DE_A499_9285854C7ADD__INCLUDED_)
