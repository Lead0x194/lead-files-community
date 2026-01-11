// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoundArranger.h"

#include "MainFrm.h"
#include "Util.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

static UINT g_auiStatusIndicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

#include <EterLib/Camera.h>
#include <GameLib/Area.h>

float CCamera::CAMERA_MAX_DISTANCE = 2500.0f;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_SAVE_SOUND_DATA, OnAppSaveSoundData)
	ON_COMMAND(ID_TOOLBAR_PLAY, OnToolbarPlay)
	ON_COMMAND(ID_TOOLBAR_PLAYLOOP, OnToolbarPlayloop)
	ON_COMMAND(ID_TOOLBAR_REVERSE, OnToolbarReverse)
	ON_COMMAND(ID_TOOLBAR_PAUSE, OnToolbarPause)
	ON_COMMAND(ID_TOOLBAR_STOP, OnToolbarStop)
	ON_COMMAND(ID_KEYBOARD_PLAY, OnKeyboardPlay)
	ON_COMMAND(ID_CANCEL_SOUND, OnCancelSound)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODEL_LIST, OnUpdateViewModelList)
	ON_COMMAND(ID_VIEW_MODEL_LIST, OnViewModelList)
	ON_COMMAND(ID_VIEW_MOTION_LIST, OnViewMotionList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOTION_LIST, OnUpdateViewMotionList)
	ON_COMMAND(ID_VIEW_MOTION_PROPERTY, OnViewMotionProperty)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOTION_PROPERTY, OnUpdateViewMotionProperty)
	ON_COMMAND(ID_VIEW_SOUND_LIST, OnViewSoundList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOUND_LIST, OnUpdateViewSoundList)
	ON_COMMAND(ID_WORK_MODE_EDIT, OnWorkModeEdit)
	ON_UPDATE_COMMAND_UI(ID_WORK_MODE_EDIT, OnUpdateWorkModeEdit)
	ON_COMMAND(ID_WORK_MODE_VIEW, OnWorkModeView)
	ON_UPDATE_COMMAND_UI(ID_WORK_MODE_VIEW, OnUpdateWorkModeView)
	ON_COMMAND(ID_WORK_SCREEN_SHOT_CAPTURE, OnCaptureScreen)
	ON_COMMAND(ID_WORK_RELOAD_ALL_RESOURCE, OnReloadAllResource)
	ON_WM_TIMER()
	ON_COMMAND(ID_CHAR_VISIBLE_TOGGLE, OnCharVisibleToggle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static const RGBTRIPLE	kBackgroundColor = {192, 192, 192};

static void	ReplaceBackgroundColor (CBitmap& ioBM)
{
	BITMAP		bmInfo;

	VERIFY (ioBM.GetBitmap (&bmInfo));
	
	VERIFY (bmInfo.bmBitsPixel == 24);
//	VERIFY (bmInfo.bmWidthBytes == (bmInfo.bmWidth * 3));

	const UINT		numPixels (bmInfo.bmHeight * bmInfo.bmWidth);

    DIBSECTION  ds;

    VERIFY (ioBM.GetObject (sizeof (DIBSECTION), &ds) == sizeof (DIBSECTION));

	RGBTRIPLE*		pixels = reinterpret_cast<RGBTRIPLE*>(ds.dsBm.bmBits);
	VERIFY (pixels != NULL);

	const COLORREF		buttonColor (::GetSysColor (COLOR_BTNFACE));
	const RGBTRIPLE		userBackgroundColor = {
		GetBValue (buttonColor), GetGValue (buttonColor), GetRValue (buttonColor)};

	for (UINT i = 0; i < numPixels; ++i)
	{
		if (pixels [i].rgbtBlue == kBackgroundColor.rgbtBlue &&
			pixels [i].rgbtGreen == kBackgroundColor.rgbtGreen &&
			pixels [i].rgbtRed == kBackgroundColor.rgbtRed)
		{
			pixels [i] = userBackgroundColor;
		}
	}
}

static void	MakeToolbarImageList (UINT inBitmapID, CImageList & outImageList)
{
	CBitmap bm;

	VERIFY (bm.Attach (::LoadImage (::AfxFindResourceHandle(
		MAKEINTRESOURCE (inBitmapID), RT_BITMAP),
		MAKEINTRESOURCE (inBitmapID), IMAGE_BITMAP, 0, 0,
		(LR_DEFAULTSIZE | LR_CREATEDIBSECTION))));

	::ReplaceBackgroundColor (bm);

	VERIFY (outImageList.Create(19, 31, ILC_COLOR24, 5, 0));
	VERIFY (outImageList.Add (&bm, RGB (0, 0, 0)) != -1);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pSplitterWindow = NULL;
	m_pDlgMotionViewMode = NULL;
}

CMainFrame::~CMainFrame()
{
	ms_ActorInstance.Destroy();
	ms_RaceManager.DestroyAccessor();

	if (m_pDlgMotionViewMode)
	{
		delete m_pDlgMotionViewMode;
		m_pDlgMotionViewMode = NULL;
	}

	if (m_pSplitterWindow)
	{
		delete m_pSplitterWindow;
		m_pSplitterWindow = NULL;
	}

	DestroyCollisionInstanceSystem();
	DestroyEffect();

	if (ms_pEffectData)
		ms_pEffectData->Clear();

	m_EffectManager.Destroy();
	m_SoundManager.Destroy();
	Clear();

	/////
	CTextFileLoader::DestroySystem();
	ms_EffectInstancePool.Clear();
	CParticleSystemInstance::DestroySystem();
	CEffectInstance::DestroySystem();
	CRaceData::DestroySystem();
	NRaceData::DestroySystem();
	CArea::DestroySystem();
	CActorInstance::DestroySystem();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize
	if (CGraphicDevice::CREATE_OK != m_GraphicDevice.Create(GetSafeHwnd(), 1280, 960))
		return -1;

	if (-1 == CreateToolBar())
		return -1;

	if (-1 == CreateWindows())
		return -1;

	if (-1 == CreateStatusBar())
		return -1;

	m_pDlgMotionViewMode = new CDlgMotionViewMode;
	m_pDlgMotionViewMode->Create(CDlgMotionViewMode::IDD, this);
	m_pDlgMotionViewMode->SetWindowPos(NULL, 10, 600, 0, 0, TRUE);
	m_pDlgMotionViewMode->ShowWindow(FALSE);

	m_SoundManager.Create();
	if (!Initialize())
		return -1;

	return 0;
}

int CMainFrame::CreateToolBar()
{
	if (!m_wndPlayToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndPlayToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;
	}

	::MakeToolbarImageList(IDB_TOOLBAR, m_ImageList);

	CToolBarCtrl & rToolBarCtrl = m_wndPlayToolBar.GetToolBarCtrl();
	rToolBarCtrl.SetImageList(&m_ImageList);
	rToolBarCtrl.SetDisabledImageList(&m_ImageListDisabled);
	rToolBarCtrl.SetHotImageList(&m_ImageListHot);

	m_wndPlayToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndPlayToolBar);

	///////////////////////////////////////////////////////////////////////////////////////////////

	if (!m_wndToolBarWindow.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
		return FALSE;

	if (!m_wndToolBarWindow.LoadToolBar(IDR_TOOLBAR_WINDOW))
		return FALSE;
	if (!m_wndToolBarWindow.LoadTrueColorToolBar(36, IDB_TOOLBAR_WINDOW, IDB_TOOLBAR_WINDOW, IDB_TOOLBAR_WINDOW))
		return FALSE;

	m_wndToolBarWindow.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBarWindow, AFX_IDW_DOCKBAR_TOP);

	///////////////////////////////////////////////////////////////////////////////////////////////

	if (!m_wndToolBarMode.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
		return FALSE;

	if (!m_wndToolBarMode.LoadToolBar(IDR_TOOLBAR_MODE))
		return FALSE;

	m_wndToolBarMode.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBarMode, AFX_IDW_DOCKBAR_TOP);
	ShowControlBar(&m_wndToolBarMode, FALSE, FALSE);

	/////
	/////

	// NOTE : Position 배치 사용법
	//        1. 기준이 되는 ToolBar의 Rect를 얻는다.
	//        2. 얻은 Rect에서 원하는 방향으로 OffsetRect 시킨다.
	//        3. 변환된 Rect로 Dock을 한번 더 해준다.
	//        [levites]

	RecalcLayout();

	CRect PlayToolBarRect;
	m_wndPlayToolBar.GetWindowRect(PlayToolBarRect);
	PlayToolBarRect.OffsetRect(1, 0);
	DockControlBar(&m_wndToolBarWindow, AFX_IDW_DOCKBAR_TOP, PlayToolBarRect);

	RecalcLayout();

	CRect WindowToolBarRect;
	m_wndToolBarWindow.GetWindowRect(WindowToolBarRect);
	WindowToolBarRect.OffsetRect(1, 0);
	DockControlBar(&m_wndToolBarMode, AFX_IDW_DOCKBAR_TOP, WindowToolBarRect);

	return 0;
}

