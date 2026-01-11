// SoundArrangerView.cpp : implementation of the CSoundArrangerView class
//

#include "stdafx.h"
#include "SoundArranger.h"

#include "SoundArrangerDoc.h"
#include "SoundArrangerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView

IMPLEMENT_DYNCREATE(CSoundArrangerView, CView)

BEGIN_MESSAGE_MAP(CSoundArrangerView, CView)
	//{{AFX_MSG_MAP(CSoundArrangerView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView construction/destruction

CSoundArrangerView::CSoundArrangerView()
{
}

CSoundArrangerView::~CSoundArrangerView()
{
}

BOOL CSoundArrangerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView drawing

void CSoundArrangerView::OnDraw(CDC* pDC)
{
	CSoundArrangerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView printing

BOOL CSoundArrangerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSoundArrangerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CSoundArrangerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView diagnostics

#ifdef _DEBUG
void CSoundArrangerView::AssertValid() const
{
	CView::AssertValid();
}

void CSoundArrangerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSoundArrangerDoc* CSoundArrangerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundArrangerDoc)));
	return (CSoundArrangerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerView message handlers
