// CResMotionList.cpp : implementation file
//

#include "stdafx.h"
#include "SoundArranger.h"
#include "ResMotionList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResMotionList

CResMotionList::CResMotionList()
{
	m_ChangedModelObserver.SetOwner(this);
	RegisterChangedDataObserver(&m_ChangedModelObserver);
}

CResMotionList::~CResMotionList()
{
}


BEGIN_MESSAGE_MAP(CResMotionList, CListCtrl)
	//{{AFX_MSG_MAP(CResMotionList)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResMotionList normal functions

void CResMotionList::OnChangedModel()
{
	TCharacterData * pCharacterData;
	if (!GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
		return;

	DeleteAllItems();
	m_ItemDataMap.clear();

	DWORD dwCount = 0;
	TMotionDataMap::iterator itor = pCharacterData->MotionDataMap.begin();
	for (; itor != pCharacterData->MotionDataMap.end(); ++itor, ++dwCount)
	{
		DWORD dwCRC32 = itor->first;
		TMotionData & rMotionData = itor->second;

		DWORD dwSelectedIndex = InsertItem(dwCount, _T(rMotionData.strName.c_str()));
		std::string strName = GetItemText(dwSelectedIndex, 0);

		// ERROR Check
		if (m_ItemDataMap.find(strName) != m_ItemDataMap.end())
		{
			Tracef(" ERROR!! CResMotionList::OnChangedModel - There is already same motion name [%s]\n", strName.c_str());
		}
		// ERROR Check

		if (GetCurrentMotionIndex() == dwCRC32)
		{
			CListCtrl::SetItemState(dwSelectedIndex, LVIS_SELECTED, LVIS_SELECTED);
		}

		m_ItemDataMap.insert(std::map<std::string, DWORD>::value_type(strName, dwCRC32));
	}

	if (0 == CListCtrl::GetSelectedCount())
	if (CListCtrl::GetItemCount())
	{
		CListCtrl::SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		std::string strItemName = CListCtrl::GetItemText(0, 0);
		SetMotionIndex(m_ItemDataMap[strItemName]);
	}
}

void CResMotionList::OnChangedMotion()
{
	// 기존에 선택 되어있던 모션과 현재의 모션이 다르면 다시 선택한다.
	TCharacterData * pCharacterData;
	if (!GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
		return;

	TMotionDataMap::iterator itorMotion = pCharacterData->MotionDataMap.find(GetCurrentMotionIndex());
	if (pCharacterData->MotionDataMap.end() != itorMotion)
	{
		TMotionData & rMotionData = itorMotion->second;

		for (int i = 0; i < CListCtrl::GetItemCount(); ++i)
		{
			CString strItemName = CListCtrl::GetItemText(i, 0);
			if (0 == rMotionData.strName.compare(strItemName))
			{
				CListCtrl::SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CResMotionList message handlers

void CResMotionList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PlayMotion();

	/////

	*pResult = 0;
}

// NOTE : OnItemChanged로 ChangeSoundIndex를 하지 않는다. 상위에서 바뀐 정보를 참조 하여
//        Sel을 바꾸는 경우도 있기 때문이다. 만약OnItemChanged로 처리 하게 되면 여러번 (혹은 무한)
//        셋팅 되는 경우가 생길 수 있다. - [levites]
// FIXME : 키로 셀을 바꾸는 경우에 적용 되지 않는다. - [levites]
void CResMotionList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iIndex = HitTest(point, NULL);

	if (-1 != iIndex)
	{
		std::string strName = GetItemText(iIndex, 0);
		SetMotionIndex(m_ItemDataMap[strName]);
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}
