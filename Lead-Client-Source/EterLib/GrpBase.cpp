#include "StdAfx.h"
#include "../eterBase/Utils.h"
#include "../eterBase/Timer.h"
#include "GrpBase.h"
#include "Camera.h"
#include "StateManager.h"
#include "GraphicShaderPool.h"

void PixelPositionToD3DXVECTOR3(const D3DXVECTOR3& c_rkPPosSrc, D3DXVECTOR3* pv3Dst)
{
	pv3Dst->x=+c_rkPPosSrc.x;
	pv3Dst->y=-c_rkPPosSrc.y;
	pv3Dst->z=+c_rkPPosSrc.z;
}

void D3DXVECTOR3ToPixelPosition(const D3DXVECTOR3& c_rv3Src, D3DXVECTOR3* pv3Dst)
{
	pv3Dst->x=+c_rv3Src.x;
	pv3Dst->y=-c_rv3Src.y;
	pv3Dst->z=+c_rv3Src.z;
}

UINT					CGraphicBase::ms_iD3DAdapterInfo=0;
UINT					CGraphicBase::ms_iD3DDevInfo=0;
UINT					CGraphicBase::ms_iD3DModeInfo=0;		
D3D_CDisplayModeAutoDetector				CGraphicBase::ms_kD3DDetector;

HWND CGraphicBase::ms_hWnd;
HDC CGraphicBase::ms_hDC;

LPDIRECT3D9EX				CGraphicBase::ms_lpd3d = NULL;
LPDIRECT3DDEVICE9EX		CGraphicBase::ms_lpd3dDevice = NULL;
ID3DXMatrixStack *		CGraphicBase::ms_lpd3dMatStack = NULL;
D3DPRESENT_PARAMETERS	CGraphicBase::ms_d3dPresentParameter = {};
D3DVIEWPORT9			CGraphicBase::ms_Viewport;

HRESULT					CGraphicBase::ms_hLastResult = NULL;
bool					CGraphicBase::ms_bUseShaderFFP = false;

int						CGraphicBase::ms_iWidth;
int						CGraphicBase::ms_iHeight;

DWORD					CGraphicBase::ms_faceCount = 0;

D3DCAPS9				CGraphicBase::ms_d3dCaps;

DWORD					CGraphicBase::ms_dwD3DBehavior = 0;

LPDIRECT3DVERTEXDECLARATION9 CGraphicBase::ms_ptDecl = 0;
LPDIRECT3DVERTEXDECLARATION9 CGraphicBase::ms_pntDecl = 0;
LPDIRECT3DVERTEXDECLARATION9 CGraphicBase::ms_pnt2Decl = 0;

D3DXMATRIX				CGraphicBase::ms_matIdentity;

D3DXMATRIX				CGraphicBase::ms_matView;
D3DXMATRIX				CGraphicBase::ms_matProj;
D3DXMATRIX				CGraphicBase::ms_matInverseView;
D3DXMATRIX				CGraphicBase::ms_matInverseViewYAxis;

D3DXMATRIX				CGraphicBase::ms_matWorld;
D3DXMATRIX				CGraphicBase::ms_matWorldView;

D3DXMATRIX				CGraphicBase::ms_matScreen0;
D3DXMATRIX				CGraphicBase::ms_matScreen1;
D3DXMATRIX				CGraphicBase::ms_matScreen2;

D3DXVECTOR3				CGraphicBase::ms_vtPickRayOrig;
D3DXVECTOR3				CGraphicBase::ms_vtPickRayDir;

float					CGraphicBase::ms_fFieldOfView;
float					CGraphicBase::ms_fNearY;
float					CGraphicBase::ms_fFarY;
float					CGraphicBase::ms_fAspect;

DWORD					CGraphicBase::ms_dwWavingEndTime;
int						CGraphicBase::ms_iWavingPower;
DWORD					CGraphicBase::ms_dwFlashingEndTime;
D3DXCOLOR				CGraphicBase::ms_FlashingColor;

// Ray for terrain picking... Version using CCamera... Requires integration with existing Ray...
CRay					CGraphicBase::ms_Ray;
bool					CGraphicBase::ms_bSupportDXT = true;
bool					CGraphicBase::ms_isLowTextureMemory = false;
bool					CGraphicBase::ms_isHighTextureMemory = false;

