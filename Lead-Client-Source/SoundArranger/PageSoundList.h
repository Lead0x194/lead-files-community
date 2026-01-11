#if !defined(AFX_PAGESOUNDLIST_H__9DDBD3CE_2B4B_4B02_9446_2A0258A49340__INCLUDED_)
#define AFX_PAGESOUNDLIST_H__9DDBD3CE_2B4B_4B02_9446_2A0258A49340__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSoundList.h : header file
//

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CPageSoundList dialog

class CPageSoundList : public CPageCtrl, public CManager
{
// Construction
public:
	CPageSoundList(CWnd* pParent = NULL);   // standard constructor

	BOOL Create(CWnd * pParent);
	void UpdateUI();

	void OnChangedSound();

// Dialog Data
	//{{AFX_DATA(CPageSoundList)
	enum { IDD = IDD_PAGE_SOUND_LIST };
	CTreeCtrl	m_ctrlSoundList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageSoundList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageSoundList)
	afx_msg void OnSelchangedSoundList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSoundPlay();
	afx_msg void OnDblclkSoundList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragSoundList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSoundRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	class CChangedDataObserver : public CManager::IChangedDataObserver
	{
		public:
			void SetOwner(CPageSoundList * pOwner) { m_pOwner = pOwner; }
			void OnChangedSound() { m_pOwner->OnChangedSound(); }
		protected:
			CPageSoundList * m_pOwner;
	} m_ChangedSoundObserver;
	CImageList m_TreeItemImage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESOUNDLIST_H__9DDBD3CE_2B4B_4B02_9446_2A0258A49340__INCLUDED_)
