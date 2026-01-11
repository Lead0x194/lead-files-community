// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__C7A49A70_9E36_48C4_8F16_85CC22722D2E__INCLUDED_)
#define AFX_MAINFRM_H__C7A49A70_9E36_48C4_8F16_85CC22722D2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <EterBase/Timer.h>
#include <EterBase/lzo.h>
#include <EterPack/EterPack.h>
#include <EterPack/EterPackManager.h>
#include <EterLib/ResourceManager.h>
#include <EterLib/GrpDevice.h>
#include <EterLib/GrpLightManager.h>
#include <GameLib/GameEventManager.h>
#include <GameLib/FlyingObjectManager.h>
#include <EffectLib/EffectManager.h>
#include <milesLib/SoundManager.h>

#include "ST_SplitterWnd.h"
#include "DlgMotionViewMode.h"
#include "ResModelList.h"
#include "ResMotionList.h"
#include "PageSoundList.h"
#include "PageMotionProperty.h"

#include "ModelViewer.h"
#include "GraphViewer.h"

class CMainFrame : public CMDIFrameWnd, public CManager
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnAppSaveSoundData();
	afx_msg void OnToolbarPlay();
	afx_msg void OnToolbarPlayloop();
	afx_msg void OnToolbarReverse();
	afx_msg void OnToolbarPause();
	afx_msg void OnToolbarStop();
	afx_msg void OnKeyboardPlay();
	afx_msg void OnCancelSound();
	afx_msg void OnUpdateViewModelList(CCmdUI* pCmdUI);
	afx_msg void OnViewModelList();
	afx_msg void OnViewMotionList();
	afx_msg void OnUpdateViewMotionList(CCmdUI* pCmdUI);
	afx_msg void OnViewMotionProperty();
	afx_msg void OnUpdateViewMotionProperty(CCmdUI* pCmdUI);
	afx_msg void OnViewSoundList();
	afx_msg void OnUpdateViewSoundList(CCmdUI* pCmdUI);
	afx_msg void OnWorkModeEdit();
	afx_msg void OnUpdateWorkModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnWorkModeView();
	afx_msg void OnUpdateWorkModeView(CCmdUI* pCmdUI);
	afx_msg void OnCaptureScreen();
	afx_msg void OnReloadAllResource();
	afx_msg void OnCharVisibleToggle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	bool	Initialize();
	void	Clear();

	void	ControlSoundInformationFiles(BOOL isLoad);
	void	LoadSoundInformation();
	void	SaveSoundInformation();

	BOOL	LoadSoundScript(const char * c_szFileName);
	BOOL	LoadSoundScriptGroup(HTREEITEM hParent, CTextFileLoader & rTextFileLoader);
	BOOL	LoadSoundScriptModel(HTREEITEM hParent, CTextFileLoader & rTextFileLoader, EModelType eModelType);
	BOOL	LoadSoundScriptEffect(HTREEITEM hParent, CTextFileLoader & rTextFileLoader);
	BOOL	LoadSoundScriptEquipment(CTextFileLoader & rTextFileLoader, TEquipmentDataList & rEquipmentDataList);

	void	SetStatusBarString(const char * c_pszStr);

	void	Process();

protected:
	void RegisterResource();

	int CreateToolBar();
	int CreateWindows();
	int CreateStatusBar();

	void __InitGrannyError();
	static void GrannyError(granny_log_message_type Type, granny_log_message_origin Origin, char const *Error, void *UserData);

protected:
	ST_SplitterWnd * m_pSplitterWindow;
	CToolBar m_wndPlayToolBar;
	CTrueColorToolBar m_wndToolBarWindow;
	CTrueColorToolBar m_wndToolBarMode;
	CDlgMotionViewMode * m_pDlgMotionViewMode;

	// Status Bar
	CStatusBar m_wndStatusBar;

	// Model List
	CSizingControlBarCF m_wndModelListBar;
	CResModelList m_resModelList;

	// Motion List
	CSizingControlBarCF m_wndMotionListBar;
	CResMotionList m_resMotionList;

	// Current Motion Property
	CSizingControlBarCF m_wndPropertyWindow;
	CPageMotionProperty m_pageMotionProperty;

	// Sound List
	CSizingControlBarCF m_wndSoundListBar;
	CPageSoundList m_pageSoundList;

	CModelViewer * m_pModelViewer;
	CGraphViewer * m_pGraphViewer;

	CImageList m_ImageList;
	CImageList m_ImageListHot;
	CImageList m_ImageListDisabled;
	CImageList m_ModelTreeItemImage;

	std::string m_stCharacterSetFileName;

private:
	CGraphicDevice			m_GraphicDevice;
	CEffectManager			m_EffectManager;
	CSoundManager			m_SoundManager;
	CEterPack				m_pack;
	CEterPackManager		m_EterPackManager;
	CResourceManager		m_resourceManager;
	CGameEventManager		m_eventManager;
	CLZO					lzo;
	CLightManager			m_LightManager;
	CTimer					m_Timer;
	CFlyingManager			m_FlyingManager;
	CCullingManager			m_CullingManager;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__C7A49A70_9E36_48C4_8F16_85CC22722D2E__INCLUDED_)
