// FileOpener.cpp : implementation file
//

#include "stdafx.h"
#include "FileOpener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileOpener

IMPLEMENT_DYNAMIC(CFileOpener, CFileDialog)

CFileOpener::CFileOpener(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CFileOpener, CFileDialog)
	//{{AFX_MSG_MAP(CFileOpener)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
