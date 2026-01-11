// PageSoundList.cpp : implementation file
//

#include "stdafx.h"
#include "SoundArranger.h"
#include "PageSoundList.h"
#include "Util.h"
#include "SoundDataLoader.h"
#include <MilesLib/SoundManager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageSoundList dialog


CPageSoundList::CPageSoundList(CWnd* pParent /*=NULL*/)
	: CPageCtrl(CPageSoundList::IDD, pParent)
{
	m_ChangedSoundObserver.SetOwner(this);
	RegisterChangedDataObserver(&m_ChangedSoundObserver);
	//{{AFX_DATA_INIT(CPageSoundList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPageSoundList::DoDataExchange(CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSoundList)
	DDX_Control(pDX, IDC_SOUND_LIST, m_ctrlSoundList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSoundList, CPageCtrl)
	//{{AFX_MSG_MAP(CPageSoundList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SOUND_LIST, OnSelchangedSoundList)
	ON_BN_CLICKED(IDC_SOUND_PLAY, OnSoundPlay)
	ON_NOTIFY(NM_DBLCLK, IDC_SOUND_LIST, OnDblclkSoundList)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_SOUND_LIST, OnBegindragSoundList)
	ON_BN_CLICKED(IDC_SOUND_REFRESH, OnSoundRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSoundList normal functions

BOOL CPageSoundList::Create(CWnd * pParent)
{
	if (!CPageCtrl::Create(IDD, pParent))
		return FALSE;

	CreateHighColorImageList(IDB_SOUND_TREE_ITEM, &m_TreeItemImage);
	m_ctrlSoundList.SetImageList(&m_TreeItemImage, TVSIL_NORMAL);

	CSoundDataLoader SoundDataLoader;
	SoundDataLoader.SetTreeControler(&m_ctrlSoundList);
	SoundDataLoader.Create("wav;mp3", "sound", TRUE);

	ShowWindow(SW_SHOW);
	return TRUE;
}

void CPageSoundList::UpdateUI()
{
}

void CPageSoundList::OnChangedSound()
{
	if (0 == GetCurrentSoundIndex())
	{
		m_ctrlSoundList.SelectItem(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPageSoundList message handlers

void CPageSoundList::OnSelchangedSoundList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;

	if (hTreeItem)
	{
		DWORD dwIndex = m_ctrlSoundList.GetItemData(hTreeItem);
		SetSoundIndex(dwIndex);
	}

	*pResult = 0;
}

void CPageSoundList::OnSoundPlay() 
{
	const char * c_szFileName;
	if (!GetSoundFileName(GetCurrentSoundIndex(), &c_szFileName))
		return;

	CSoundManager::Instance().PlaySound2D(c_szFileName);
}

void CPageSoundList::OnDblclkSoundList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hTreeItem = m_ctrlSoundList.GetSelectedItem();

	if (!hTreeItem)
		return;

	OnSoundPlay();
	*pResult = 0;
}

void CPageSoundList::OnBegindragSoundList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	m_ctrlSoundList.SelectItem(pNMTreeView->itemNew.hItem);
	*pResult = 0;
}

void CPageSoundList::OnSoundRefresh() 
{
	m_ctrlSoundList.DeleteAllItems();
	ClearAllSoundFileName();

	CSoundDataLoader SoundDataLoader2;
	SoundDataLoader2.SetTreeControler(&m_ctrlSoundList);
	SoundDataLoader2.Create("wav;mp3", "sound", TRUE);
}
