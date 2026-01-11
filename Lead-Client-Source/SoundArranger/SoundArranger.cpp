// SoundArranger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SoundArranger.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "SoundArrangerDoc.h"
#include "SoundArrangerView.h"

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment( lib, "granny2.lib" )
#pragma comment( lib, "mss32.lib" )
#pragma comment( lib, "SpeedTreeRT.lib" )
//#pragma comment( lib, "version.lib" )
//#pragma comment( lib, "python22.lib" )
//#pragma comment( lib, "imagehlp.lib" )
//#pragma comment( lib, "devil.lib" )
//#pragma comment( lib, "winmm.lib" )
//#pragma comment( lib, "imm32.lib" )
//#pragma comment( lib, "oldnames.lib" )
//#pragma comment( lib, "dinput8.lib" )
//#pragma comment( lib, "dxguid.lib" )
//#pragma comment( lib, "ws2_32.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerApp

BEGIN_MESSAGE_MAP(CSoundArrangerApp, CWinApp)
	//{{AFX_MSG_MAP(CSoundArrangerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerApp construction

CSoundArrangerApp::CSoundArrangerApp()
{
	timeBeginPeriod(1);
}

CSoundArrangerApp::~CSoundArrangerApp()
{
	if (m_pMainWnd)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}

	timeEndPeriod(1);
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CSoundArrangerApp object

CSoundArrangerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerApp initialization

BOOL CSoundArrangerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register document templates

	//CMultiDocTemplate* pDocTemplate;
	//pDocTemplate = new CMultiDocTemplate(
		//IDR_SOUNDATYPE,
		//RUNTIME_CLASS(CSoundArrangerDoc),
		//RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		//RUNTIME_CLASS(CSoundArrangerView));
	//AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame * pMainFrame = new CMainFrame;
	
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
		
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSoundArrangerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSoundArrangerApp message handlers

BOOL CSoundArrangerApp::OnIdle(LONG lCount) 
{
//	static long s_lLastTime = timeGetTime();
//	long lcurTime = timeGetTime();
//	Tracef("[%d] %d / %d\n", lCount, lcurTime, lcurTime - s_lLastTime);
//	s_lLastTime = lcurTime;

	if (CWinApp::OnIdle(lCount))
		return TRUE;

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->Process();

	return TRUE;
}
