#include "Metin2TorrentConsole.h"

#include "WebBrowserPanel.h"

#include <windows.h>

#include <atlbase.h>
#include <exdisp.h>
#include <mshtmhst.h>

BEGIN_EVENT_TABLE(EL_WebBrowserPanel, wxPanel)
	EVT_SIZE(EL_WebBrowserPanel::__OnSize)
END_EVENT_TABLE()

namespace { struct webhostwnd; }

class CWebControlImpl
{
public:
	CWebControlImpl(HWND hWnd, const wxString& URL);
	~CWebControlImpl();

	void SetSize(UINT Width, UINT Height);

	void Navigate(EL_C_WCHAR_P URL);

private:
	struct webhostwnd* m_wnd;
};

EL_WebBrowserPanel::EL_WebBrowserPanel(wxWindow* parent, wxWindowID id, const wxString& URL, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size, wxNO_BORDER), m_webControl(NULL)
{
	OleInitialize(NULL);

	HWND hWnd = static_cast<HWND>(GetHandle());
	m_webControl = new CWebControlImpl(hWnd, URL);
}

EL_WebBrowserPanel::~EL_WebBrowserPanel()
{
	MA_SAFE_DELETE(m_webControl);
	OleUninitialize();
}

void EL_WebBrowserPanel::Go(EL_C_WCHAR_P URL)
{
	if (m_webControl)
		m_webControl->Navigate(URL);
}

void EL_WebBrowserPanel::__OnSize(wxSizeEvent& evt)
{
	m_webControl->SetSize(evt.GetSize().x, evt.GetSize().y);
}

