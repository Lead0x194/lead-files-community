#include "StdAfx.h"
#include "GrpDevice.h"
#include "../eterBase/Stl.h"
#include "../eterBase/Debug.h"

bool GRAPHICS_CAPS_CAN_NOT_DRAW_LINE = false;
bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = false;
bool GRAPHICS_CAPS_HALF_SIZE_IMAGE = false;
bool GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER = false;
bool GRAPHICS_CAPS_SOFTWARE_TILING = false;

D3DPRESENT_PARAMETERS g_kD3DPP;
bool g_isBrowserMode=false;
RECT g_rcBrowser;

CGraphicDevice::CGraphicDevice()
: m_uBackBufferCount(0)
{
	__Initialize();
}

CGraphicDevice::~CGraphicDevice()
{
	Destroy();
}

void CGraphicDevice::__Initialize()
{
	ms_iD3DAdapterInfo=D3DADAPTER_DEFAULT;
	ms_iD3DDevInfo=D3DADAPTER_DEFAULT;
	ms_iD3DModeInfo=D3DADAPTER_DEFAULT;

	ms_lpd3d			= NULL;
	ms_lpd3dDevice		= NULL;
	ms_lpd3dMatStack	= NULL;

	ms_dwWavingEndTime = 0;
	ms_dwFlashingEndTime = 0;

	m_pStateManager		= NULL;

	__InitializeDefaultIndexBufferList();
	__InitializePDTVertexBufferList();
}

void CGraphicDevice::RegisterWarningString(UINT uiMsg, const char * c_szString)
{
	m_kMap_strWarningMessage[uiMsg] = c_szString;
}

void CGraphicDevice::__WarningMessage(HWND hWnd, UINT uiMsg)
{
	if (m_kMap_strWarningMessage.end() == m_kMap_strWarningMessage.find(uiMsg))
		return;
	MessageBox(hWnd, m_kMap_strWarningMessage[uiMsg].c_str(), "Warning", MB_OK|MB_TOPMOST);
}

void CGraphicDevice::MoveWebBrowserRect(const RECT& c_rcWebPage)
{
	g_rcBrowser=c_rcWebPage;
}

void CGraphicDevice::EnableWebBrowserMode(const RECT& c_rcWebPage)
{
	if (!ms_lpd3dDevice)
		return;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	
	g_isBrowserMode=true;

	if (D3DSWAPEFFECT_COPY==rkD3DPP.SwapEffect)
		return;

	g_kD3DPP=rkD3DPP;
	g_rcBrowser=c_rcWebPage;
	
	//rkD3DPP.Windowed=TRUE;
	rkD3DPP.SwapEffect=D3DSWAPEFFECT_COPY;
	rkD3DPP.BackBufferCount = 1;
	rkD3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	IDirect3DDevice9Ex& rkD3DDev=*ms_lpd3dDevice;
	HRESULT hr=rkD3DDev.ResetEx(&rkD3DPP, NULL);
	if (FAILED(hr))
		return;
	
	STATEMANAGER.SetDefaultState();	
}

void CGraphicDevice::DisableWebBrowserMode()
{
	if (!ms_lpd3dDevice)
		return;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	
	g_isBrowserMode=false;

	rkD3DPP=g_kD3DPP;

	IDirect3DDevice9Ex& rkD3DDev=*ms_lpd3dDevice;
	HRESULT hr=rkD3DDev.ResetEx(&rkD3DPP, NULL);
	if (FAILED(hr))
		return;
	
	STATEMANAGER.SetDefaultState();	
}
		
bool CGraphicDevice::ResizeBackBuffer(UINT uWidth, UINT uHeight)
{
	if (!ms_lpd3dDevice)
		return false;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	if (rkD3DPP.Windowed)
	{
		if (rkD3DPP.BackBufferWidth!=uWidth || rkD3DPP.BackBufferHeight!=uHeight)
		{
			rkD3DPP.BackBufferWidth=uWidth;
			rkD3DPP.BackBufferHeight=uHeight;

			IDirect3DDevice9Ex& rkD3DDev=*ms_lpd3dDevice;

			HRESULT hr=rkD3DDev.ResetEx(&rkD3DPP, NULL);
			if (FAILED(hr))
			{
				return false;
			}

			STATEMANAGER.SetDefaultState();
		}
	}

	return true;
}

