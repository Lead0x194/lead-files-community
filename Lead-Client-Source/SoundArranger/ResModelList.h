#if !defined(AFX_RESMODELLIST_H__E4DE4162_35C5_40D3_8943_CD98E7A02BCC__INCLUDED_)
#define AFX_RESMODELLIST_H__E4DE4162_35C5_40D3_8943_CD98E7A02BCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResModelList.h : header file
//

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CResModelList window

class CResModelList : public CTreeCtrl, public CManager
{
// Construction
public:
	CResModelList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResModelList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResModelList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResModelList)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESMODELLIST_H__E4DE4162_35C5_40D3_8943_CD98E7A02BCC__INCLUDED_)
