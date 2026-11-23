#include "StdAfx.h"
#include "MsApplication.h"

CMSApplication::CMSApplication()
{
}

CMSApplication::~CMSApplication()
{
//	for (TWindowClassSet::iterator i=ms_stWCSet.begin(); i!=ms_stWCSet.end(); ++i)
//		UnregisterClass(*i, ms_hInstance);
}

void CMSApplication::Initialize(HINSTANCE hInstance)
{
	ms_hInstance = hInstance;
}

void CMSApplication::MessageLoop()
{
	while (MessageProcess());
}

bool CMSApplication::IsMessage()
{
	MSG msg;

	if (!PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
		return false;

	return true;
}

bool CMSApplication::MessageProcess()
{
	MSG msg;

	if (!GetMessageW(&msg, NULL, 0, 0))
		return false;

	TranslateMessage(&msg);
	DispatchMessageW(&msg);
	return true;
}

LRESULT CMSApplication::WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
	}

	return CMSWindow::WindowProcedure(hWnd, uiMsg, wParam, lParam);
}