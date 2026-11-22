#include "StdAfx.h"
#include "MsWindow.h"

#include <windowsx.h>

CMSWindow::TWindowClassSet CMSWindow::ms_stWCSet;
HINSTANCE CMSWindow::ms_hInstance = NULL;

LRESULT CALLBACK MSWindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{	
	CMSWindow * pWnd = (CMSWindow *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (pWnd)
		return pWnd->WindowProcedure(hWnd, uiMsg, wParam, lParam);	

	return DefWindowProcW(hWnd, uiMsg, wParam, lParam);
}

LRESULT CMSWindow::WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_SIZE:
			OnSize(wParam, lParam);
			break;

		case WM_ACTIVATEAPP:
			m_isActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			break;
	}

	return DefWindowProcW(hWnd, uiMsg, wParam, lParam);
}

void CMSWindow::OnSize(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam == SIZE_MINIMIZED) 
	{
		InvalidateRect(m_hWnd, NULL, true);
		m_isActive = false;        
		m_isVisible = false;
	}
	else
	{
		m_isActive = true;
		m_isVisible = true;
	}
}

void CMSWindow::Destroy()
{
	if (!m_hWnd)
		return;

	if (IsWindow(m_hWnd))
		DestroyWindow(m_hWnd);
	
	m_hWnd = NULL;
	m_isVisible = false;
}

bool CMSWindow::Create(const char* c_szName, int brush, DWORD cs, DWORD ws, HICON hIcon, int iCursorResource)
{
	//assert(ms_hInstance != NULL);
	Destroy();
		
	const char* c_szClassName = RegisterWindowClass(cs, brush, MSWindowProcedure, hIcon, iCursorResource);

	m_hWnd = CreateWindow(
						c_szClassName,
						c_szName,
						ws, 
						0, 0, 0, 0, 
						NULL,
						NULL, 
						ms_hInstance,
						NULL);

	if (!m_hWnd)
		return false;

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR) this);
	//DestroyWindow(ImmGetDefaultIMEWnd(m_hWnd));

	return true;
}

void CMSWindow::SetVisibleMode(bool isVisible)
{
	m_isVisible = isVisible;

	if (m_isVisible)
	{
		ShowWindow(m_hWnd, SW_SHOW);		
	}
	else
	{
		ShowWindow(m_hWnd, SW_HIDE);
	}	
}

void CMSWindow::Show()
{
	m_isVisible = true;
	ShowWindow(m_hWnd, SW_SHOW);
}

void CMSWindow::Hide()
{
	m_isVisible = false;
	ShowWindow(m_hWnd, SW_HIDE);
}

bool CMSWindow::IsVisible()
{
	return m_isVisible;
}

bool CMSWindow::IsActive()
{
	return m_isActive;
}

HINSTANCE CMSWindow::GetInstance()
{
	return ms_hInstance;
}

HWND CMSWindow::GetWindowHandle()
{
	return m_hWnd;
}

int	CMSWindow::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int	CMSWindow::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

static bool GetVisualWindowRect(HWND hwnd, RECT& outRect)
{
	if (!::GetWindowRect(hwnd, &outRect))
		return false;

	using DwmGetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
	constexpr DWORD ExtendedFrameBounds = 9;

	static HMODULE s_dwm = ::LoadLibraryA("dwmapi.dll");
	if (!s_dwm)
		return true;

	static auto s_getAttr =
		reinterpret_cast<DwmGetWindowAttributeFn>(
			::GetProcAddress(s_dwm, "DwmGetWindowAttribute"));

	if (!s_getAttr)
		return true;

	RECT visual{};
	if (SUCCEEDED(s_getAttr(hwnd, ExtendedFrameBounds, &visual, sizeof(visual))))
	{
		outRect = visual;
	}

	return true;
}

static POINT GetCenteredPosition(const RECT& visualRect, const RECT& workArea, const RECT& windowRect)
{
	const int width = visualRect.right - visualRect.left;
	const int height = visualRect.bottom - visualRect.top;

	const int dx = visualRect.left - windowRect.left;
	const int dy = visualRect.top - windowRect.top;

	POINT pt{};
	pt.x = workArea.left + ((workArea.right - workArea.left) - width) / 2 - dx;
	pt.y = workArea.top + ((workArea.bottom - workArea.top) - height) / 2 - dy;
	return pt;
}


void CMSWindow::GetClientRect(RECT* prc)
{
	::GetClientRect(m_hWnd, prc);
}

void CMSWindow::GetMousePosition(POINT* ppt)
{
	GetCursorPos(ppt);
	ScreenToClient(m_hWnd, ppt);
}

void CMSWindow::SetPosition(int x, int y)
{
	SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

void CMSWindow::SetCenterPosition()
{
	RECT window{};
	RECT visual{};
	RECT workArea{};

	if (!::GetWindowRect(m_hWnd, &window))
		return;

	visual = window;
	GetVisualWindowRect(m_hWnd, visual);

	::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	const POINT pos = GetCenteredPosition(visual, workArea, window);
	SetPosition(pos.x, pos.y);
}

void CMSWindow::AdjustSize(int width, int height)
{
	SetRect(&m_rect, 0, 0, width, height);

	AdjustWindowRectEx(&m_rect,
						GetWindowStyle(m_hWnd),     
						GetMenu(m_hWnd ) != NULL,    
						GetWindowExStyle(m_hWnd ) ); 

	MoveWindow
	( 
		m_hWnd, 
		0, 
		0, 
		m_rect.right - m_rect.left, 
		m_rect.bottom - m_rect.top, 
		FALSE
	);
}

void CMSWindow::SetText(const char* c_szText)
{
	SetWindowText(m_hWnd, c_szText);
}

void CMSWindow::SetSize(int width, int height)
{	
	SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOZORDER|SWP_NOMOVE);
}

const char * CMSWindow::RegisterWindowClass(DWORD style, int brush, WNDPROC pfnWndProc, HICON hIcon, int iCursorResource)
{
	char szClassName[1024];
	sprintf_s(szClassName, sizeof(szClassName), "eter - s%x:b%x:p:%Ix", style, brush, (uintptr_t) pfnWndProc);

	TWindowClassSet::iterator f = ms_stWCSet.find((char*) szClassName);

	if (f != ms_stWCSet.end())
		return *f;

	const char* c_szStaticClassName = stl_static_string(szClassName).c_str();

	ms_stWCSet.insert((char * const) c_szStaticClassName);
	
	const std::wstring wstrClassName(c_szStaticClassName, c_szStaticClassName + strlen(c_szStaticClassName));

	WNDCLASSW wc;

	wc.style			= 0;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= pfnWndProc;
	wc.hCursor			= LoadCursor(ms_hInstance, MAKEINTRESOURCE(iCursorResource));
	wc.hIcon			= hIcon ? hIcon : LoadIcon(ms_hInstance, IDI_APPLICATION);
	wc.hbrBackground	= (HBRUSH) GetStockObject(brush);
	wc.hInstance		= ms_hInstance;	
	wc.lpszClassName	= wstrClassName.c_str();
	wc.lpszMenuName		= L"";

	if (!RegisterClassW(&wc)) 
		return "";

	return c_szStaticClassName;
}

CMSWindow::CMSWindow()
{
	m_hWnd=NULL;
	m_isVisible=false;
}

CMSWindow::~CMSWindow()
{
}