// 2004.11.18.myevan.Replaced with DynamicVertexBuffer
/*
std::vector<TIndex>		CGraphicBase::ms_lineIdxVector;
std::vector<TIndex>		CGraphicBase::ms_lineTriIdxVector;
std::vector<TIndex>		CGraphicBase::ms_lineRectIdxVector;
std::vector<TIndex>		CGraphicBase::ms_lineCubeIdxVector;

std::vector<TIndex>		CGraphicBase::ms_fillTriIdxVector;
std::vector<TIndex>		CGraphicBase::ms_fillRectIdxVector;
std::vector<TIndex>		CGraphicBase::ms_fillCubeIdxVector;
*/

LPD3DXMESH				CGraphicBase::ms_lpSphereMesh = NULL;
LPD3DXMESH				CGraphicBase::ms_lpCylinderMesh = NULL;

LPDIRECT3DVERTEXBUFFER9	CGraphicBase::ms_smallPdtVertexBuffer = NULL;
LPDIRECT3DVERTEXBUFFER9	CGraphicBase::ms_largePdtVertexBuffer = NULL;

LPDIRECT3DINDEXBUFFER9	CGraphicBase::ms_alpd3dDefIB[DEFAULT_IB_NUM];

bool CGraphicBase::IsLowTextureMemory()
{
	return ms_isLowTextureMemory;
}

bool CGraphicBase::IsHighTextureMemory()
{
	return ms_isHighTextureMemory;
}

bool CGraphicBase::IsFastTNL()
{ 
	if (ms_dwD3DBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ||
		ms_dwD3DBehavior & D3DCREATE_MIXED_VERTEXPROCESSING)
	{
		if (ms_d3dCaps.VertexShaderVersion>D3DVS_VERSION(1,0))
			return true;
	}
	return false;
}

bool CGraphicBase::IsTLVertexClipping()
{
	if (ms_d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS)
		return true;

	return false;
}

void CGraphicBase::GetBackBufferSize(UINT* puWidth, UINT* puHeight)
{
	*puWidth=ms_d3dPresentParameter.BackBufferWidth;
	*puHeight=ms_d3dPresentParameter.BackBufferHeight;
}

void CGraphicBase::SetDefaultIndexBuffer(UINT eDefIB)
{
	if (eDefIB>=DEFAULT_IB_NUM)
		return;

	STATEMANAGER.SetIndices(ms_alpd3dDefIB[eDefIB], 0);
}

bool CGraphicBase::SetPDTStream(SPDTVertex* pVertices, UINT uVtxCount)
{
	return SetPDTStream((SPDTVertexRaw*)pVertices, uVtxCount);
}

bool CGraphicBase::SetPDTStream(SPDTVertexRaw* pSrcVertices, UINT uVtxCount)
{
	if (!uVtxCount)
		return false;

	if (uVtxCount > LARGE_PDT_VERTEX_BUFFER_SIZE)
		return false;

	assert(uVtxCount <= LARGE_PDT_VERTEX_BUFFER_SIZE);

	IDirect3DVertexBuffer9* vb = NULL;

	if (uVtxCount <= SMALL_PDT_VERTEX_BUFFER_SIZE)
		vb = ms_smallPdtVertexBuffer;
	else
		vb = ms_largePdtVertexBuffer;

	if (!vb)
		return false;

	const UINT bytes = sizeof(TPDTVertex) * uVtxCount;

	TPDTVertex* pDstVertices;
	if (FAILED(vb->Lock(0, bytes, (void**)&pDstVertices, D3DLOCK_DISCARD)))
	{
		STATEMANAGER.SetStreamSource(0, NULL, 0);
		return false;
	}

	memcpy(pDstVertices, pSrcVertices, bytes);

	vb->Unlock();

	STATEMANAGER.SetStreamSource(0, vb, sizeof(TPDTVertex));

	return true;
}

static CGraphicShaderPool gs_kShaderPool;

void CGraphicBase::SetUseShaderFFP(bool bEnable)
{
	ms_bUseShaderFFP = bEnable;
}

bool CGraphicBase::IsUseShaderFFP()
{
	return ms_bUseShaderFFP;
}

bool CGraphicBase::BeginPDTShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPDTModulate();
}

bool CGraphicBase::BeginPDTDiffuseShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPDTDiffuse();
}

bool CGraphicBase::BeginPTTextureShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPTTexture();
}

bool CGraphicBase::BeginPDTTextureShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPDTTexture();
}

bool CGraphicBase::BeginPDTModulateTexAlphaShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPDTModulateTexAlpha();
}

