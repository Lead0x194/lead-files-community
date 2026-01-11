// SoundArrangerView.h : interface of the CSoundArrangerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDARRANGERVIEW_H__273D8B8A_C7B0_4959_A8E5_394CE1151263__INCLUDED_)
#define AFX_SOUNDARRANGERVIEW_H__273D8B8A_C7B0_4959_A8E5_394CE1151263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSoundArrangerView : public CView
{
protected: // create from serialization only
	CSoundArrangerView();
	DECLARE_DYNCREATE(CSoundArrangerView)

// Attributes
public:
	CSoundArrangerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundArrangerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoundArrangerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSoundArrangerView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SoundArrangerView.cpp
inline CSoundArrangerDoc* CSoundArrangerView::GetDocument()
   { return (CSoundArrangerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDARRANGERVIEW_H__273D8B8A_C7B0_4959_A8E5_394CE1151263__INCLUDED_)
