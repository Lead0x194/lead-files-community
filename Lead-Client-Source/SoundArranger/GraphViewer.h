#pragma once

#include <EterLib/GrpTextInstance.h>
#include "Manager.h"

class CResSoundData;

// CGraphViewer 뷰입니다.
class CGraphViewer : public CView, public CManager
{
	DECLARE_DYNCREATE(CGraphViewer)

protected:
	CGraphViewer();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CGraphViewer();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void Update();
	void Render();

	void TimeToGraphPosition(const float & rTime, long * pPoint);
	void GraphPositionToTime(const long & pPoint, float * pTime);

	void OnChangedSound();

protected:
	void RenderSoundNodes(const NSound::TSoundDataVector & c_rVector);

	bool IsClickedTimebar(CPoint & rPoint);
	bool GetClickPoint(CPoint & rPoint, int * pIndex);
	void InsertSoundData(int ix);
	void ChangeSoundData(int iPointIndex);
	void DeleteSoundData(int iPointIndex);

protected:
	HWND m_hWnd;
	CRect m_WindowRect;

	CScreen m_Screen;
	CGraphicTextInstance * pTextInstance;

	int m_xMousePosition;
	long m_lHalfHeight;

	CRect m_TimeBox;
	BOOL m_isTimeBarOver;
	BOOL m_isTimeBarDragging;

	int m_OverPoint;
	int m_DraggingPoint;

	CResSoundData * m_pdlgSoundData;

	class CChangedSoundObserver : public CManager::IChangedDataObserver
	{
		public:
			void SetOwner(CGraphViewer * pOwner) { m_pOwner = pOwner; }
			void OnChangedSound() { m_pOwner->OnChangedSound(); }
		protected:
			CGraphViewer * m_pOwner;
	} m_ChangedSoundObserver;
};