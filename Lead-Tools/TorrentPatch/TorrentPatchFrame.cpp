#include "Metin2TorrentConsole.h"
#include "TorrentPatchFrame.h"
#include "TorrentPatchPanel.h"
#include "TorrentPatchEvent.h"
#include "PatchThread.h"
#include "TorrentPatchApp.h"
#include "TorrentPatchTaskBarIcon.h"

#include "StringFromFile.h"
#include "CMiniUnZIP.h"

#include <wx-2.8.10/mstream.h>
#include "resource.h"

#include "TorrentSkin.h"

BEGIN_EVENT_TABLE(EL_TorrentPatchFrame, wxFrame)
	EVT_CLOSE(EL_TorrentPatchFrame::OnClose)
	EVT_ERASE_BACKGROUND(EL_TorrentPatchFrame::OnErase)
END_EVENT_TABLE()

EL_TorrentPatchFrame::EL_TorrentPatchFrame(const wxString& title)
: wxFrame(NULL, ID_FRAME, title, wxDefaultPosition, wxSize(400, 280), wxMINIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
, m_patchThreadPtr(NULL)
, m_patchPanelPtr(NULL)
{
	CentreOnParent();

	EL_TorrentPatchPanel& panel = *new EL_TorrentPatchPanel(this);
	panel.Layout();

	m_patchPanelPtr = &panel;

	m_taskBarIcon = new EL_TorrentPatchTaskBarIcon(this);

	BOOL		bLoaded = FALSE;
	std::string	icoData;

#ifdef _SFX_ENABLE
	CMiniUnZIP	cZIP;

	TCHAR		tszSelfFileName[MAX_PATH];
	::GetModuleFileName( NULL, tszSelfFileName, MAX_PATH );
	if( cZIP.OpenArchive( tszSelfFileName ) ) {
		DWORD dwFileIndex = cZIP.GetFileIndex( FILENAME_APP_ICON );
		if( dwFileIndex != CMiniUnZIP::FILE_NOT_FOUND ) {
			DWORD dwFileSize = cZIP.GetFileLength( dwFileIndex );
			icoData.resize( dwFileSize );
			cZIP.ReadFile( dwFileIndex, (&icoData[0]) );
			bLoaded	= TRUE;
		}
		cZIP.CloseArchive();
	}
#endif
	if (bLoaded == FALSE) {
		if( EL_StringFromDiskFileW(icoData, FILENAME_APP_ICON) ) {
			bLoaded = TRUE;
		}
	}

	wxIcon icon;
	if( bLoaded ) {
		wxMemoryInputStream mis(icoData.c_str(), icoData.length());
		wxImage img(mis, wxBITMAP_TYPE_ICO, -1);
		wxBitmap bmp(img);

		icon.CopyFromBitmap(bmp);		
	} else {
		icon.SetHICON( ::LoadIcon( ::GetModuleHandle( NULL ), MAKEINTRESOURCE( IDI_MAIN_ICON ) ) );
	}
	icon.SetSize( ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON) );

	if( m_taskBarIcon->SetIcon( icon, EL_LTEXT("TORRENT_START") ) == false )
		wxMessageBox( EL_LTEXT("SET_ICON_ERROR") );

	SetIcon(icon);

	if(g_pcSkin) {		
		wxBitmap bmp(g_pcSkin->GetMainBitmap());
		wxMask* maskPtr = new wxMask(bmp, wxColour(0, 0, 0, 0));
		bmp.SetMask(maskPtr);

		wxRegion region(bmp);
		wxSize regionSize(region.GetBox().GetSize());

		SetWindowStyleFlag(wxFRAME_SHAPED | wxCLIP_CHILDREN);
		SetShape(region);
		SetSize(regionSize);		
		CentreOnParent();
	}
}

EL_TorrentPatchFrame::~EL_TorrentPatchFrame()
{
	MA_SAFE_DELETE(m_taskBarIcon);
}

bool EL_TorrentPatchFrame::Destroy()
{
	EL_TorrentPatchApp& cApp = wxGetApp();
	cApp.RequestTerminate();
	cApp.WaitUntilAllThreadsDone();

	bool bRet = wxFrame::Destroy();

	SkinDestroy();

	return bRet;
}

void EL_TorrentPatchFrame::StartPatchThread()
{
	if( m_patchPanelPtr == NULL || m_taskBarIcon == NULL )
		return;

	EL_PatchThread& patchThread = *new EL_PatchThread(m_patchPanelPtr, m_taskBarIcon);

	if( patchThread.Create() == wxTHREAD_NO_ERROR ) {
		patchThread.Run();
		EL_TorrentPatchApp& app = wxGetApp();
		app.RegisterThread(&patchThread);
	} else {
		delete &patchThread;
	}
}

void EL_TorrentPatchFrame::OnClose( wxCloseEvent& WXUNUSED(evt) )
{
	this->Destroy();
}

void EL_TorrentPatchFrame::OnErase( wxEraseEvent& cEvent )
{
	if( NULL == g_pcSkin )
		cEvent.Skip();
}

void  EL_TorrentPatchFrame::OnDynamic( wxCommandEvent& WXUNUSED(event) )
{
}

