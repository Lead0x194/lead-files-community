#if !defined(AFX_FILEOPENER_H__D5190597_42A1_4318_82B8_7BBE22F731FE__INCLUDED_)
#define AFX_FILEOPENER_H__D5190597_42A1_4318_82B8_7BBE22F731FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileOpener.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileOpener dialog

#define WM_TEMP1 (WM_USER+300)
#define WM_TEMP2 (WM_USER+302)

class CFileOpener : public CFileDialog
{
	DECLARE_DYNAMIC(CFileOpener)

public:
	CFileOpener(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

public:
	char * GetFileFullName(){ return m_szFileName; }

protected:
	//{{AFX_MSG(CFileOpener)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEOPENER_H__D5190597_42A1_4318_82B8_7BBE22F731FE__INCLUDED_)
