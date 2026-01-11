// ResModelList.cpp : implementation file
//

#include "stdafx.h"
#include "SoundArranger.h"
#include "ResModelList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResModelList

CResModelList::CResModelList()
{
}

CResModelList::~CResModelList()
{
}


BEGIN_MESSAGE_MAP(CResModelList, CTreeCtrl)
	//{{AFX_MSG_MAP(CResModelList)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResModelList message handlers

void CResModelList::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	SetModelIndex(GetItemData(hTreeItem));

	*pResult = 0;
}

void CResModelList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PlayMotion();

	/////

	*pResult = 0;
}
