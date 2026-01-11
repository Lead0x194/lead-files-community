// GraphViewer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SoundArranger.h"
#include "GraphViewer.h"
#include "ResSoundData.h"


const int c_TimeBox_Width = 20;
const int c_TimeBox_Height = 7;
const int c_Point_Width = 5;
const int c_Point_Height = 10;


// CGraphViewer

IMPLEMENT_DYNCREATE(CGraphViewer, CView)

CGraphViewer::CGraphViewer()
{
	m_pdlgSoundData = NULL;

	m_ChangedSoundObserver.SetOwner(this);
	RegisterChangedDataObserver(&m_ChangedSoundObserver);
}

CGraphViewer::~CGraphViewer()
{
	if (m_pdlgSoundData)
	{
		delete m_pdlgSoundData;
		m_pdlgSoundData = NULL;
	}
}

BEGIN_MESSAGE_MAP(CGraphViewer, CView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CGraphViewer::Update()
{
	GetWindowRect(&m_WindowRect);
	m_lHalfHeight = m_WindowRect.Height() / 2;

	/////

	long lxPosition;
	TimeToGraphPosition(GetLocalTime(), &lxPosition);
	m_TimeBox.left = lxPosition-c_TimeBox_Width;
	m_TimeBox.right = lxPosition+c_TimeBox_Width;
	m_TimeBox.top = m_lHalfHeight-c_TimeBox_Height;
	m_TimeBox.bottom = m_lHalfHeight+c_TimeBox_Height;
}

void CGraphViewer::Render()
{
	m_Screen.SetClearColor(0.1f, 0.1f, 0.2f, 1);

	m_Screen.Begin();
	m_Screen.Clear();

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matIdentity);
 	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matIdentity);
	STATEMANAGER.SetTransform(D3DTS_WORLD, &matIdentity);

	m_Screen.SetOrtho2D(1280.0f, 968.0f, 400);
	m_Screen.SetColorOperation();

	// Draw Line
	m_Screen.SetDiffuseColor(1, 1, 1, 1);
	m_Screen.RenderBar2d(0.0f,
						(float) m_WindowRect.Height() / 2 - 1,
						(float) m_WindowRect.Width(),
						(float) m_WindowRect.Height() / 2 + 1);

	static std::vector<float> vecLocalTime;
	if (ms_PlayFlag)
	{
		if (vecLocalTime.empty())
		{
			vecLocalTime.push_back(GetLocalTime());
		}
		else if (vecLocalTime[vecLocalTime.size()-1] != GetLocalTime())
			vecLocalTime.push_back(GetLocalTime());
	}
	else
		vecLocalTime.clear();

	// Draw Debugging Data
