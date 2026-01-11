// SoundArrangerDoc.cpp : implementation of the CSoundArrangerDoc class
//

#include "stdafx.h"
#include "SoundArranger.h"

#include "SoundArrangerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerDoc

IMPLEMENT_DYNCREATE(CSoundArrangerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSoundArrangerDoc, CDocument)
	//{{AFX_MSG_MAP(CSoundArrangerDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerDoc construction/destruction

CSoundArrangerDoc::CSoundArrangerDoc()
{
}

CSoundArrangerDoc::~CSoundArrangerDoc()
{
}

BOOL CSoundArrangerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerDoc serialization

void CSoundArrangerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerDoc diagnostics

#ifdef _DEBUG
void CSoundArrangerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoundArrangerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerDoc commands