int CMainFrame::CreateWindows()
{
	/////
	// Model List Bar
	if (!m_wndModelListBar.Create(_T("Model List"), this, CSize(200, 300), TRUE, 5002))
	{
		TRACE0("Failed to create Property Bar\n");
		return -1;		// fail to create
	}
	m_wndModelListBar.SetBarStyle(m_wndModelListBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndModelListBar.SetSCBStyle(m_wndModelListBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndModelListBar.EnableDocking(CBRS_ALIGN_LEFT);
	ShowControlBar(&m_wndModelListBar, TRUE, FALSE);
	DockControlBar(&m_wndModelListBar, AFX_IDW_DOCKBAR_LEFT);
	//////////////////////////////////////////////////////////////////////////////////////////////
	if (!m_resModelList.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS, CRect(0, 0, 0, 0), &m_wndModelListBar, 5102))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;
	}
	m_resModelList.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	/////
	CreateHighColorImageList(IDB_MODEL_TREE_ITEM, &m_ModelTreeItemImage);
	m_resModelList.SetImageList(&m_ModelTreeItemImage, TVSIL_NORMAL);
	//////////////////////////////////////////////////////////////////////////////////////////////

	/////
	// Motion List Bar
	if (!m_wndMotionListBar.Create(_T("Motion List"), this, CSize(200, 300), TRUE, 5003))
	{
		TRACE0("Failed to create Property Bar\n");
		return -1;		// fail to create
	}

	m_wndMotionListBar.SetBarStyle(m_wndMotionListBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndMotionListBar.SetSCBStyle(m_wndMotionListBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndMotionListBar.EnableDocking(CBRS_ALIGN_LEFT);

	ShowControlBar(&m_wndMotionListBar, TRUE, FALSE);
	DockControlBar(&m_wndMotionListBar, AFX_IDW_DOCKBAR_LEFT);

	//////////////////////////////////////////////////////////////////////////////////////////////
	if (!m_resMotionList.Create(WS_CHILD|WS_VISIBLE|LVS_LIST|LVS_SORTASCENDING|LVS_SHOWSELALWAYS|LVS_SINGLESEL, CRect(0, 0, 0, 0), &m_wndMotionListBar, 5103))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;
	}
	m_resMotionList.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	//////////////////////////////////////////////////////////////////////////////////////////////

	/////
	// Sound List Bar
	if (!m_wndSoundListBar.Create(_T("Sound List"), this, CSize(200, 300), TRUE, 5005))
	{
		TRACE0("Failed to create Property Bar\n");
		return -1;		// fail to create
	}
	m_wndSoundListBar.SetBarStyle(m_wndSoundListBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndSoundListBar.SetSCBStyle(m_wndSoundListBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndSoundListBar.EnableDocking(CBRS_ALIGN_RIGHT);
	ShowControlBar(&m_wndSoundListBar, TRUE, FALSE);
	DockControlBar(&m_wndSoundListBar, AFX_IDW_DOCKBAR_RIGHT);
	//////////////////////////////////////////////////////////////////////////////////////////////
	if (!m_pageSoundList.Create((CWnd*)&m_wndSoundListBar))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	/////
	// Motion Property
	if (!m_wndPropertyWindow.Create(_T("Motion Property"), this, CSize(200, 470), TRUE, 5004))
	{
		TRACE0("Failed to create Property Bar\n");
		return -1;		// fail to create
	}
	m_wndPropertyWindow.SetBarStyle(m_wndPropertyWindow.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndPropertyWindow.EnableDocking(CBRS_ALIGN_RIGHT);
	ShowControlBar(&m_wndPropertyWindow, TRUE, FALSE);
	////////////////////////////////////////////////////////////
	RecalcLayout();
	CRect PropertyBarRect;
	m_wndSoundListBar.GetWindowRect(PropertyBarRect);
	DockControlBar(&m_wndPropertyWindow, AFX_IDW_DOCKBAR_RIGHT, PropertyBarRect);
	//////////////////////////////////////////////////////////////////////////////////////////////
	if (!m_pageMotionProperty.Create((CWnd*)&m_wndPropertyWindow))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	RecalcLayout();

	// 여기서 다시 m_wndModelListBar 도킹을 해주는 이유는, 이렇게 하지 않으면
	// 모션 리스트와 모델 리스트 도킹바가 세로로 두개 한줄로 안붙고 두줄을 차지
	// 하기 때문이다.
	// 한줄 : |    세로로 두줄 : ||
	//        |                  ||
	CRect ModelListBarRect;
	m_wndMotionListBar.GetWindowRect(ModelListBarRect);
	DockControlBar(&m_wndModelListBar, AFX_IDW_DOCKBAR_LEFT, ModelListBarRect);

	return 0;
}

BOOL CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(g_auiStatusIndicators, sizeof(g_auiStatusIndicators) / sizeof(UINT)))
		return FALSE;

	return TRUE;
}

void CMainFrame::SetStatusBarString(const char * c_pszStr)
{
	m_wndStatusBar.SetPaneText(0, c_pszStr, TRUE);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	m_pSplitterWindow = new ST_SplitterWnd();
	m_pSplitterWindow->Create(this, RUNTIME_CLASS(CGraphViewer), RUNTIME_CLASS(CModelViewer), pContext, false);

	m_pGraphViewer = (CGraphViewer *) m_pSplitterWindow->GetPane(0, 0);
	m_pModelViewer = (CModelViewer *) m_pSplitterWindow->GetPane(1, 0);

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.x = 0;
	cs.y = 0;
	cs.cx = 1024;
	cs.cy = 768;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose() 
{
	if (isEdit())
	{
		int result = MessageBox("캐릭터 사운드 데이타가 수정 되었습니다. 저장 하시겠습니까?", NULL, MB_YESNOCANCEL);

		switch(result)
		{
			case 6: // Yes
				SaveSoundInformation();
				break;

			case 7: // No
				break;

			case 2: // Cancel
				return;
				break;
		}
	}

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnAppSaveSoundData() 
{
	SaveSoundInformation();
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Normal Functions

bool PackInitialize(const char * c_pszFolder)
{
	if (access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	std::string stFileName(stFolder);
	stFileName += "Index";

	CMappedFile file;
	LPCVOID pvData;

	if (!file.Create(stFileName.c_str(), &pvData, 0, 0))
	{
		LogBoxf("FATAL ERROR! File not exist: %s", stFileName.c_str());
		TraceError("FATAL ERROR! File not exist: %s", stFileName.c_str());
		return true;
	}

	CMemoryTextFileLoader TextLoader;
	TextLoader.Bind(file.Size(), pvData);

	bool bPackFirst = TRUE;

	std::string strPackType = TextLoader.GetLineString(0).c_str();

	if (strPackType.compare("FILE") && strPackType.compare("PACK"))
	{
		TraceError("Pack/Index has invalid syntax. First line must be 'PACK' or 'FILE'");
		return false;
	}

	if (0 == strPackType.compare("FILE"))
	{
		bPackFirst = FALSE;
		Tracef("알림: 파일 모드입니다.\n");
	}
	else
	{
		Tracef("알림: 팩 모드입니다.\n");
	}

	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode(); // Miles 파일 콜백을 셋팅

	for (DWORD i = 1; i < TextLoader.GetLineCount() - 1; i += 2)
	{
		const std::string & c_rstFolder = TextLoader.GetLineString(i);
		const std::string & c_rstName = TextLoader.GetLineString(i + 1);

		CEterPackManager::Instance().RegisterPack((stFolder + c_rstName).c_str(), c_rstFolder.c_str());
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());
	return true;
}

bool CMainFrame::Initialize()
{
	FILE * file = fopen("config.cfg", "r");
	if (file)
	{
		char szLine[256+1];
		if (fgets(szLine, 256, file))
		{
			int ir, ig, ib;
			sscanf(szLine, " %d %d %d ", &ir, &ig, &ib);

			extern float g_frClearColor;
			extern float g_fgClearColor;
			extern float g_fbClearColor;

			g_frClearColor = float(ir) / 255.0f;
			g_fgClearColor = float(ig) / 255.0f;
			g_fbClearColor = float(ib) / 255.0f;
		}

		fclose(file);
	}

	/////////

	__InitGrannyError();

	ms_pEffectData = new CEffectData;
	ms_RaceManager.SetRaceData(0, &ms_RaceData);

	CTimer& rkTimer=CTimer::Instance();
	rkTimer.SetBaseTime();
	rkTimer.UseCustomTime();

	RegisterResource();
	m_LightManager.Initialize();

#ifdef _DEBUG
	OpenConsoleWindow();
#endif

	PackInitialize("pack");

	LoadSoundScript("SoundScript.txt");
	LoadSoundInformation();

	return true;
}

void CMainFrame::Process()
{
	long lStartTime = timeGetTime();

	CTimer& rkTimer=CTimer::Instance();
	rkTimer.Advance();

	m_pGraphViewer->Update();

	TimeProcess();

	m_pModelViewer->Render();
	m_pGraphViewer->Render();

	long lElapsedTime = timeGetTime() - lStartTime;
	if (lElapsedTime < 16)
	{
		Sleep(16 - lElapsedTime);
	}
}

void CMainFrame::Clear()
{
	ms_iMode = MODE_EDIT;
	ms_icurType = MODEL_TYPE_NONE;
	ms_isEdit = false;
	ms_PlayFlag = false;
	ms_LoopFlag = false;
	ms_fLocalTime = 0.001f;
	ms_dwLocalFrame = 0;
	ms_dwModelIndex = 0;
	ms_dwShapeIndex = 0;
	ms_dwMotionIndex = 0;
	ms_dwSoundIndex = 0;
	SetLimitTime(2.0f);

	ms_ActorInstance.Clear();
	if (ms_pEffectData)
	{
		ms_pEffectData->Clear();
		delete ms_pEffectData;
	}
	ms_pEffectData = NULL;
	stl_wipe_second(ms_ItemDataMap);
}

// ViewModelList
void CMainFrame::OnViewModelList() 
{
	if (m_wndModelListBar.IsVisible())
		ShowControlBar(&m_wndModelListBar, FALSE, FALSE);
	else
		ShowControlBar(&m_wndModelListBar, TRUE, FALSE);
}
void CMainFrame::OnUpdateViewModelList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndModelListBar.IsVisible());
}

// ViewMotionList
void CMainFrame::OnViewMotionList() 
{
	if (m_wndMotionListBar.IsVisible())
		ShowControlBar(&m_wndMotionListBar, FALSE, FALSE);
	else
		ShowControlBar(&m_wndMotionListBar, TRUE, FALSE);
}
void CMainFrame::OnUpdateViewMotionList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndMotionListBar.IsVisible());
}

// ViewMotionProperty
void CMainFrame::OnViewMotionProperty() 
{
	if (m_wndPropertyWindow.IsVisible())
		ShowControlBar(&m_wndPropertyWindow, FALSE, FALSE);
	else
		ShowControlBar(&m_wndPropertyWindow, TRUE, FALSE);
}
void CMainFrame::OnUpdateViewMotionProperty(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndPropertyWindow.IsVisible());
}

// ViewSoundList
void CMainFrame::OnViewSoundList()
{
	if (m_wndSoundListBar.IsVisible())
		ShowControlBar(&m_wndSoundListBar, FALSE, FALSE);
	else
		ShowControlBar(&m_wndSoundListBar, TRUE, FALSE);
}
void CMainFrame::OnUpdateViewSoundList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndSoundListBar.IsVisible());
}