//............implements
namespace
{
#ifdef _DEBUG
#define NOTIMPLEMENTED __asm{ int 3 }; return E_NOTIMPL
#else
#define NOTIMPLEMENTED return E_NOTIMPL
#endif
	struct CNullStorage : public IStorage
	{
		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid,void ** ppvObject);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);
		// IStorage
		STDMETHODIMP CreateStream(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream ** ppstm);
		STDMETHODIMP OpenStream(const WCHAR * pwcsName,void * reserved1,DWORD grfMode,DWORD reserved2,IStream ** ppstm);
		STDMETHODIMP CreateStorage(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage ** ppstg);
		STDMETHODIMP OpenStorage(const WCHAR * pwcsName,IStorage * pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage ** ppstg);
		STDMETHODIMP CopyTo(DWORD ciidExclude,IID const * rgiidExclude,SNB snbExclude,IStorage * pstgDest);
		STDMETHODIMP MoveElementTo(const OLECHAR * pwcsName,IStorage * pstgDest,const OLECHAR* pwcsNewName,DWORD grfFlags);
		STDMETHODIMP Commit(DWORD grfCommitFlags);
		STDMETHODIMP Revert(void);
		STDMETHODIMP EnumElements(DWORD reserved1,void * reserved2,DWORD reserved3,IEnumSTATSTG ** ppenum);
		STDMETHODIMP DestroyElement(const OLECHAR * pwcsName);
		STDMETHODIMP RenameElement(const WCHAR * pwcsOldName,const WCHAR * pwcsNewName);
		STDMETHODIMP SetElementTimes(const WCHAR * pwcsName,FILETIME const * pctime,FILETIME const * patime,FILETIME const * pmtime);
		STDMETHODIMP SetClass(REFCLSID clsid);
		STDMETHODIMP SetStateBits(DWORD grfStateBits,DWORD grfMask);
		STDMETHODIMP Stat(STATSTG * pstatstg,DWORD grfStatFlag);
	};

	struct webhostwnd;

	struct CMyFrame : public IOleInPlaceFrame
	{
		webhostwnd* host;
		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid,void ** ppvObject);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);
		// IOleWindow
		STDMETHODIMP GetWindow(HWND FAR* lphwnd);
		STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
		// IOleInPlaceUIWindow
		STDMETHODIMP GetBorder(LPRECT lprectBorder);
		STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
		STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
		STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName);
		// IOleInPlaceFrame
		STDMETHODIMP InsertMenus(HMENU hmenuShared,LPOLEMENUGROUPWIDTHS lpMenuWidths);
		STDMETHODIMP SetMenu(HMENU hmenuShared,HOLEMENU holemenu,HWND hwndActiveObject);
		STDMETHODIMP RemoveMenus(HMENU hmenuShared);
		STDMETHODIMP SetStatusText(LPCOLESTR pszStatusText);
		STDMETHODIMP EnableModeless(BOOL fEnable);
		STDMETHODIMP TranslateAccelerator(  LPMSG lpmsg,WORD wID);

	};

	struct CMyDocHostUIHandler : public IDocHostUIHandler
	{
		STDMETHODIMP QueryInterface(REFIID /*riid*/,void ** /*ppvObject*/) {NOTIMPLEMENTED;}

		STDMETHODIMP_(ULONG) AddRef(void)	{return 1;}

		STDMETHODIMP_(ULONG) Release(void)	{return 1;}

		STDMETHODIMP ShowContextMenu( 
			/* [in] */ DWORD /*dwID*/,
			/* [in] */ POINT* /*ppt*/,
			/* [in] */ IUnknown* /*pcmdtReserved*/,
			/* [in] */ IDispatch* /*pdispReserved*/) {return S_FALSE;}

		STDMETHODIMP GetHostInfo( 
			/* [out][in] */ DOCHOSTUIINFO *pInfo) 
		{
			pInfo->cbSize = sizeof(DOCHOSTUIINFO);
			pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
			pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

			return(S_OK);
		}

		STDMETHODIMP ShowUI( 
			/* [in] */ DWORD /*dwID*/,
			/* [in] */ IOleInPlaceActiveObject* /*pActiveObject*/,
			/* [in] */ IOleCommandTarget* /*pCommandTarget*/,
			/* [in] */ IOleInPlaceFrame* /*pFrame*/,
			/* [in] */ IOleInPlaceUIWindow* /*pDoc*/) {return S_FALSE;}

		STDMETHODIMP HideUI( void) {return S_FALSE;}

		STDMETHODIMP UpdateUI( void) {return S_FALSE;}

		STDMETHODIMP EnableModeless( 
			/* [in] */ BOOL /*fEnable*/) {return S_FALSE;}

		STDMETHODIMP OnDocWindowActivate( 
			/* [in] */ BOOL /*fActivate*/) {return S_FALSE;}

		STDMETHODIMP OnFrameWindowActivate( 
			/* [in] */ BOOL /*fActivate*/) {return S_FALSE;}

		STDMETHODIMP ResizeBorder( 
			/* [in] */ LPCRECT /*prcBorder*/,
			/* [in] */ IOleInPlaceUIWindow* /*pUIWindow*/,
			/* [in] */ BOOL /*fRameWindow*/) {return S_FALSE;}

		STDMETHODIMP TranslateAccelerator( 
			/* [in] */ LPMSG /*lpMsg*/,
			/* [in] */ const GUID* /*pguidCmdGroup*/,
			/* [in] */ DWORD /*nCmdID*/) {return S_FALSE;}

		STDMETHODIMP GetOptionKeyPath( 
			/* [out] */ 
			__out  LPOLESTR* /*pchKey*/,
			/* [in] */ DWORD /*dw*/)
		{
			return S_FALSE;
		}

		STDMETHODIMP GetDropTarget( 
			/* [in] */ IDropTarget* /*pDropTarget*/,
			/* [out] */ IDropTarget** /*ppDropTarget*/) {return S_FALSE;}

		STDMETHODIMP GetExternal( 
			/* [out] */ IDispatch** /*ppDispatch*/) {return S_FALSE;}

		STDMETHODIMP TranslateUrl( 
			/* [in] */ DWORD /*dwTranslate*/,
			/* [in] */ 
			__in __nullterminated  OLECHAR* /*pchURLIn*/,
			/* [out] */ 
			__out  OLECHAR** /*ppchURLOut*/) {return S_FALSE;}

		STDMETHODIMP FilterDataObject( 
			/* [in] */ IDataObject* /*pDO*/,
			/* [out] */ IDataObject** /*ppDORet*/) {return S_FALSE;}
	};



	struct CMySite : public IOleClientSite, public IOleInPlaceSite
	{
		webhostwnd* host;
		CMyDocHostUIHandler uiHandler;

		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid,void ** ppvObject);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);
		// IOleClientSite
		STDMETHODIMP SaveObject();
		STDMETHODIMP GetMoniker(DWORD dwAssign,DWORD dwWhichMoniker,IMoniker ** ppmk);
		STDMETHODIMP GetContainer(LPOLECONTAINER FAR* ppContainer);
		STDMETHODIMP ShowObject();
		STDMETHODIMP OnShowWindow(BOOL fShow);
		STDMETHODIMP RequestNewObjectLayout();
		// IOleWindow
		STDMETHODIMP GetWindow(HWND FAR* lphwnd);
		STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
		// IOleInPlaceSite methods
		STDMETHODIMP CanInPlaceActivate();
		STDMETHODIMP OnInPlaceActivate();
		STDMETHODIMP OnUIActivate();
		STDMETHODIMP GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHODIMP Scroll(SIZE scrollExtent);
		STDMETHODIMP OnUIDeactivate(BOOL fUndoable);
		STDMETHODIMP OnInPlaceDeactivate();
		STDMETHODIMP DiscardUndoState();
		STDMETHODIMP DeactivateAndUndo();
		STDMETHODIMP OnPosRectChange(LPCRECT lprcPosRect);
	};


	struct CMyContainer : public IOleContainer
	{
		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid,void ** ppvObject);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);
		// IParseDisplayName
		STDMETHODIMP ParseDisplayName(IBindCtx *pbc,LPOLESTR pszDisplayName,ULONG *pchEaten,IMoniker **ppmkOut);
		// IOleContainer
		STDMETHODIMP EnumObjects(DWORD grfFlags,IEnumUnknown **ppenum);
		STDMETHODIMP LockContainer(BOOL fLock);
	};


	struct webhostwnd
	{
		webhostwnd();
		~webhostwnd();

		static void Initialize(HMODULE);
		static webhostwnd* Create(HINSTANCE hAppInstance);

		virtual BOOL HandleMessage(UINT,WPARAM,LPARAM,LRESULT*);

		void CreateEmbeddedWebControl(const wchar_t* URL);
		void UnCreateEmbeddedWebControl(void);

		void SetSize(UINT width, UINT height);
		void Navigate(const WCHAR* URL);

		HWND operator =(webhostwnd* rhs);

		CNullStorage storage;
		CMySite site;
		CMyFrame frame;		

		IOleObject* mpWebObject;

		void OnPaint(HDC hdc);

		void Release();

		HWND hwnd;
		static TCHAR szClassName[];
	};

	TCHAR webhostwnd::szClassName[] = TEXT("ALFKJASF");

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
	{
		static webhostwnd* _this = NULL;
		switch (uiMessage)
		{
		case WM_CREATE:
			{
				_this = (webhostwnd*) lParam;
				//_this->CreateEmbeddedWebControl();
			}
			break;
		case WM_DESTROY:
			//_this->UnCreateEmbeddedWebControl();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uiMessage, wParam, lParam);
		}
		return FALSE;
	}

	void webhostwnd::Initialize(HMODULE hInstance)
	{
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(wcex));

		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.lpszClassName = szClassName;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

		if (NULL == RegisterClassEx(&wcex))
		{
			wcex = wcex;
		}
	}

	void webhostwnd::Release()
	{
		//TODO:something
	}


	///////////////////////////////////////////////////////////////////////////////


	// IUnknown 

	STDMETHODIMP CNullStorage::QueryInterface(REFIID /*riid*/,void ** /*ppvObject*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP_(ULONG) CNullStorage::AddRef(void)
	{
		return 1;
	}

	STDMETHODIMP_(ULONG) CNullStorage::Release(void)
	{
		return 1;
	}


	// IStorage
	STDMETHODIMP CNullStorage::CreateStream(const WCHAR* /*pwcsName*/, DWORD /*grfMode*/, DWORD /*reserved1*/,DWORD /*reserved2*/,IStream** /*ppstm*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::OpenStream(const WCHAR* /*pwcsName*/,void* /*reserved1*/,DWORD /*grfMode*/,DWORD /*reserved2*/,IStream** /*ppstm*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::CreateStorage(const WCHAR* /*pwcsName*/,DWORD /*grfMode*/,DWORD /*reserved1*/,DWORD /*reserved2*/,IStorage** /*ppstg*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::OpenStorage(const WCHAR* /*pwcsName*/,IStorage* /*pstgPriority*/,DWORD /*grfMode*/,SNB /*snbExclude*/,DWORD /*reserved*/,IStorage** /*ppstg*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::CopyTo(DWORD /*ciidExclude*/,IID const* /*rgiidExclude*/,SNB /*snbExclude*/,IStorage* /*pstgDest*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::MoveElementTo(const OLECHAR* /*pwcsName*/,IStorage* /*pstgDest*/,const OLECHAR* /*pwcsNewName*/,DWORD /*grfFlags*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::Commit(DWORD /*grfCommitFlags*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::Revert(void)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::EnumElements(DWORD /*reserved1*/, void* /*reserved2*/, DWORD /*reserved3*/, IEnumSTATSTG** /*ppenum*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::DestroyElement(const OLECHAR* /*pwcsName*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::RenameElement(const WCHAR* /*pwcsOldName*/,const WCHAR* /*pwcsNewName*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::SetElementTimes(const WCHAR* /*pwcsName*/,FILETIME const* /*pctime*/,FILETIME const* /*patime*/,FILETIME const* /*pmtime*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::SetClass(REFCLSID /*clsid*/)
	{
		return S_OK;
	}

	STDMETHODIMP CNullStorage::SetStateBits(DWORD /*grfStateBits*/,DWORD /*grfMask*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CNullStorage::Stat(STATSTG* /*pstatstg*/,DWORD /*grfStatFlag*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::QueryInterface(REFIID riid,void ** ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IOleClientSite)
			*ppvObject = (IOleClientSite*)this;
		else if (riid == IID_IOleInPlaceSite) // || riid == IID_IOleInPlaceSiteEx || riid == IID_IOleInPlaceSiteWindowless)
			*ppvObject = (IOleInPlaceSite*)this;
		else if (riid == IID_IDocHostUIHandler)
			*ppvObject = &uiHandler;			
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		return S_OK;
	}

	STDMETHODIMP_(ULONG) CMySite::AddRef(void)
	{
		return 1;
	}

	STDMETHODIMP_(ULONG) CMySite::Release(void)
	{
		return 1;
	}


	// IOleClientSite

	STDMETHODIMP CMySite::SaveObject()
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::GetMoniker(DWORD /*dwAssign*/, DWORD /*dwWhichMoniker*/, IMoniker** /*ppmk*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::GetContainer(LPOLECONTAINER FAR* ppContainer)
	{
		// We are a simple object and don't support a container.
		*ppContainer = NULL;

		return E_NOINTERFACE;
	}

	STDMETHODIMP CMySite::ShowObject()
	{
		//  NOTIMPLEMENTED;
		// huh?
		return NOERROR;
	}

	STDMETHODIMP CMySite::OnShowWindow(BOOL /*fShow*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::RequestNewObjectLayout()
	{
		NOTIMPLEMENTED;
	}

	// IOleWindow

	STDMETHODIMP CMySite::GetWindow(HWND FAR* lphwnd)
	{
		*lphwnd = host->hwnd;

		return S_OK;
	}

	STDMETHODIMP CMySite::ContextSensitiveHelp(BOOL /*fEnterMode*/)
	{
		NOTIMPLEMENTED;
	}

	// IOleInPlaceSite


	STDMETHODIMP CMySite::CanInPlaceActivate()
	{
		// Yes we can
		return S_OK;
	}

	STDMETHODIMP CMySite::OnInPlaceActivate()
	{
		// Why disagree.
		return S_OK;
	}

	STDMETHODIMP CMySite::OnUIActivate()
	{
		return S_OK;
	}

	STDMETHODIMP CMySite::GetWindowContext(
		LPOLEINPLACEFRAME FAR* ppFrame,
		LPOLEINPLACEUIWINDOW FAR* ppDoc,
		LPRECT prcPosRect,
		LPRECT prcClipRect,
		LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		*ppFrame = &host->frame;
		*ppDoc = NULL;
		GetClientRect(host->hwnd,prcPosRect);
		GetClientRect(host->hwnd,prcClipRect);

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = host->hwnd;
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;

		return S_OK;
	}

	STDMETHODIMP CMySite::Scroll(SIZE /*scrollExtent*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::OnUIDeactivate(BOOL /*fUndoable*/)
	{
		return S_OK;
	}

	STDMETHODIMP CMySite::OnInPlaceDeactivate()
	{
		return S_OK;
	}

	STDMETHODIMP CMySite::DiscardUndoState()
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::DeactivateAndUndo()
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMySite::OnPosRectChange(LPCRECT /*lprcPosRect*/)
	{
		return S_OK;
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// CMyFrame
	//


	// IUnknown
	STDMETHODIMP CMyFrame::QueryInterface(REFIID /*riid*/, void** /*ppvObject*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP_(ULONG) CMyFrame::AddRef(void)
	{
		return 1;
	}

	STDMETHODIMP_(ULONG) CMyFrame::Release(void)
	{
		return 1;
	}

	// IOleWindow
	STDMETHODIMP CMyFrame::GetWindow(HWND FAR* lphwnd)
	{
		*lphwnd = this->host->hwnd;
		return S_OK;
		//  NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::ContextSensitiveHelp(BOOL /*fEnterMode*/)
	{
		NOTIMPLEMENTED;
	}

	// IOleInPlaceUIWindow
	STDMETHODIMP CMyFrame::GetBorder(LPRECT /*lprectBorder*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::RequestBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::SetBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::SetActiveObject(IOleInPlaceActiveObject* /*pActiveObject*/, LPCOLESTR /*pszObjName*/)
	{
		return S_OK;
	}

	// IOleInPlaceFrame
	STDMETHODIMP CMyFrame::InsertMenus(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::SetMenu(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
	{
		return S_OK;
	}

	STDMETHODIMP CMyFrame::RemoveMenus(HMENU /*hmenuShared*/)
	{
		NOTIMPLEMENTED;
	}

	STDMETHODIMP CMyFrame::SetStatusText(LPCOLESTR /*pszStatusText*/)
	{
		return S_OK;
	}

	STDMETHODIMP CMyFrame::EnableModeless(BOOL /*fEnable*/)
	{
		return S_OK;
	}

	STDMETHODIMP CMyFrame::TranslateAccelerator(LPMSG /*lpmsg*/, WORD /*wID*/)
	{
		NOTIMPLEMENTED;
	}



	webhostwnd::webhostwnd()
		: mpWebObject(NULL)
	{
		site.host = this;
		frame.host = this;
		hwnd = NULL;
	}

	webhostwnd::~webhostwnd()
	{
	}

	HWND webhostwnd::operator =(webhostwnd* /*rhs*/)
	{
		return hwnd;
	}

	webhostwnd* webhostwnd::Create(HINSTANCE hAppInstance)
	{
		webhostwnd* _this = new webhostwnd;

		_this->hwnd = CreateWindowEx(
			0,
			szClassName,TEXT("TheTestWindow"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,0,CW_USEDEFAULT,0,
			HWND_DESKTOP,NULL,hAppInstance,_this);

		//DWORD errCode = GetLastError();

		return _this;
	}


	BOOL webhostwnd::HandleMessage(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, LRESULT* /*r*/)
	{
		if(uMsg == WM_DESTROY)
		{
			UnCreateEmbeddedWebControl();
			PostQuitMessage(0);
			return TRUE;
		}
		else if(uMsg == WM_CREATE)
		{
			CreateEmbeddedWebControl(TEXT("about:blank"));
			return TRUE;
		}

		return FALSE;
	}


	void webhostwnd::CreateEmbeddedWebControl(const wchar_t* URL)
	{
		HRESULT hrResult = OleCreate(CLSID_WebBrowser,IID_IOleObject,OLERENDER_DRAW,0, &site, &storage,(void**)&mpWebObject);

		if( FAILED(hrResult) )
			return;

		LPCOLESTR wszContainerApp = L"Web Host";
		LPCOLESTR wszContainerObj = L"Web View";

		mpWebObject->SetHostNames(wszContainerApp, wszContainerObj);

		// I have no idea why this is necessary. remark it out and everything works perfectly.
		OleSetContainedObject(mpWebObject,TRUE);

		RECT rect;
		GetClientRect(hwnd,&rect);

		mpWebObject->DoVerb(OLEIVERB_SHOW,NULL,&site,-1,hwnd,&rect);

		IWebBrowser2* pBrowser2;

		hrResult = mpWebObject->QueryInterface( IID_IWebBrowser2,(void**)&pBrowser2 );
		if( FAILED(hrResult) )
			return;

		VARIANT vURL;
		vURL.vt = VT_BSTR;
		vURL.bstrVal = SysAllocString(URL);
		VARIANT ve1, ve2, ve3, ve4;
		ve1.vt = VT_EMPTY;
		ve2.vt = VT_EMPTY;
		ve3.vt = VT_EMPTY;
		ve4.vt = VT_EMPTY;

		pBrowser2->put_Resizable(VARIANT_FALSE);
		pBrowser2->put_Left(0);
		pBrowser2->put_Top(0);
		pBrowser2->put_Width(rect.right);
		pBrowser2->put_Height(rect.bottom);  

		pBrowser2->Navigate2(&vURL, &ve1, &ve2, &ve3, &ve4);

		VariantClear(&vURL);

		pBrowser2->Release();
	}


	void webhostwnd::UnCreateEmbeddedWebControl(void)
	{
		mpWebObject->Close(OLECLOSE_NOSAVE);
		mpWebObject->Release();
	}

	void webhostwnd::SetSize(UINT width, UINT height)
	{
		RECT rect;
		GetClientRect(hwnd,&rect);	

		ATL::CComPtr<IOleWindow> oleWindow;
		mpWebObject->QueryInterface(&oleWindow);

		HWND hWndBrowser;
		oleWindow->GetWindow(&hWndBrowser);

		::MoveWindow(hWndBrowser, 0, 0, width, height, TRUE);
		

		//IWebBrowser2* iBrowser;
		//HRESULT Result = mpWebObject->QueryInterface(IID_IWebBrowser2,(void**)&iBrowser);

		//VARIANT_BOOL b;
		//iBrowser->get_Resizable(&b);
		//iBrowser->put_Resizable(VARIANT_TRUE);
		//iBrowser->put_Left(0);
		//iBrowser->put_Top(0);
		//iBrowser->put_Width(rect.right);
		//iBrowser->put_Height(rect.bottom);
		//iBrowser->UpdateWindow();
		//iBrowser->Release();
	}

	void webhostwnd::Navigate(const WCHAR* URL)
	{
		HRESULT hr;

		IWebBrowser2* pBrowser2;
		hr = mpWebObject->QueryInterface(IID_IWebBrowser2,(void**)&pBrowser2);

		VARIANT vURL;
		vURL.vt = VT_BSTR;
		vURL.bstrVal = SysAllocString(URL); //"about:blank");
		VARIANT ve1, ve2, ve3, ve4;
		ve1.vt = VT_EMPTY;
		ve2.vt = VT_EMPTY;
		ve3.vt = VT_EMPTY;
		ve4.vt = VT_EMPTY;

		pBrowser2->put_Top(0);
		pBrowser2->Navigate2(&vURL, &ve1, &ve2, &ve3, &ve4);

		VariantClear(&vURL);


	}

} // namespace end

CWebControlImpl::CWebControlImpl(HWND hWnd, const wxString& URL)
{
	m_wnd = new webhostwnd();
	m_wnd->hwnd = hWnd;
	m_wnd->CreateEmbeddedWebControl(URL);
}

CWebControlImpl::~CWebControlImpl()
{
	if(m_wnd)
		m_wnd->UnCreateEmbeddedWebControl();
	MA_SAFE_DELETE(m_wnd);
}

void CWebControlImpl::SetSize(UINT Width, UINT Height)
{
	m_wnd->SetSize(Width, Height);
}

void CWebControlImpl::Navigate(EL_C_WCHAR_P URL)
{
	m_wnd->Navigate(URL);
}