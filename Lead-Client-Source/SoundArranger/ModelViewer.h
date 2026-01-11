#pragma once

#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// CModelViewer view

class CModelViewer : public CView, public CManager
{
protected:
	CModelViewer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CModelViewer)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelViewer)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CModelViewer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CModelViewer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void		Render();
	void		ToggleVisable();

protected:
	HWND		m_hWnd;
	bool		m_bMButtonDown;
	BOOL		m_bVisible;
	CScreen		m_Screen;

	float		m_fDistance;
	float		m_fPitch;
	float		m_fRotation;

	LONG		m_xMousePosition;
	LONG		m_yMousePosition;
};

/////////////////////////////////////////////////////////////////////////////