LPDIRECT3DVERTEXDECLARATION9 CGraphicDevice::CreatePNTStreamVertexDeclaration()
{
	assert(ms_lpd3dDevice != NULL);

	LPDIRECT3DVERTEXDECLARATION9 dwShader = NULL;

	D3DVERTEXELEMENT9 pShaderDecl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (ms_lpd3dDevice->CreateVertexDeclaration(pShaderDecl, &dwShader) != D3D_OK)
	{
		char szError[1024];
		sprintf_s(szError, sizeof(szError), "Failed to create CreatePNTStreamVertexDeclaration");
		MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
	}

	return dwShader;
}

LPDIRECT3DVERTEXDECLARATION9 CGraphicDevice::CreatePNT2StreamVertexDeclaration()
{
	assert(ms_lpd3dDevice != NULL);

	LPDIRECT3DVERTEXDECLARATION9 dwShader = NULL;

	D3DVERTEXELEMENT9 pShaderDecl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};

	if (ms_lpd3dDevice->CreateVertexDeclaration(pShaderDecl, &dwShader) != D3D_OK)
	{
		char szError[1024];
		sprintf_s(szError, sizeof(szError), "Failed to create CreatePNT2StreamVertexDeclaration");
		MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
	}

	return dwShader;
}

LPDIRECT3DVERTEXDECLARATION9 CGraphicDevice::CreatePTStreamVertexDeclaration()
{
	assert(ms_lpd3dDevice != NULL);

	LPDIRECT3DVERTEXDECLARATION9 dwShader = NULL;

	D3DVERTEXELEMENT9 pShaderDecl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (ms_lpd3dDevice->CreateVertexDeclaration(pShaderDecl, &dwShader) != D3D_OK)
	{
		char szError[1024];
		sprintf_s(szError, sizeof(szError), "Failed to create CreatePTStreamVertexDeclaration");
		MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
	}

	return dwShader;
}

LPDIRECT3DVERTEXDECLARATION9 CGraphicDevice::CreateDoublePNTStreamVertexDeclaration()
{
	assert(ms_lpd3dDevice != NULL);

	LPDIRECT3DVERTEXDECLARATION9 dwShader = NULL;

	D3DVERTEXELEMENT9 pShaderDecl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
		{ 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 1, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};

	if (ms_lpd3dDevice->CreateVertexDeclaration(pShaderDecl, &dwShader) != D3D_OK)
	{
		char szError[1024];
		sprintf_s(szError, sizeof(szError), "Failed to create CreateDoublePNTStreamVertexDeclaration");
		MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
	}

	return dwShader;
}


static LPDIRECT3DSURFACE9 s_lpStencil;
static DWORD   s_MaxTextureWidth, s_MaxTextureHeight;

