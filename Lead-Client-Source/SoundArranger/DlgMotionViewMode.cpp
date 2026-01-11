// DlgMotionViewMode.cpp : implementation file
//

#include "stdafx.h"
#include "soundarranger.h"
#include "DlgMotionViewMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMotionViewMode dialog


CDlgMotionViewMode::CDlgMotionViewMode(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMotionViewMode::IDD, pParent)
{
	m_ChangedDataObserver.SetOwner(this);
	RegisterChangedDataObserver(&m_ChangedDataObserver);

	//{{AFX_DATA_INIT(CDlgMotionViewMode)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgMotionViewMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMotionViewMode)
	DDX_Control(pDX, IDC_RESERVE_MOTION_LIST, m_ctrlReserveMotionList);
	DDX_Control(pDX, IDC_BASIS_MOTION_LIST, m_ctrlBasisMotionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMotionViewMode, CDialog)
	//{{AFX_MSG_MAP(CDlgMotionViewMode)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMotionViewMode normal functions

void CDlgMotionViewMode::OnChangedModel()
{
	m_ctrlBasisMotionList.ResetContent();

	TCharacterData * pCharacterData;
	if (GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
	{
		for (DWORD i = 0; i < pCharacterData->BasisMotionIDVector.size(); ++i)
		{
			TMotionDataMap::iterator itor = pCharacterData->MotionDataMap.find(pCharacterData->BasisMotionIDVector[i]);
			if (itor != pCharacterData->MotionDataMap.end())
			{
				m_ctrlBasisMotionList.InsertString(m_ctrlBasisMotionList.GetSelCount(), itor->second.strName.c_str());
			}
		}
	}
}

void CDlgMotionViewMode::OnChangedReserveMotion()
{
	m_ctrlReserveMotionList.ResetContent();

	TCharacterData * pCharacterData;
	if (GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
	{
		for (DWORD i = 0; i < ms_ReserveMotionDeque.size(); ++i)
		{
			TMotionDataMap::iterator itor = pCharacterData->MotionDataMap.find(ms_ReserveMotionDeque[i]);
			if (itor != pCharacterData->MotionDataMap.end())
			{
				m_ctrlReserveMotionList.InsertString(m_ctrlReserveMotionList.GetSelCount(), itor->second.strName.c_str());
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgMotionViewMode message handlers

void CDlgMotionViewMode::OnOK()
{
}

void CDlgMotionViewMode::OnCancel()
{
}