//	for (DWORD k = 0; k < vecLocalTime.size(); ++k)
//	{
//		long lxPos;
//		TimeToGraphPosition(vecLocalTime[k], &lxPos);
//
//		m_Screen.SetDiffuseColor(1.0f, 0.0f, 0.0f, 0.8f);
//		m_Screen.RenderBar2d(lxPos-1, m_lHalfHeight-5, lxPos+1, m_lHalfHeight+5);
//	}

	// Draw Frame
	float fLineTimePos = 0.0f;
	for (int i = 0; i < ms_iFrameCount-1; ++i)
	{
		fLineTimePos += 0.033f;

		long lxPos;
		TimeToGraphPosition(fLineTimePos, &lxPos);

		m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
		m_Screen.RenderBar2d(lxPos-1, m_lHalfHeight-3, lxPos+1, m_lHalfHeight+3);
	}

	// Draw Time Box Around
	if (isMotion())
	{
		if (m_isTimeBarDragging || m_isTimeBarOver)
		{
			m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
			m_Screen.RenderBar2d(m_TimeBox.left, m_TimeBox.top, m_TimeBox.right, m_TimeBox.bottom);
		}
		else
		{
			m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			m_Screen.RenderBar2d(m_TimeBox.left, m_TimeBox.top, m_TimeBox.right, m_TimeBox.top+2);
			m_Screen.RenderBar2d(m_TimeBox.left, m_TimeBox.bottom-2, m_TimeBox.right, m_TimeBox.bottom);
			m_Screen.RenderBar2d(m_TimeBox.left, m_TimeBox.top, m_TimeBox.left+2, m_TimeBox.bottom);
			m_Screen.RenderBar2d(m_TimeBox.right-2, m_TimeBox.top, m_TimeBox.right, m_TimeBox.bottom);
		}

		m_Screen.SetDiffuseColor(0.0f, 1.0f, 0.0f, 0.8f);
		m_Screen.RenderBar2d((m_TimeBox.left+m_TimeBox.right)/2-1, 0, (m_TimeBox.left+m_TimeBox.right)/2+1, m_WindowRect.Height());
	}

	// Draw Time Node
	if (isEffect())
	{
		TTimeNodeList::iterator itor = ms_TimeNodeList.begin();
		for (; itor != ms_TimeNodeList.end(); ++itor)
		{
			const TTimeNode & c_rTimeNode = *itor;

			float fcurTime = GetCurrentTime() - c_rTimeNode.fStartTime;

			long lxLine;
			TimeToGraphPosition(fcurTime, &lxLine);

			m_Screen.SetDiffuseColor(0.0f, 1.0f, 0.0f, 0.8f);
			m_Screen.RenderBar2d(lxLine-1, 0, lxLine+1, m_WindowRect.Height());
		}
	}

	// Draw Points
	TCharacterData * pCharacterData;
	if (GetCharacterDataPointer(GetCurrentModelIndex(), &pCharacterData))
	{
		RenderSoundNodes(pCharacterData->SoundDataVector);
	}
	TMotionData * pMotionData;
	if (GetCharacterMotionDataPointer(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pMotionData))
	{
		RenderSoundNodes(pMotionData->SoundDataVector);
	}

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	CRect RenderingRect;
	RenderingRect.left = 0;
	RenderingRect.top = 0;
	RenderingRect.right = m_WindowRect.Width();
	RenderingRect.bottom = m_WindowRect.Height();

	m_Screen.End();
	m_Screen.Show(&RenderingRect, m_hWnd);
}

void CGraphViewer::RenderSoundNodes(const NSound::TSoundDataVector & c_rVector)
{
	for (int i = 0; i < int(c_rVector.size()); ++i)
	{
		const NSound::TSoundData & c_rSoundData = c_rVector[i];

		long lxPosition;
		TimeToGraphPosition(c_rSoundData.fTime, &lxPosition);

		if (i == m_OverPoint || i == m_DraggingPoint)
		{
			m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
			m_Screen.RenderBar2d(lxPosition-c_Point_Width, m_lHalfHeight-c_Point_Height, lxPosition+c_Point_Width, m_lHalfHeight+c_Point_Height);
		}
		else
		{
			m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
			m_Screen.RenderBar2d(lxPosition-c_Point_Width, m_lHalfHeight-c_Point_Height, lxPosition+c_Point_Width, m_lHalfHeight-c_Point_Height+2);
			m_Screen.RenderBar2d(lxPosition-c_Point_Width, m_lHalfHeight+c_Point_Height-2, lxPosition+c_Point_Width, m_lHalfHeight+c_Point_Height);
			m_Screen.RenderBar2d(lxPosition-c_Point_Width, m_lHalfHeight-c_Point_Height, lxPosition-c_Point_Width+2, m_lHalfHeight+c_Point_Height);
			m_Screen.RenderBar2d(lxPosition+c_Point_Width-2, m_lHalfHeight-c_Point_Height, lxPosition+c_Point_Width, m_lHalfHeight+c_Point_Height);
		}

		m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
		m_Screen.RenderBar2d(lxPosition-1, m_lHalfHeight-3, lxPosition+1, m_lHalfHeight+3);
	}
}

void CGraphViewer::TimeToGraphPosition(const float & rTime, long * pPoint)
{
	*pPoint = rTime / GetLimitTime() * m_WindowRect.Width();
}
void CGraphViewer::GraphPositionToTime(const long & pPoint, float * pTime)
{
	*pTime = float(pPoint) / float(m_WindowRect.Width()) * GetLimitTime();
}

bool CGraphViewer::IsClickedTimebar(CPoint & rPoint)
{
	if (rPoint.x >= m_TimeBox.left)
	if (rPoint.x <= m_TimeBox.right)
	if (rPoint.y >= m_TimeBox.top)
	if (rPoint.y <= m_TimeBox.bottom)
	{
		return true;
	}

	return false;
}