BOOL EL3D_ConfirmDevice(D3DCAPS9& rkD3DCaps, UINT uBehavior, D3DFORMAT /*eD3DFmt*/)
{
	// PUREDEVICE does not work with GetTransform / GetViewport, etc.
	if (uBehavior & D3DCREATE_PUREDEVICE) 
        return FALSE;
	
	if (uBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) 
	{	
		// DirectionalLight
		if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
			return FALSE;
		
		// PositionalLight
		if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
			return FALSE;

		// Software T&L Support - ATI NOT SUPPORT CLIP, USE DIRECTX SOFTWARE PROCESSING CLIPPING
		if (GRAPHICS_CAPS_SOFTWARE_TILING)
		{
			if (!(rkD3DCaps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
				return FALSE;
		}
		else
		{
			// Shadow/Terrain
			if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
				return FALSE;
		}
	}

	s_MaxTextureWidth = rkD3DCaps.MaxTextureWidth;
	s_MaxTextureHeight = rkD3DCaps.MaxTextureHeight;
	
	return TRUE;
}

DWORD GetMaxTextureWidth()
{
	return s_MaxTextureWidth;
}

DWORD GetMaxTextureHeight()
{
	return s_MaxTextureHeight;
}

bool CGraphicDevice::__IsInDriverBlackList(D3D_CAdapterInfo& rkD3DAdapterInfo)
{
	D3DADAPTER_IDENTIFIER9& d3dAdapterIdentifier=rkD3DAdapterInfo.GetIdentifier();

	char szSrcDriver[256];
	strncpy_s(szSrcDriver, sizeof(szSrcDriver), d3dAdapterIdentifier.Driver, _TRUNCATE);
	DWORD dwSrcHighVersion=d3dAdapterIdentifier.DriverVersion.QuadPart>>32;
	DWORD dwSrcLowVersion=d3dAdapterIdentifier.DriverVersion.QuadPart&0xffffffff;

	bool ret=false;
		
	FILE* fp=NULL;
	fopen_s(&fp, "grpblk.txt", "r");
	if (fp)
	{
		DWORD dwChkHighVersion;
		DWORD dwChkLowVersion;

		char szChkDriver[256];

		char szLine[256];
		while (fgets(szLine, sizeof(szLine)-1, fp))
		{			
			sscanf_s(szLine, "%s %x %x", szChkDriver, (unsigned)sizeof(szChkDriver), &dwChkHighVersion, &dwChkLowVersion);
			
			if (strcmp(szSrcDriver, szChkDriver)==0)
				if (dwSrcHighVersion==dwChkHighVersion)
					if (dwSrcLowVersion==dwChkLowVersion)
					{
						ret=true;				
						break;
					}

			szLine[0]='\0';
		}
		fclose(fp);
	}

	return ret;
}

static CGraphicDevice::EBackend gs_eRequestedBackend = CGraphicDevice::BACKEND_DX9;
static CGraphicDevice::EBackend gs_eActiveBackend = CGraphicDevice::BACKEND_DX9;

void CGraphicDevice::SetRequestedBackend(EBackend eBackend)
{
	gs_eRequestedBackend = eBackend;
}

CGraphicDevice::EBackend CGraphicDevice::GetBackend()
{
	return gs_eActiveBackend;
}

int CGraphicDevice::Create(HWND hWnd, int iHres, int iVres, bool Windowed, int /*iBit*/, int iReflashRate)
{
	if (BACKEND_DX12 == gs_eRequestedBackend)
	{
		// The DX12 backend is not implemented yet; stay on DX9 so a stray
		// config entry can never leave the player without a device.
		TraceError("RENDERER dx12 requested but the DX12 backend is not available yet; using dx9.");
	}
	gs_eActiveBackend = BACKEND_DX9;

	int iRet = CREATE_OK;

	Destroy();

	ms_iWidth	= iHres;
	ms_iHeight	= iVres;

	ms_hWnd		= hWnd;
	ms_hDC		= GetDC(hWnd);
	Direct3DCreate9Ex(D3D_SDK_VERSION, &ms_lpd3d);

		if (!ms_lpd3d)
		return CREATE_NO_DIRECTX;

	D3DADAPTER_IDENTIFIER9 d3dAdapterId;
	ms_lpd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &d3dAdapterId);

	D3DDISPLAYMODEEX d3dDisplayMode;
	D3DDISPLAYROTATION d3dRotation = D3DDISPLAYROTATION_IDENTITY;
	ZeroMemory(&d3dDisplayMode, sizeof(d3dDisplayMode));
	d3dDisplayMode.Size = sizeof(D3DDISPLAYMODEEX);
	ms_lpd3d->GetAdapterDisplayModeEx(D3DADAPTER_DEFAULT, &d3dDisplayMode, &d3dRotation);

	if (Windowed &&
		strnicmp(d3dAdapterId.Driver, "3dfx", 4) == 0 &&
		D3DFMT_X8R8G8B8 == d3dDisplayMode.Format)
	{
		return CREATE_FORMAT;
	}

	D3DCAPS9 d3dCaps;
	ms_lpd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);

	BOOL isFormatConfirmed = FALSE;
	if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		if (d3dCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
		{
			ms_dwD3DBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
			isFormatConfirmed = EL3D_ConfirmDevice(d3dCaps, ms_dwD3DBehavior, d3dDisplayMode.Format);
		}

		if (!isFormatConfirmed)
		{
			ms_dwD3DBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			isFormatConfirmed = EL3D_ConfirmDevice(d3dCaps, ms_dwD3DBehavior, d3dDisplayMode.Format);
		}

		if (!isFormatConfirmed)
		{
			ms_dwD3DBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
			isFormatConfirmed = EL3D_ConfirmDevice(d3dCaps, ms_dwD3DBehavior, d3dDisplayMode.Format);
		}
	}

	if (!isFormatConfirmed)
	{
		ms_dwD3DBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		isFormatConfirmed = EL3D_ConfirmDevice(d3dCaps, ms_dwD3DBehavior, d3dDisplayMode.Format);
		iRet |= CREATE_NO_TNL;
	}

	int ErrorCorrection = 0;
	bool disableMSAA = false;

RETRY:
	ZeroMemory(&ms_d3dPresentParameter, sizeof(ms_d3dPresentParameter));
	
	ms_d3dPresentParameter.Windowed						= Windowed;
	ms_d3dPresentParameter.BackBufferWidth					= iHres;
	ms_d3dPresentParameter.BackBufferHeight				= iVres;
	ms_d3dPresentParameter.hDeviceWindow					= hWnd;
	ms_d3dPresentParameter.BackBufferFormat				= Windowed ? D3DFMT_UNKNOWN : d3dDisplayMode.Format;
	ms_d3dPresentParameter.BackBufferCount					= m_uBackBufferCount;
	ms_d3dPresentParameter.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	ms_d3dPresentParameter.MultiSampleType					= D3DMULTISAMPLE_NONE;

	if (Windowed)
	{
		ms_d3dPresentParameter.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;
		ms_d3dPresentParameter.FullScreen_RefreshRateInHz	= 0;
	}
	else
	{
		ms_d3dPresentParameter.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;
		ms_d3dPresentParameter.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	}

	ms_d3dPresentParameter.EnableAutoDepthStencil			= TRUE;
	ms_d3dPresentParameter.AutoDepthStencilFormat			= D3DFMT_D24S8;

	if (!Windowed && !disableMSAA)
	{
		D3DFORMAT msaaCheckFormat = ms_d3dPresentParameter.BackBufferFormat;
		if (SUCCEEDED(ms_lpd3d->CheckDeviceMultiSampleType(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			msaaCheckFormat,
			FALSE,
			D3DMULTISAMPLE_2_SAMPLES,
			&ms_d3dPresentParameter.MultiSampleQuality)))
		{
			ms_d3dPresentParameter.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			ms_d3dPresentParameter.MultiSampleQuality = 0;
		}

		if (SUCCEEDED(ms_lpd3d->CheckDeviceMultiSampleType(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			msaaCheckFormat,
			FALSE,
			D3DMULTISAMPLE_4_SAMPLES,
			&ms_d3dPresentParameter.MultiSampleQuality)))
		{
			ms_d3dPresentParameter.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
			ms_d3dPresentParameter.MultiSampleQuality = 0;
		}
	}

	D3DDISPLAYMODEEX displayModeEx;
	ZeroMemory(&displayModeEx, sizeof(displayModeEx));
	displayModeEx.Size = sizeof(D3DDISPLAYMODEEX);
	displayModeEx.Width = iHres;
	displayModeEx.Height = iVres;
	displayModeEx.RefreshRate = iReflashRate;
	displayModeEx.Format = d3dDisplayMode.Format;
	displayModeEx.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

	D3DDISPLAYMODEEX* pDisplayMode = Windowed ? NULL : &displayModeEx;

	if (FAILED(ms_hLastResult = ms_lpd3d->CreateDeviceEx(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hWnd,
				// 2004. 1. 9 myevan Vertex processing method automatic selection added
				ms_dwD3DBehavior,
				&ms_d3dPresentParameter,
				pDisplayMode,
				&ms_lpd3dDevice)))
	{
		switch (ms_hLastResult)
		{
			case D3DERR_INVALIDCALL:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_INVALIDCALL\nThe method call is invalid. For example, a method's parameter may have an invalid value.");					
				break;
			case D3DERR_NOTAVAILABLE:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_NOTAVAILABLE\nThis device does not support the queried technique. ");
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_OUTOFVIDEOMEMORY\nDirect3D does not have enough display memory to perform the operation");
				break;
			default:
				Tracenf("IDirect3DDevice.CreateDevice - ERROR %d", ms_hLastResult);
				break;
		}

		if (ErrorCorrection)
			return CREATE_DEVICE;

		disableMSAA = true;
		// 2004. 1. 9 myevan It seems to be a meaningless code... If an error occurs, display it and exit.
		iReflashRate = 0;
		++ErrorCorrection;
		iRet = CREATE_REFRESHRATE;
		goto RETRY;
	}

	// Check DXT Support Info
	const D3DFORMAT baseFormatForTextureCheck = Windowed ? d3dDisplayMode.Format : ms_d3dPresentParameter.BackBufferFormat;

	if(ms_lpd3d->CheckDeviceFormat(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL,
				baseFormatForTextureCheck,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT1) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}

	if(ms_lpd3d->CheckDeviceFormat(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL,
				baseFormatForTextureCheck,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT3) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}

	if(ms_lpd3d->CheckDeviceFormat(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL,
				baseFormatForTextureCheck,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT5) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}	

	if (FAILED((ms_hLastResult = ms_lpd3dDevice->GetDeviceCaps(&ms_d3dCaps))))
	{
		Tracenf("IDirect3DDevice.GetDeviceCaps - ERROR %d", ms_hLastResult);
		return CREATE_GET_DEVICE_CAPS2;
	}

	if (!Windowed)
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, iHres, iVres, SWP_SHOWWINDOW);

	//Tracef("vertex shader version : %X\n",(DWORD)ms_d3dCaps.VertexShaderVersion);

	ms_lpd3dDevice->GetViewport(&ms_Viewport);

	m_pStateManager = new CStateManager(ms_lpd3dDevice);

	D3DXCreateMatrixStack(0, &ms_lpd3dMatStack);
	ms_lpd3dMatStack->LoadIdentity();

	ms_ptDecl	= CreatePTStreamVertexDeclaration();
	ms_pntDecl = CreatePNTStreamVertexDeclaration();
	ms_pnt2Decl = CreatePNT2StreamVertexDeclaration();

	D3DXMatrixIdentity(&ms_matIdentity);
	D3DXMatrixIdentity(&ms_matView);
	D3DXMatrixIdentity(&ms_matProj);
	D3DXMatrixIdentity(&ms_matInverseView);
	D3DXMatrixIdentity(&ms_matInverseViewYAxis);
	D3DXMatrixIdentity(&ms_matScreen0);
	D3DXMatrixIdentity(&ms_matScreen1);
	D3DXMatrixIdentity(&ms_matScreen2);

	ms_matScreen0._11 = 1;
	ms_matScreen0._22 = -1;	

	ms_matScreen1._41 = 1;
	ms_matScreen1._42 = 1;

	ms_matScreen2._11 = (float) iHres / 2;
	ms_matScreen2._22 = (float) iVres / 2;
	
	D3DXCreateSphere(ms_lpd3dDevice, 1.0f, 32, 32, &ms_lpSphereMesh, NULL);
	D3DXCreateCylinder(ms_lpd3dDevice, 1.0f, 1.0f, 1.0f, 8, 8, &ms_lpCylinderMesh, NULL);

	ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

	if (!__CreateDefaultIndexBufferList())
		return false;

	if (!__CreatePDTVertexBufferList())
		return false;
	
	DWORD dwTexMemSize = GetAvailableTextureMemory();

	if (dwTexMemSize < 64 * 1024 * 1024)
		ms_isLowTextureMemory = true;
	else
		ms_isLowTextureMemory = false;

	if (dwTexMemSize > 100 * 1024 * 1024)
		ms_isHighTextureMemory = true;
	else
		ms_isHighTextureMemory = false;

	if (ms_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
		GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER=false;
	else
		GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER=true;

	//D3DADAPTER_IDENTIFIER8& d3dAdapterId=pkD3DAdapterInfo->GetIdentifier();
	if (strnicmp(d3dAdapterId.Driver, "SIS", 3) == 0)
	{
		GRAPHICS_CAPS_CAN_NOT_DRAW_LINE = true;
		GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = true;
		GRAPHICS_CAPS_HALF_SIZE_IMAGE = true;
		ms_isLowTextureMemory = true;
	}
	else if (strnicmp(d3dAdapterId.Driver, "3dfx", 4) == 0)
	{
		GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = true;
		GRAPHICS_CAPS_HALF_SIZE_IMAGE = true;
		ms_isLowTextureMemory = true;
	}

	return (iRet);
}

