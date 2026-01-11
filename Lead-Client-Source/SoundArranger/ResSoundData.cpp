// ResSoundData.cpp : implementation file
//

#include "stdafx.h"
#include "SoundArranger.h"
#include "ResSoundData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResSoundData dialog
CResSoundData::CResSoundData(CWnd* pParent /*=NULL*/) : CDialog(CResSoundData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResSoundData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CResSoundData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResSoundData)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResSoundData, CDialog)
	//{{AFX_MSG_MAP(CResSoundData)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResSoundData Normal Functions

void CResSoundData::ClearSoundData()
{
	SetDlgItemText(IDC_SOUND_DATA_TIME, "");
	SetDlgItemText(IDC_SOUND_DATA_NAME, "");
	SetDlgItemText(IDC_SOUND_DATA_PERCENTAGE, "");
}

void CResSoundData::UpdateSoundData(float fTime, const char * c_szSoundFileName)
{
	char szTime[64];
	sprintf(szTime, "%f", fTime);
	SetDlgItemText(IDC_SOUND_DATA_TIME, szTime);
	SetDlgItemText(IDC_SOUND_DATA_NAME, c_szSoundFileName);
}
/////////////////////////////////////////////////////////////////////////////
// CResSoundData message handlers

BOOL CResSoundData::Create(CWnd * pParent)
{
	if (!CDialog::Create(IDD, pParent))
		return FALSE;

	SetWindowPos(NULL, 0, 0, 0, 0, TRUE);

	return TRUE;
}

void CResSoundData::OnClose() 
{
	return;
}

void CResSoundData::OnOK()
{
	return;
}

void CResSoundData::OnCancel()
{
	return;
}
