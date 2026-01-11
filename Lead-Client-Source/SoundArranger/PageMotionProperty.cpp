// PageMotionProperty.cpp : implementation file
//

#include "stdafx.h"
#include "soundarranger.h"
#include "PageMotionProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageMotionProperty dialog


CPageMotionProperty::CPageMotionProperty(CWnd* pParent /*=NULL*/)
	: CPageCtrl(CPageMotionProperty::IDD, pParent)
{
	m_ChangedModelObserver.SetOwner(this);
	RegisterChangedDataObserver(&m_ChangedModelObserver);
	//{{AFX_DATA_INIT(CPageMotionProperty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPageMotionProperty::DoDataExchange(CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMotionProperty)
	DDX_Control(pDX, IDC_MOTION_PROPERTY_SHAPE_LIST, m_ctrlShapeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMotionProperty, CPageCtrl)
	//{{AFX_MSG_MAP(CPageMotionProperty)
	ON_LBN_SELCHANGE(IDC_MOTION_PROPERTY_SHAPE_LIST, OnChangeShape)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMotionProperty normal functions

BOOL CPageMotionProperty::Create(CWnd * pParent)
{
	if (!CPageCtrl::Create(CPageMotionProperty::IDD, pParent))
		return FALSE;

	ShowWindow(SW_SHOW);
	return TRUE;
}

void CPageMotionProperty::UpdateUI()
{
}

void CPageMotionProperty::OnChangedModel()
{
	// Change Name
	TCharacterData * pCharacterData;
	if (GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
	{
		SetDlgItemText(IDC_MOTION_PROPERTY_MODEL_NAME, pCharacterData->strName.c_str());
	}
	SetDlgItemText(IDC_MOTION_PROPERTY_MOTION_NAME, "None");
	SetDlgItemText(IDC_MOTION_PROPERTY_DESCRIPTION, "");
	SetDlgItemText(IDC_MOTION_PROPERTY_DESCRIPTION, "0.0");

	// Update ShapeList
	m_ctrlShapeList.ResetContent();
	m_ctrlShapeList.InsertString(0, "<Default>");
	m_ctrlShapeList.SelectString(-1, "<Default>");

	int iItemNumber = 1;

	if (ms_RaceData.HasShape())
	{
		CRaceDataAccessor::TShapeDictIterator itor = ms_RaceData.GetShapeDictIterator();
		do {
			int iShapeNumber = ms_RaceData.GetShapeNumber(itor);
			CFilename strModelFileName = ms_RaceData.GetShapeModelFileName(itor);
			m_ctrlShapeList.InsertString(iItemNumber, strModelFileName.GetName().c_str());
			m_ctrlShapeList.SetItemData(iItemNumber, iShapeNumber);
			++iItemNumber;

			if (ms_dwShapeIndex == DWORD(iShapeNumber))
			{
				m_ctrlShapeList.SelectString(-1, strModelFileName.GetName().c_str());
			}
		} while (ms_RaceData.GetNextIterator(itor));
	}
}

void CPageMotionProperty::OnChangedMotion()
{
	SetDlgItemText(IDC_MOTION_PROPERTY_DESCRIPTION, "");

	TCharacterData * pCharacterData;
	if (!GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
		return;

	TMotionDataMap::iterator itor = pCharacterData->MotionDataMap.find(GetCurrentMotionIndex());
	if (pCharacterData->MotionDataMap.end() == itor)
		return;

	SetDlgItemText(IDC_MOTION_PROPERTY_MOTION_NAME, itor->second.strName.c_str());
	SetDialogFloatText(GetSafeHwnd(), IDC_MOTION_PROPERTY_MOTION_TIME, ms_ActorInstance.GetMotionDuration(0));
}

/////////////////////////////////////////////////////////////////////////////
// CPageMotionProperty message handlers

void CPageMotionProperty::OnChangeShape() 
{
	int iCurSel = m_ctrlShapeList.GetCurSel();
	ms_dwShapeIndex = DWORD(m_ctrlShapeList.GetItemData(iCurSel));

	Refresh();
}