void CGraphicDevice::__InitializePDTVertexBufferList()
{
	ms_smallPdtVertexBuffer = NULL;
	ms_largePdtVertexBuffer = NULL;
}
		
void CGraphicDevice::__DestroyPDTVertexBufferList()
{
	if (ms_smallPdtVertexBuffer)
	{
		ms_smallPdtVertexBuffer->Release();
		ms_smallPdtVertexBuffer = NULL;
	}

	if (ms_largePdtVertexBuffer)
	{
		ms_largePdtVertexBuffer->Release();
		ms_largePdtVertexBuffer = NULL;
	}
}

bool CGraphicDevice::__CreatePDTVertexBufferList()
{
	HRESULT hr = ms_lpd3dDevice->CreateVertexBuffer(
		sizeof(TPDTVertex) * SMALL_PDT_VERTEX_BUFFER_SIZE,
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
		D3DPOOL_DEFAULT,
		&ms_smallPdtVertexBuffer, NULL);

	if (FAILED(hr))
		return false;

	hr = ms_lpd3dDevice->CreateVertexBuffer(
		sizeof(TPDTVertex) * LARGE_PDT_VERTEX_BUFFER_SIZE,
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
		D3DPOOL_DEFAULT,
		&ms_largePdtVertexBuffer, NULL);

	if (FAILED(hr))
		return false;

	return true;
}

