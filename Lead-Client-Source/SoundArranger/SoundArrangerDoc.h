// SoundArrangerDoc.h : interface of the CSoundArrangerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDARRANGERDOC_H__0A47B197_2A73_48C8_BE08_B7B3FB7ABA96__INCLUDED_)
#define AFX_SOUNDARRANGERDOC_H__0A47B197_2A73_48C8_BE08_B7B3FB7ABA96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSoundArrangerDoc : public CDocument
{
protected: // create from serialization only
	CSoundArrangerDoc();
	DECLARE_DYNCREATE(CSoundArrangerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundArrangerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoundArrangerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSoundArrangerDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDARRANGERDOC_H__0A47B197_2A73_48C8_BE08_B7B3FB7ABA96__INCLUDED_)