bool CGraphicBase::BeginMiniMapShader(bool bTexture)
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindMiniMap(bTexture);
}

bool CGraphicBase::BeginPDTCloudShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	return gs_kShaderPool.BindPDTTexMatInvAlphaAdd();
}

bool CGraphicBase::BeginEffectShader(DWORD dwColorOp)
{
	if (!ms_bUseShaderFFP)
		return false;

	switch (dwColorOp)
	{
		case D3DTOP_MODULATE:
			return gs_kShaderPool.BindPTTFactorModulate();
		case D3DTOP_ADD:
			return gs_kShaderPool.BindPTTFactorAdd();
		case D3DTOP_SELECTARG1:
			return gs_kShaderPool.BindPTTFactorOnly();
		case D3DTOP_SELECTARG2:
			return gs_kShaderPool.BindPTTexTFactorAlpha();
		default:
			return false;	// uncommon combiner: keep the fixed-function path
	}
}


bool CGraphicBase::BeginGrannyMeshShader()
{
	if (!ms_bUseShaderFFP)
		return false;

	// Route by the bound stream layout first: granny characters/objects use PNT,
	// dungeon blocks PNT2. Anything else keeps the fixed-function path.
	DWORD dwValue;
	const UINT uStride = STATEMANAGER.GetStreamStride(0);
	if (sizeof(TPNT2Vertex) == uStride)
	{
		// Dungeon blocks are drawn twice: the lightmap pass and the character-shadow
		// receiver re-render. Both convert together with the same WVP math so the
		// two passes rasterize identical depths (mixed paths z-fight).
		STATEMANAGER.GetTextureStageState(0, D3DTSS_COLOROP, &dwValue);
		if (D3DTOP_SELECTARG1 != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(0, D3DTSS_COLORARG1, &dwValue);
		const DWORD dwColorArg1 = dwValue;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLOROP, &dwValue);
		if (D3DTOP_MODULATE != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG1, &dwValue);
		if (D3DTA_TEXTURE != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG2, &dwValue);
		if (D3DTA_CURRENT != dwValue)
			return false;
		LPDIRECT3DBASETEXTURE9 pkTexture;
		STATEMANAGER.GetTexture(1, &pkTexture);
		if (!pkTexture)
			return false;
		if (D3DTA_TEXTURE == dwColorArg1)
		{
			// Lightmap cascade: stage0 SELECTARG1(TEXTURE), stage1 MODULATE(lightmap, CURRENT).
			STATEMANAGER.GetTexture(0, &pkTexture);
			if (!pkTexture)
				return false;
			return gs_kShaderPool.BindPNT2Lightmap();
		}
		if (D3DTA_TFACTOR == dwColorArg1)
		{
			// Shadow receiver: white TFACTOR times the shadow map projected via
			// CAMERASPACEPOSITION texgen, multiplied into the scene by the blend.
			STATEMANAGER.GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &dwValue);
			if (D3DTSS_TCI_CAMERASPACEPOSITION != dwValue)
				return false;
			return gs_kShaderPool.BindPNT2ShadowReceiver();
		}
		return false;
	}
	if (sizeof(TPNTVertex) != uStride)
		return false;

	// Only the base cascade is converted: stage 0 MODULATE(TEXTURE, DIFFUSE) for
	// color and alpha, stage 1 disabled, a texture bound. Specular, fades and
	// two-texture materials keep the fixed-function path for now.
	STATEMANAGER.GetTextureStageState(0, D3DTSS_COLOROP, &dwValue);
	if (D3DTOP_MODULATE != dwValue)
		return false;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_COLORARG1, &dwValue);
	if (D3DTA_TEXTURE != dwValue)
		return false;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_COLORARG2, &dwValue);
	if (D3DTA_DIFFUSE != dwValue)
		return false;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_ALPHAOP, &dwValue);
	if (D3DTOP_DISABLE == dwValue)
	{
		// Character-shadow receiver re-render: stage1 projects the shadow map via
		// CAMERASPACEPOSITION texgen. Must use the same vertex path as the main
		// pass or the two passes z-fight.
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLOROP, &dwValue);
		if (D3DTOP_MODULATE != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG1, &dwValue);
		if (D3DTA_TEXTURE != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG2, &dwValue);
		if (D3DTA_CURRENT != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &dwValue);
		if (D3DTSS_TCI_CAMERASPACEPOSITION != dwValue)
			return false;
		LPDIRECT3DBASETEXTURE9 pkShadowTexture;
		STATEMANAGER.GetTexture(0, &pkShadowTexture);
		if (!pkShadowTexture)
			return false;
		STATEMANAGER.GetTexture(1, &pkShadowTexture);
		if (!pkShadowTexture)
			return false;
		if (!gs_kShaderPool.BindPNTLitShadowReceiver())
			return false;
		__UploadGrannyLightingConstants();
		return true;
	}
	if (D3DTOP_MODULATE != dwValue)
		return false;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwValue);
	if (D3DTA_TEXTURE != dwValue)
		return false;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwValue);
	const bool bSpecularAlpha = (D3DTA_TFACTOR == dwValue);
	if (D3DTA_DIFFUSE != dwValue && !bSpecularAlpha)
		return false;
	STATEMANAGER.GetTextureStageState(1, D3DTSS_COLOROP, &dwValue);
	bool bSpecular = false;
	if (bSpecularAlpha && D3DTOP_MODULATEALPHA_ADDCOLOR == dwValue)
	{
		// Granny sphere-map specular: verify the full stage-1 cascade.
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG1, &dwValue);
		if (D3DTA_CURRENT != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_COLORARG2, &dwValue);
		if (D3DTA_TEXTURE != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_ALPHAOP, &dwValue);
		if (D3DTOP_SELECTARG1 != dwValue)
			return false;
		STATEMANAGER.GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &dwValue);
		if (D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR != dwValue)
			return false;
		LPDIRECT3DBASETEXTURE9 pEnvTexture;
		STATEMANAGER.GetTexture(1, &pEnvTexture);
		if (!pEnvTexture)
			return false;
		bSpecular = true;
	}
	else if (bSpecularAlpha || D3DTOP_DISABLE != dwValue)
		return false;

	LPDIRECT3DBASETEXTURE9 pTexture;
	STATEMANAGER.GetTexture(0, &pTexture);
	if (!pTexture)
		return false;

	// The character-preview screens light with a spot + point pair instead of
	// the world's directional light (grp.SetOmniLight); route them to the omni
	// program. Any other light type keeps the fixed-function path.
	if (STATEMANAGER.GetRenderState(D3DRS_LIGHTING) && STATEMANAGER.GetLightEnable(0))
	{
		D3DLIGHT9 kLight;
		STATEMANAGER.GetLight(0, &kLight);
		if (D3DLIGHT_SPOT == kLight.Type)
		{
			if (bSpecular)
				return false;
			if (!gs_kShaderPool.BindPNTLitOmni())
				return false;
			__UploadOmniLightingConstants();
			return true;
		}
		if (D3DLIGHT_DIRECTIONAL != kLight.Type)
			return false;
	}

	if (bSpecular)
	{
		if (!gs_kShaderPool.BindPNTLitSpecular())
			return false;
	}
	else if (!gs_kShaderPool.BindPNTLit())
		return false;

	__UploadGrannyLightingConstants();
	return true;
}