// Mode - Edit
void CMainFrame::OnWorkModeEdit() 
{
	ms_iMode = MODE_EDIT;
	m_pDlgMotionViewMode->ShowWindow(FALSE);
}
void CMainFrame::OnUpdateWorkModeEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(MODE_EDIT == ms_iMode);
}

// Mode - View
void CMainFrame::OnWorkModeView() 
{
	ms_iMode = MODE_VIEW;
	m_pDlgMotionViewMode->ShowWindow(TRUE);
}
void CMainFrame::OnUpdateWorkModeView(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(MODE_VIEW == ms_iMode);
}

// Granny Error
void CMainFrame::GrannyError(granny_log_message_type Type,
						granny_log_message_origin Origin,
						char const *Error,
						void *UserData)
{
	TCharacterDataMap::iterator itorModel = ms_CharacterDataMap.find(ms_dwModelIndex);
	if (itorModel == ms_CharacterDataMap.end())
		return;

	TCharacterData & rCharacterData = itorModel->second;
	TMotionDataMap::iterator itorMotion = rCharacterData.MotionDataMap.find(ms_dwMotionIndex);
	if (rCharacterData.MotionDataMap.end() != itorMotion)
	{
		TMotionData & rMotionData = itorMotion->second;

		Tracef("CurrentAnimationFile : %s\n", rMotionData.strFullFileName.c_str());
		TraceError("GRANNY: %s", Error);
		Tracef("\n");
	}
}

void CMainFrame::__InitGrannyError()
{
	granny_log_callback Callback;
    Callback.Function = GrannyError;
    Callback.UserData = 0;
    GrannySetLogCallback(&Callback);
}

void CMainFrame::OnCaptureScreen() 
{
}

void CMainFrame::OnReloadAllResource() 
{
	ms_RaceData.Destroy();
	ms_ActorInstance.Clear();
	ms_ActorInstance.ClearAttachingEffect();
	ms_pEffectData->Clear();
	CEffectManager::Instance().DeleteAllInstances();
	DestroyEffect();
	Refresh();
}

void CMainFrame::OnCharVisibleToggle() 
{
	if (!m_pModelViewer)
		return;

	m_pModelViewer->ToggleVisable();
}
