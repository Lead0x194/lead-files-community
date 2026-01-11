#include "StdAfx.h"
#include "MainFrm.h"

void CMainFrame::OnToolbarReverse()
{
	ms_fLocalTime = 0.001f;
	ms_dwLocalFrame = 0;
}

void CMainFrame::OnToolbarPlay()
{
	PlayMotion();
}

void CMainFrame::OnToolbarPlayloop()
{
	PlayMotion(TRUE);
}

void CMainFrame::OnToolbarStop()
{
	OnToolbarReverse();
	ms_PlayFlag = false;
}

void CMainFrame::OnToolbarPause()
{
	ms_PlayFlag = !ms_PlayFlag;
}

void CMainFrame::OnKeyboardPlay() 
{
	OnToolbarPlay();
}

//void CMainFrame::OnUpdateWorkLoop(CCmdUI *pCmdUI)
//{
//	if (ms_LoopFlag)
//		pCmdUI->SetCheck(1);
//	else
//		pCmdUI->SetCheck(0);
//}

void CMainFrame::OnCancelSound() 
{
	SetSoundIndex(0);
}