bool CGraphViewer::GetClickPoint(CPoint & rPoint, int * pIndex)
{
	*pIndex = -1;

	static long HalfHeight;
	HalfHeight = m_WindowRect.Height()/2;

	NSound::TSoundDataVector * pSoundDataVector;
	if (GetCharacterSoundDataVector(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pSoundDataVector))
	{
		for (DWORD i = 0; i < pSoundDataVector->size(); ++i)
		{
			NSound::TSoundData & rSoundData = pSoundDataVector->at(i);

			static long xPosition;
			TimeToGraphPosition(rSoundData.fTime, &xPosition);

			if (rPoint.x >= xPosition - c_Point_Width)
			if (rPoint.x <= xPosition + c_Point_Width)
			if (rPoint.y >= HalfHeight - c_Point_Height)
			if (rPoint.y <= HalfHeight + c_Point_Height)
			{
				*pIndex = i;
				return true;
			}
		}
	}

	return false;
}


void CGraphViewer::InsertSoundData(int ix)
{
	if (!isMotion() && !isEffect())
	{
		LogBox("먼저 모션 혹은 이펙트를 선택 하십시오", "ERROR", GetSafeHwnd());
		return;
	}

	if (!isSound())
		return;

	Edit();

	float fTime;
	GraphPositionToTime(ix, &fTime);

	DWORD dwFrame = fTime / 0.033f;
	float fFrameTime = dwFrame * 0.033f;

	const char * c_szFileName;
	if (GetSoundFileName(GetCurrentSoundIndex(), &c_szFileName))
	{
		CManager::InsertSoundData(GetCurrentModelIndex(), GetCurrentMotionIndex(), fFrameTime, c_szFileName);
	}

	CManager::SetSoundIndex(0);
}

void CGraphViewer::ChangeSoundData(int iPointIndex)
{
	if (!isSound())
		return;

	TMotionData * pMotionData;
	if (!CManager::GetCharacterMotionDataPointer(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pMotionData))
		return;

	NSound::TSoundData & rSoundData = pMotionData->SoundDataVector[iPointIndex];

	if (DWORD(iPointIndex) >= pMotionData->SoundDataVector.size())
		return;

	const char * c_szFileName;
	if (!GetSoundFileName(GetCurrentSoundIndex(), &c_szFileName))
		return;

	char szAlert[128+1];
	_snprintf(szAlert, 128, "%s를 %s로 바꾸시겠습니까?", rSoundData.strSoundFileName.c_str(), c_szFileName);
	if (6 != MessageBox(szAlert, NULL, MB_YESNOCANCEL))
		return;

	Edit();

	rSoundData.strSoundFileName = c_szFileName;
}

void CGraphViewer::DeleteSoundData(int iPointIndex)
{
	if (-1 == iPointIndex)
		return;

	NSound::TSoundDataVector * pSoundDataVector;
	if (GetCharacterSoundDataVector(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pSoundDataVector))
	{
		char szAlert[128+1];
		_snprintf(szAlert, 128, "정말로 지우시겠습니까?");
		if (6 != MessageBox(szAlert, NULL, MB_YESNO))
			return;

		DeleteVectorItem<NSound::TSoundData>(pSoundDataVector, iPointIndex);
		Edit();

		m_pdlgSoundData->ShowWindow(SW_HIDE);
	}
}

void CGraphViewer::OnChangedSound()
{
	if (0 == GetCurrentSoundIndex())
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
	}
}


// CGraphViewer 그리기입니다.
void CGraphViewer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}


// CGraphViewer 진단입니다.

#ifdef _DEBUG
void CGraphViewer::AssertValid() const
{
	CView::AssertValid();
}

void CGraphViewer::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


//
// CGraphViewer 메시지 처리기입니다.
//

void CGraphViewer::OnPaint()
{
	Render();
	CView::OnPaint();
}

void CGraphViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if (canEdit())
	{
		int iPointIndex;
		// TimeBar를 눌렀다면..
		if (IsClickedTimebar(point))
		{
			m_isTimeBarDragging = TRUE;
		}
		// SoundPoint를 눌렀다면..
		else if (GetClickPoint(point, &iPointIndex))
		{
			if (isSound())
			{
				ChangeSoundData(iPointIndex);
			}
			else
			{
				m_DraggingPoint = iPointIndex;
			}
		}
		// 빈 공간을 눌렀다면..
		else
		{
			if (isSound())
			{
				InsertSoundData(point.x);
			}
			else
			{
				float fTime;

				GraphPositionToTime(point.x, &fTime);

				ms_fLocalTime = fTime;
			}
		}
	}

	CView::OnLButtonDown(nFlags, point);
}
void CGraphViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_DraggingPoint = -1;
	m_isTimeBarDragging = FALSE;

	if (this == GetCapture())
	{
		ReleaseCapture();
	}
	else
	{
		if (canEdit())
		{
			if (isSound())
			{
				InsertSoundData(point.x);
			}
		}
	}

	CView::OnLButtonUp(nFlags, point);
}

void CGraphViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if (canEdit())
	{
		int iPointIndex;
		if (GetClickPoint(point, &iPointIndex))
		{
			DeleteSoundData(iPointIndex);
		}
		else
		{
			CManager::SetSoundIndex(0);
		}
	}

	CView::OnRButtonDown(nFlags, point);
}
void CGraphViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture()==this)
		ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}

void CGraphViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	static float fTime;

	m_xMousePosition = point.x;
	m_isTimeBarOver = FALSE;

	if (m_isTimeBarDragging)
	{
		GraphPositionToTime(point.x, &fTime);

		SetLocalTime(min(GetLimitTime(), max(0.001f, fTime)));
	}
	else if (m_DraggingPoint >= 0)
	{
		GraphPositionToTime(point.x, &fTime);

		NSound::TSoundDataVector * pSoundDataVector;
		if (GetCharacterSoundDataVector(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pSoundDataVector))
		{
			DWORD dwFrame = min(GetLimitTime(), max(0.0f, fTime)) / 0.033f;
			float fFrameTime = dwFrame * 0.033f;
			pSoundDataVector->at(m_DraggingPoint).fTime = fFrameTime;
		}

		Edit();
	}
	else
	{
		if (IsClickedTimebar(point))
		{
			m_isTimeBarOver = TRUE;
			m_OverPoint = -1;
		}
		else if (GetClickPoint(point, &m_OverPoint))
		{
			if (GetCurrentSoundIndex())
			{
				SetCursor(AfxGetApp()->LoadCursor(IDC_CHANGE_SOUND));
			}
		}
		else
		{
			if (GetCurrentSoundIndex())
			{
				SetCursor(AfxGetApp()->LoadCursor(IDC_ADD_SOUND));
			}
		}
	}

	/////

	if (-1 != m_OverPoint || -1 != m_DraggingPoint)
	{
		DWORD dwPointIndex = max(m_OverPoint, m_DraggingPoint);

		NSound::TSoundDataVector * pSoundDataVector;
		if (GetCharacterSoundDataVector(GetCurrentModelIndex(), GetCurrentMotionIndex(), &pSoundDataVector))
		if (dwPointIndex < pSoundDataVector->size())
		{
			const NSound::TSoundData & c_rSoundData = pSoundDataVector->at(dwPointIndex);
			m_pdlgSoundData->UpdateSoundData(c_rSoundData.fTime, c_rSoundData.strSoundFileName.c_str());

			/////

			CRect Rect;
			m_pdlgSoundData->GetWindowRect(&Rect);

			CPoint GlobalPoint(0, (m_WindowRect.Height()/2) - Rect.Height() - 30);
			TimeToGraphPosition(c_rSoundData.fTime, &GlobalPoint.x);
			ClientToScreen(&GlobalPoint);
			GlobalPoint.x -= Rect.Width() / 2;

			m_pdlgSoundData->MoveWindow(GlobalPoint.x, GlobalPoint.y, Rect.Width(), Rect.Height());

			m_pdlgSoundData->ShowWindow(TRUE);
			SetFocus();
		}
	}
	else
	{
		m_pdlgSoundData->ShowWindow(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}

int CGraphViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_hWnd = GetSafeHwnd();

	m_OverPoint = -1;
	m_DraggingPoint = -1;
	m_isTimeBarOver = FALSE;
	m_isTimeBarDragging = FALSE;

	m_pdlgSoundData = new CResSoundData;
	m_pdlgSoundData->Create(this);
	m_pdlgSoundData->ShowWindow(FALSE);

	return 0;
}