void CGraphicBase::__UploadGrannyLightingConstants()
{
	// Fixed-function directional light: color = saturate(cAmbient + cDiffuse * max(0, N.L)).
	D3DXVECTOR4 avConstants[3];
	if (STATEMANAGER.GetRenderState(D3DRS_LIGHTING) && STATEMANAGER.GetLightEnable(0))
	{
		D3DLIGHT9 kLight;
		STATEMANAGER.GetLight(0, &kLight);
		D3DMATERIAL9 kMaterial;
		STATEMANAGER.GetMaterial(&kMaterial);
		const DWORD dwAmbient = STATEMANAGER.GetRenderState(D3DRS_AMBIENT);
		const float c_fInv255 = 1.0f / 255.0f;
		const float fAmbientR = ((dwAmbient >> 16) & 0xff) * c_fInv255 + kLight.Ambient.r;
		const float fAmbientG = ((dwAmbient >> 8) & 0xff) * c_fInv255 + kLight.Ambient.g;
		const float fAmbientB = (dwAmbient & 0xff) * c_fInv255 + kLight.Ambient.b;
		const float fAmbientA = ((dwAmbient >> 24) & 0xff) * c_fInv255 + kLight.Ambient.a;

		avConstants[0] = D3DXVECTOR4(-kLight.Direction.x, -kLight.Direction.y, -kLight.Direction.z, 0.0f);
		avConstants[1] = D3DXVECTOR4(kMaterial.Diffuse.r * kLight.Diffuse.r,
									 kMaterial.Diffuse.g * kLight.Diffuse.g,
									 kMaterial.Diffuse.b * kLight.Diffuse.b,
									 kMaterial.Diffuse.a * kLight.Diffuse.a);
		avConstants[2] = D3DXVECTOR4(kMaterial.Ambient.r * fAmbientR + kMaterial.Emissive.r,
									 kMaterial.Ambient.g * fAmbientG + kMaterial.Emissive.g,
									 kMaterial.Ambient.b * fAmbientB + kMaterial.Emissive.b,
									 kMaterial.Ambient.a * fAmbientA + kMaterial.Emissive.a);
	}
	else
	{
		// Lighting off: fixed function feeds an opaque white vertex color.
		avConstants[0] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		avConstants[1] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		avConstants[2] = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	STATEMANAGER.SetVertexShaderConstant(8, avConstants, 3);
}

void CGraphicBase::__UploadOmniLightingConstants()
{
	// Fixed-function spot (light 0) + point (light 1) vertex lighting, as set up
	// by the character-preview screens. Products are premultiplied per light;
	// the shader applies range-clamped attenuation and the linear cone ramp.
	D3DLIGHT9 kSpot;
	STATEMANAGER.GetLight(0, &kSpot);
	D3DMATERIAL9 kMaterial;
	STATEMANAGER.GetMaterial(&kMaterial);
	const DWORD dwAmbient = STATEMANAGER.GetRenderState(D3DRS_AMBIENT);
	const float c_fInv255 = 1.0f / 255.0f;
	const float fGlobalR = ((dwAmbient >> 16) & 0xff) * c_fInv255;
	const float fGlobalG = ((dwAmbient >> 8) & 0xff) * c_fInv255;
	const float fGlobalB = (dwAmbient & 0xff) * c_fInv255;

	D3DXVECTOR3 v3SpotDir(kSpot.Direction.x, kSpot.Direction.y, kSpot.Direction.z);
	D3DXVec3Normalize(&v3SpotDir, &v3SpotDir);

	// A degenerate cone (Theta == Phi) would make the shader's linear ramp 0/0;
	// keep the denominator positive so it degrades to the FFP hard cutoff.
	const float fCosTheta = cosf(kSpot.Theta * 0.5f);
	float fCosPhi = cosf(kSpot.Phi * 0.5f);
	if (fCosTheta - fCosPhi < 0.0001f)
		fCosPhi = fCosTheta - 0.0001f;

	D3DXVECTOR4 avConstants[10];
	avConstants[0] = D3DXVECTOR4(kSpot.Position.x, kSpot.Position.y, kSpot.Position.z, kSpot.Range);
	avConstants[1] = D3DXVECTOR4(v3SpotDir.x, v3SpotDir.y, v3SpotDir.z, 0.0f);
	avConstants[2] = D3DXVECTOR4(kSpot.Attenuation0, kSpot.Attenuation1, kSpot.Attenuation2, fCosTheta);
	avConstants[3] = D3DXVECTOR4(kMaterial.Diffuse.r * kSpot.Diffuse.r,
								 kMaterial.Diffuse.g * kSpot.Diffuse.g,
								 kMaterial.Diffuse.b * kSpot.Diffuse.b,
								 fCosPhi);
	avConstants[4] = D3DXVECTOR4(kMaterial.Ambient.r * kSpot.Ambient.r,
								 kMaterial.Ambient.g * kSpot.Ambient.g,
								 kMaterial.Ambient.b * kSpot.Ambient.b, 0.0f);

	D3DLIGHT9 kPoint;
	STATEMANAGER.GetLight(1, &kPoint);
	const float fPointOn = STATEMANAGER.GetLightEnable(1) ? 1.0f : 0.0f;
	avConstants[5] = D3DXVECTOR4(kPoint.Position.x, kPoint.Position.y, kPoint.Position.z, kPoint.Range);
	avConstants[6] = D3DXVECTOR4(kPoint.Attenuation0, kPoint.Attenuation1, kPoint.Attenuation2, fPointOn);
	avConstants[7] = D3DXVECTOR4(kMaterial.Diffuse.r * kPoint.Diffuse.r,
								 kMaterial.Diffuse.g * kPoint.Diffuse.g,
								 kMaterial.Diffuse.b * kPoint.Diffuse.b, 0.0f);
	avConstants[8] = D3DXVECTOR4(kMaterial.Ambient.r * kPoint.Ambient.r,
								 kMaterial.Ambient.g * kPoint.Ambient.g,
								 kMaterial.Ambient.b * kPoint.Ambient.b, 0.0f);
	avConstants[9] = D3DXVECTOR4(kMaterial.Ambient.r * fGlobalR + kMaterial.Emissive.r,
								 kMaterial.Ambient.g * fGlobalG + kMaterial.Emissive.g,
								 kMaterial.Ambient.b * fGlobalB + kMaterial.Emissive.b,
								 kMaterial.Diffuse.a);
	STATEMANAGER.SetVertexShaderConstant(18, avConstants, 10);
}

void CGraphicBase::EndPDTShader()
{
	gs_kShaderPool.Unbind();
}

void CGraphicBase::DestroyShaderPool()
{
	gs_kShaderPool.Destroy();
}

DWORD CGraphicBase::GetAvailableTextureMemory()
{
	assert(ms_lpd3dDevice!=NULL && "CGraphicBase::GetAvailableTextureMemory - D3DDevice is EMPTY");

	static DWORD s_dwNextUpdateTime=0;
	static DWORD s_dwTexMemSize=0;//ms_lpd3dDevice->GetAvailableTextureMem();

	DWORD dwCurTime=ELTimer_GetMSec();
	if (s_dwNextUpdateTime<dwCurTime)
	{
		s_dwNextUpdateTime=dwCurTime+5000;
		s_dwTexMemSize=ms_lpd3dDevice->GetAvailableTextureMem();
	}

	return s_dwTexMemSize;
}

const D3DXMATRIX& CGraphicBase::GetViewMatrix()
{
	return ms_matView;
}

const D3DXMATRIX & CGraphicBase::GetIdentityMatrix()
{
	return ms_matIdentity;
}

void CGraphicBase::SetEyeCamera(float xEye, float yEye, float zEye,
								float xCenter, float yCenter, float zCenter,
								float xUp, float yUp, float zUp)
{
	D3DXVECTOR3 vectorEye(xEye, yEye, zEye);
	D3DXVECTOR3 vectorCenter(xCenter, yCenter, zCenter);
	D3DXVECTOR3 vectorUp(xUp, yUp, zUp);

//	CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_PERSPECTIVE_CAMERA);
	CCameraManager::Instance().GetCurrentCamera()->SetViewParams(vectorEye, vectorCenter, vectorUp);
	UpdateViewMatrix();
}

void CGraphicBase::SetSimpleCamera(float x, float y, float z, float pitch, float roll)
{
	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 vectorEye(x, y, z);

	pCamera->SetViewParams(D3DXVECTOR3(0.0f, y, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	pCamera->RotateEyeAroundTarget(pitch, roll);
	pCamera->Move(vectorEye);

	UpdateViewMatrix();

	// This is levites's virtual(?) code which you should not trust.
	ms_lpd3dDevice->GetTransform(D3DTS_WORLD, &ms_matWorld);
	D3DXMatrixMultiply(&ms_matWorldView, &ms_matWorld, &ms_matView);
}

void CGraphicBase::SetAroundCamera(float distance, float pitch, float roll, float lookAtZ)
{
	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	pCamera->SetViewParams(D3DXVECTOR3(0.0f, -distance, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	pCamera->RotateEyeAroundTarget(pitch, roll);
	D3DXVECTOR3 v3Target = pCamera->GetTarget();
	v3Target.z = lookAtZ;
	pCamera->SetTarget(v3Target);
// 	pCamera->Move(v3Target);

	UpdateViewMatrix();

	// This is levites's virtual(?) code which you should not trust.
	ms_lpd3dDevice->GetTransform(D3DTS_WORLD, &ms_matWorld);
	D3DXMatrixMultiply(&ms_matWorldView, &ms_matWorld, &ms_matView);
}

void CGraphicBase::SetPositionCamera(float fx, float fy, float fz, float distance, float pitch, float roll)
{
	// I wanna downward this code to the game control level. - [levites]
	if (ms_dwWavingEndTime > CTimer::Instance().GetCurrentMillisecond())
	{
		if (ms_iWavingPower>0)
		{
			fx += float(rand() % ms_iWavingPower) / 10.0f;
			fy += float(rand() % ms_iWavingPower) / 10.0f;
			fz += float(rand() % ms_iWavingPower) / 10.0f;
		}
	}

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (!pCamera)
		return;

	pCamera->SetViewParams(D3DXVECTOR3(0.0f, -distance, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	pitch = fMIN(80.0f, fMAX(-80.0f, pitch) );
//	Tracef("SetPosition Camera : %f, %f\n", pitch, roll);
	pCamera->RotateEyeAroundTarget(pitch, roll);
	pCamera->Move(D3DXVECTOR3(fx, fy, fz));

	UpdateViewMatrix();

	// This is levites's virtual(?) code which you should not trust.
	STATEMANAGER.GetTransform(D3DTS_WORLD, &ms_matWorld);
	D3DXMatrixMultiply(&ms_matWorldView, &ms_matWorld, &ms_matView);
}

void CGraphicBase::SetOrtho2D(float hres, float vres, float zres)
{
	//CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_ORTHO_CAMERA);
	D3DXMatrixOrthoOffCenterRH(&ms_matProj, 0, hres, vres, 0, 0, zres);
	//UpdatePipeLineMatrix();
	UpdateProjMatrix();
}

void CGraphicBase::SetOrtho3D(float hres, float vres, float zmin, float zmax)
{
	//CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_PERSPECTIVE_CAMERA);
	D3DXMatrixOrthoRH(&ms_matProj, hres, vres, zmin, zmax);
	//UpdatePipeLineMatrix();
	UpdateProjMatrix();
}

void CGraphicBase::SetPerspective(float fov, float aspect, float nearz, float farz)
{
	ms_fFieldOfView = fov;


	//if (ms_d3dPresentParameter.BackBufferWidth>0 && ms_d3dPresentParameter.BackBufferHeight>0)
	//	ms_fAspect = float(ms_d3dPresentParameter.BackBufferWidth)/float(ms_d3dPresentParameter.BackBufferHeight);
	//else
	ms_fAspect = aspect;

	ms_fNearY = nearz;
	ms_fFarY = farz;

	//CCameraManager::Instance().SetCurrentCamera(CCameraManager::DEFAULT_PERSPECTIVE_CAMERA);
	D3DXMatrixPerspectiveFovRH(&ms_matProj, D3DXToRadian(fov), ms_fAspect, nearz, farz);		
	//UpdatePipeLineMatrix();
	UpdateProjMatrix();
}

void CGraphicBase::UpdateProjMatrix()
{
	STATEMANAGER.SetTransform(D3DTS_PROJECTION, &ms_matProj);
}

void CGraphicBase::UpdateViewMatrix()
{
	CCamera* pkCamera=CCameraManager::Instance().GetCurrentCamera();
	if (!pkCamera)
		return;

	ms_matView = pkCamera->GetViewMatrix();
	STATEMANAGER.SetTransform(D3DTS_VIEW, &ms_matView);

	D3DXMatrixInverse(&ms_matInverseView, NULL, &ms_matView);
	ms_matInverseViewYAxis._11 = ms_matInverseView._11;
	ms_matInverseViewYAxis._12 = ms_matInverseView._12;
	ms_matInverseViewYAxis._21 = ms_matInverseView._21;
	ms_matInverseViewYAxis._22 = ms_matInverseView._22;
}

void CGraphicBase::UpdatePipeLineMatrix()
{
	UpdateProjMatrix();
	UpdateViewMatrix();
}

void CGraphicBase::SetViewport(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, float fMinZ, float fMaxZ)
{
	ms_Viewport.X = dwX;
	ms_Viewport.Y = dwY;
	ms_Viewport.Width = dwWidth;
	ms_Viewport.Height = dwHeight;
	ms_Viewport.MinZ = fMinZ;
	ms_Viewport.MaxZ = fMaxZ;
}

void CGraphicBase::GetTargetPosition(float * px, float * py, float * pz)
{
	*px = CCameraManager::Instance().GetCurrentCamera()->GetTarget().x;
	*py = CCameraManager::Instance().GetCurrentCamera()->GetTarget().y;
	*pz = CCameraManager::Instance().GetCurrentCamera()->GetTarget().z;
}

void CGraphicBase::GetCameraPosition(float * px, float * py, float * pz)
{
	*px = CCameraManager::Instance().GetCurrentCamera()->GetEye().x;
	*py = CCameraManager::Instance().GetCurrentCamera()->GetEye().y;
	*pz = CCameraManager::Instance().GetCurrentCamera()->GetEye().z;
}

void CGraphicBase::GetMatrix(D3DXMATRIX* pRetMatrix) const
{
	assert(ms_lpd3dMatStack != NULL);
	*pRetMatrix = *ms_lpd3dMatStack->GetTop();
}

const D3DXMATRIX* CGraphicBase::GetMatrixPointer() const
{
	assert(ms_lpd3dMatStack!=NULL);
	return ms_lpd3dMatStack->GetTop();
}

void CGraphicBase::GetSphereMatrix(D3DXMATRIX * pMatrix, float fValue)
{
	D3DXMatrixIdentity(pMatrix);
	pMatrix->_11 = fValue * ms_matWorldView._11;
	pMatrix->_21 = fValue * ms_matWorldView._21;
	pMatrix->_31 = fValue * ms_matWorldView._31;
	pMatrix->_41 = fValue;
	pMatrix->_12 = -fValue * ms_matWorldView._12;
	pMatrix->_22 = -fValue * ms_matWorldView._22;
	pMatrix->_32 = -fValue * ms_matWorldView._32;
	pMatrix->_42 = -fValue;
}

float CGraphicBase::GetFOV()
{
	return ms_fFieldOfView;
}

void CGraphicBase::PushMatrix()
{
	ms_lpd3dMatStack->Push();
}

void CGraphicBase::Scale(float x, float y, float z)
{
	ms_lpd3dMatStack->Scale(x, y, z);
}

void CGraphicBase::Rotate(float degree, float x, float y, float z)
{
	D3DXVECTOR3 vec(x, y, z);
	ms_lpd3dMatStack->RotateAxis(&vec, D3DXToRadian(degree));
}

void CGraphicBase::RotateLocal(float degree, float x, float y, float z)
{
	D3DXVECTOR3 vec(x, y, z);
	ms_lpd3dMatStack->RotateAxisLocal(&vec, D3DXToRadian(degree));
}

void CGraphicBase::MultMatrix( const D3DXMATRIX* pMat)
{
	ms_lpd3dMatStack->MultMatrix(pMat);
}

void CGraphicBase::MultMatrixLocal( const D3DXMATRIX* pMat)
{
	ms_lpd3dMatStack->MultMatrixLocal(pMat);
}

void CGraphicBase::RotateYawPitchRollLocal(float fYaw, float fPitch, float fRoll)
{
	ms_lpd3dMatStack->RotateYawPitchRollLocal(D3DXToRadian(fYaw), D3DXToRadian(fPitch), D3DXToRadian(fRoll));
}

void CGraphicBase::Translate(float x, float y, float z)
{
	ms_lpd3dMatStack->Translate(x, y, z);
}

void CGraphicBase::LoadMatrix(const D3DXMATRIX& c_rSrcMatrix)
{
	ms_lpd3dMatStack->LoadMatrix(&c_rSrcMatrix);
}

void CGraphicBase::PopMatrix()
{
	ms_lpd3dMatStack->Pop();
}

DWORD CGraphicBase::GetColor(float r, float g, float b, float a)
{
	BYTE argb[4] =
	{
		(BYTE) (255.0f * b),
		(BYTE) (255.0f * g),
		(BYTE) (255.0f * r),
		(BYTE) (255.0f * a)
	};

	return *((DWORD *) argb);
}

void CGraphicBase::InitScreenEffect()
{
	ms_dwWavingEndTime = 0;
	ms_dwFlashingEndTime = 0;
	ms_iWavingPower = 0;
	ms_FlashingColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
}

void CGraphicBase::SetScreenEffectWaving(float fDuringTime, int iPower)
{
	ms_dwWavingEndTime = CTimer::Instance().GetCurrentMillisecond() + long(fDuringTime * 1000.0f);
	ms_iWavingPower = iPower;
}

void CGraphicBase::SetScreenEffectFlashing(float fDuringTime, const D3DXCOLOR & c_rColor)
{
	ms_dwFlashingEndTime = CTimer::Instance().GetCurrentMillisecond() + long(fDuringTime * 1000.0f);
	ms_FlashingColor = c_rColor;
}

DWORD CGraphicBase::GetFaceCount()
{
	return ms_faceCount;
}

void CGraphicBase::ResetFaceCount()
{
	ms_faceCount = 0;
}

HRESULT CGraphicBase::GetLastResult()
{
	return ms_hLastResult;
}

CGraphicBase::CGraphicBase()
{
}

CGraphicBase::~CGraphicBase()
{
}
