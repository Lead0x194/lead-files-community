#include "StdAfx.h"
#include "ModelViewer.h"

#include <EffectLib/EffectManager.h>
#include <GameLib/FlyingObjectManager.h>

float g_frClearColor = 0.1f;
float g_fgClearColor = 0.1f;
float g_fbClearColor = 0.2f;

/////////////////////////////////////////////////////////////////////////////
// CModelViewer

IMPLEMENT_DYNCREATE(CModelViewer, CView)

CModelViewer::CModelViewer()
{
	m_bMButtonDown = false;
	m_hWnd = NULL;
	m_bVisible = TRUE;

	m_fDistance = 1500.0f;
	m_fPitch = 30.0f;
	m_fRotation = 45.0f;

	m_xMousePosition = 0;
	m_yMousePosition = 0;
}

CModelViewer::~CModelViewer()
{
}

BEGIN_MESSAGE_MAP(CModelViewer, CView)
	//{{AFX_MSG_MAP(CModelViewer)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelViewer drawing

void CModelViewer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CModelViewer diagnostics

#ifdef _DEBUG
void CModelViewer::AssertValid() const
{
	CView::AssertValid();
}

void CModelViewer::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelViewer message handlers

int CModelViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hWnd = GetSafeHwnd();	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CModelViewer::Render()
{
	CRect WindowRect;
	GetWindowRect(&WindowRect);

	m_Screen.SetClearColor(g_frClearColor, g_fgClearColor, g_fbClearColor, 1.0f);

	m_Screen.Begin();
	m_Screen.Clear();

	m_Screen.SetPerspective(20.0f, float(WindowRect.Width())/float(WindowRect.Height()), 100.0f, 15000.0f);
	m_Screen.SetPositionCamera(0.0f, 0.0f, 100.0f, m_fDistance, m_fPitch, m_fRotation);

	STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_Screen.SetDiffuseOperation();
	m_Screen.SetDiffuseColor(1.0f, 1.0f, 1.0f);

	// Render Model
	if (isModel() && isMotion())
	{
		m_Screen.SetDiffuseOperation();
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity(&matIdentity);
		ms_ActorInstance.__SetLocalTime(GetLocalTime());
		ms_ActorInstance.Update();
		ms_ActorInstance.Deform();
		ms_ActorInstance.UpdateAttachingInstances();

		if (m_bVisible)
			ms_ActorInstance.Render();

		if (ms_PlayFlag)
			ms_ActorInstance.UpdateFrame();
	}
	// Render Model

	// Render Effect
	TEffectInstanceList::iterator itor = ms_EffectInstanceList.begin();
	for (; itor != ms_EffectInstanceList.end();)
	{
		CEffectInstance * pInstance = *itor;

		pInstance->Update();
		pInstance->Render();

		if (!pInstance->isAlive())
		{
			pInstance->Clear();
			ms_EffectInstancePool.Free(pInstance);
			itor = ms_EffectInstanceList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
	// Render Effect
	CEffectManager::Instance().Update();
	CEffectManager::Instance().Render();
	CFlyingManager::Instance().Update();
	CFlyingManager::Instance().Render();

	m_Screen.End();
	m_Screen.Show(m_hWnd);
}

void CModelViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bMButtonDown)
	{
		m_fRotation = m_fRotation - float(m_xMousePosition - point.x) * 0.3f;
		m_fPitch	= fMINMAX(-90.0f, m_fPitch - float(m_yMousePosition - point.y) * 0.3f, 90.0f);

		m_xMousePosition = point.x;
		m_yMousePosition = point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

void CModelViewer::OnMButtonDown(UINT nFlags, CPoint point) 
{
	m_bMButtonDown = true;
	m_xMousePosition = point.x;
	m_yMousePosition = point.y;
	CView::OnMButtonDown(nFlags, point);
	SetCapture();
}

void CModelViewer::OnMButtonUp(UINT nFlags, CPoint point) 
{
	m_bMButtonDown = false;
	CView::OnMButtonUp(nFlags, point);
	ReleaseCapture();
}

BOOL CModelViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	m_fDistance = fMINMAX(500.0f, m_fDistance - (float) zDelta, 10000.0f);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CModelViewer::ToggleVisable()
{
	m_bVisible = 1 - m_bVisible;
}