void CGraphicDevice::__InitializeDefaultIndexBufferList()
{
	for (UINT i=0; i<DEFAULT_IB_NUM; ++i)
		ms_alpd3dDefIB[i]=NULL;
}

void CGraphicDevice::__DestroyDefaultIndexBufferList()
{
	for (UINT i=0; i<DEFAULT_IB_NUM; ++i)
		if (ms_alpd3dDefIB[i])
		{
			ms_alpd3dDefIB[i]->Release();
			ms_alpd3dDefIB[i]=NULL;
		}	
}

bool CGraphicDevice::__CreateDefaultIndexBuffer(UINT eDefIB, UINT uIdxCount, const WORD* c_awIndices)
{
	assert(ms_alpd3dDefIB[eDefIB]==NULL);


	auto hr = ms_lpd3dDevice->CreateIndexBuffer(
		sizeof(WORD)*uIdxCount,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&ms_alpd3dDefIB[eDefIB], nullptr
	);
	if (FAILED(hr))
		return false;

	WORD* dstIndices;

	hr = ms_alpd3dDefIB[eDefIB]->Lock(0, 0, (void**)&dstIndices, 0);
	if (FAILED(hr))
		return false;

	memcpy(dstIndices, c_awIndices, sizeof(WORD)*uIdxCount);

	ms_alpd3dDefIB[eDefIB]->Unlock();

	return true;
}

