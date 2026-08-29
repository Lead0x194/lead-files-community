#pragma once

#include "GrpBase.h"
#include "GrpDetector.h"
#include "StateManager.h"

class CGraphicDevice : public CGraphicBase
{
public:

	enum ECreateReturnValues
	{
		CREATE_OK				= (1 << 0),
		CREATE_NO_DIRECTX		= (1 << 1),
		CREATE_GET_DEVICE_CAPS	= (1 << 2),
		CREATE_GET_DEVICE_CAPS2 = (1 << 3),
		CREATE_DEVICE			= (1 << 4),
		CREATE_REFRESHRATE		= (1 << 5),
		CREATE_ENUM				= (1 << 6), // 2003. 01. 09. Failed to get myevan mode list
		CREATE_DETECT			= (1 << 7), // 2003. 01. 09. myevan mode selection failed
		CREATE_NO_TNL			= (1 << 8),
		CREATE_BAD_DRIVER		= (1 << 9),
		CREATE_FORMAT			= (1 << 10),
	};

	CGraphicDevice();
	virtual ~CGraphicDevice();

	// Renderer backend selection seam: the DX12 backend plugs in here;
	// until it exists every request resolves to DX9.
	enum EBackend
	{
		BACKEND_DX9,
		BACKEND_DX12,
	};

	static void		SetRequestedBackend(EBackend eBackend);
	static EBackend	GetBackend();

	void			InitBackBufferCount(UINT uBackBufferCount);

	void			Destroy();
	int				Create(HWND hWnd, int hres, int vres, bool Windowed = true, int bit = 32, int ReflashRate = 0);

	void			EnableWebBrowserMode(const RECT& c_rcWebPage);		
	void			DisableWebBrowserMode();
	void			MoveWebBrowserRect(const RECT& c_rcWebPage);

	bool			ResizeBackBuffer(UINT uWidth, UINT uHeight);
	void			RegisterWarningString(UINT uiMsg, const char * c_szString);

protected:
	void __Initialize();
	bool __IsInDriverBlackList(D3D_CAdapterInfo& rkD3DAdapterInfo);
	void __WarningMessage(HWND hWnd, UINT uiMsg);

	void __InitializeDefaultIndexBufferList();
	void __DestroyDefaultIndexBufferList();	
	bool __CreateDefaultIndexBufferList();
	bool __CreateDefaultIndexBuffer(UINT eDefIB, UINT uIdxCount, const WORD* c_awIndices);

	void __InitializePDTVertexBufferList();
	void __DestroyPDTVertexBufferList();
	bool __CreatePDTVertexBufferList();

	LPDIRECT3DVERTEXDECLARATION9 CreatePTStreamVertexDeclaration();
	LPDIRECT3DVERTEXDECLARATION9 CreatePNTStreamVertexDeclaration();
	LPDIRECT3DVERTEXDECLARATION9 CreatePNT2StreamVertexDeclaration();
	LPDIRECT3DVERTEXDECLARATION9 CreateDoublePNTStreamVertexDeclaration();

protected:
	DWORD						m_uBackBufferCount;
	std::map<UINT, std::string>	m_kMap_strWarningMessage;
	CStateManager*				m_pStateManager;
};