bool CGraphicDevice::__CreateDefaultIndexBufferList()
{
	static const WORD c_awLineIndices[2] = { 0, 1, };
	static const WORD c_awLineTriIndices[6] = { 0, 1, 0, 2, 1, 2, };
	static const WORD c_awLineRectIndices[8] = { 0, 1, 0, 2, 1, 3, 2, 3,};
	static const WORD c_awLineCubeIndices[24] = { 
		0, 1, 0, 2, 1, 3, 2, 3,
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 4, 6, 5, 7, 6, 7,
	};
	static const WORD c_awFillTriIndices[3]= { 0, 1, 2, };
	static const WORD c_awFillRectIndices[6] = { 0, 2, 1, 2, 3, 1, };
	static const WORD c_awFillCubeIndices[36] = { 
		0, 1, 2, 1, 3, 2,
		2, 0, 6, 0, 4, 6,
		0, 1, 4, 1, 5, 4,
		1, 3, 5, 3, 7, 5,
		3, 2, 7, 2, 6, 7,
		4, 5, 6, 5, 7, 6,
	};
	
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE, 2, c_awLineIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_TRI, 6, c_awLineTriIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_RECT, 8, c_awLineRectIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_CUBE, 24, c_awLineCubeIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_TRI, 3, c_awFillTriIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_RECT, 6, c_awFillRectIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_CUBE, 36, c_awFillCubeIndices))
		return false;
	
	return true;
}

void CGraphicDevice::InitBackBufferCount(UINT uBackBufferCount)
{
	m_uBackBufferCount=uBackBufferCount;
}

void CGraphicDevice::Destroy()
{
	__DestroyPDTVertexBufferList();
	__DestroyDefaultIndexBufferList();

	if (ms_hDC)
	{
		ReleaseDC(ms_hWnd, ms_hDC);
		ms_hDC = NULL;
	}

	if (ms_ptDecl)
	{	
		ms_ptDecl->Release();
		ms_ptDecl = 0;;
	}

	if (ms_pntDecl)
	{
		ms_pntDecl->Release();
		ms_pntDecl = 0;
	}

	if (ms_pnt2Decl)
	{
		ms_pnt2Decl->Release();
		ms_pnt2Decl = 0;
	}

	safe_release(ms_lpSphereMesh);
	safe_release(ms_lpCylinderMesh);

	safe_release(ms_lpd3dMatStack);
	safe_release(ms_lpd3dDevice);
	safe_release(ms_lpd3d);	

	if (m_pStateManager)
	{
		delete m_pStateManager;
		m_pStateManager = NULL;
	}

	__Initialize();
}
