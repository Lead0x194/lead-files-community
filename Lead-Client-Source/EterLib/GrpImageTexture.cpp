#include "StdAfx.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "GrpImageTexture.h"

bool CGraphicImageTexture::Lock(int* pRetPitch, void** ppRetPixels, int level)
{
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = m_lpd3dTexture->LockRect(level, &lockedRect, NULL, 0);
	if (FAILED(hr))
	{
		D3DSURFACE_DESC desc;
		if (SUCCEEDED(m_lpd3dTexture->GetLevelDesc(level, &desc)))
			TraceError("CGraphicImageTexture::LockRect: hr=0x%08X pool=%u format=%u", hr, desc.Pool, desc.Format);
		else
			TraceError("CGraphicImageTexture::LockRect: hr=0x%08X", hr);
		return false;
	}

	*pRetPitch = lockedRect.Pitch;
	*ppRetPixels = (void*)lockedRect.pBits;	
	return true;
}

void CGraphicImageTexture::Unlock(int level)
{
	assert(m_lpd3dTexture != NULL);
	m_lpd3dTexture->UnlockRect(level);
}

void CGraphicImageTexture::Initialize()
{
	CGraphicTexture::Initialize();

	m_stFileName = "";

	m_d3dFmt=D3DFMT_UNKNOWN;
	m_dwFilter=0;
}

void CGraphicImageTexture::Destroy()
{
	CGraphicTexture::Destroy();

	Initialize();
}

bool CGraphicImageTexture::CreateDeviceObjects()
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

	if (m_stFileName.empty())
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, D3DUSAGE_DYNAMIC, m_d3dFmt, D3DPOOL_DEFAULT, &m_lpd3dTexture, NULL)))
			return false;
	}
	else
	{
		CMappedFile	mappedFile;
		LPCVOID		c_pvMap;

		if (!CEterPackManager::Instance().Get(mappedFile, m_stFileName.c_str(), &c_pvMap))
			return false;

		if (!CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter))
		{
			TraceError("CGraphicImageTexture::CreateDeviceObjects - texture not found(%s)", m_stFileName.c_str());
			return false;
		}

		return true;
	}

	m_bEmpty = false;
	return true;
}

bool CGraphicImageTexture::Create(UINT width, UINT height, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	Destroy();

	m_width = width;
	m_height = height;
	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;

	return CreateDeviceObjects();
}

void CGraphicImageTexture::CreateFromTexturePointer(const CGraphicTexture * c_pSrcTexture)
{
	if (m_lpd3dTexture)
		m_lpd3dTexture->Release();
	
	m_width = c_pSrcTexture->GetWidth();
	m_height = c_pSrcTexture->GetHeight();
	m_lpd3dTexture = c_pSrcTexture->GetD3DTexture();
	
	if (m_lpd3dTexture)
		m_lpd3dTexture->AddRef();

	m_bEmpty = false;
}

bool CGraphicImageTexture::CreateDDSTexture(CDXTCImage & image, const BYTE * /*c_pbBuf*/)
{
	int mipmapCount = image.m_dwMipMapCount == 0 ? 1 : image.m_dwMipMapCount;

	D3DFORMAT format;
	LPDIRECT3DTEXTURE9 lpd3dTexture = NULL;
	LPDIRECT3DTEXTURE9 lpd3dStaging = NULL;
	D3DPOOL pool = D3DPOOL_DEFAULT;

	if(image.m_CompFormat == PF_DXT5)
		format = D3DFMT_DXT5;
	else if(image.m_CompFormat == PF_DXT4)
		format = D3DFMT_DXT4;
	else if(image.m_CompFormat == PF_DXT3)
		format = D3DFMT_DXT3;
	else if(image.m_CompFormat == PF_DXT2)
		format = D3DFMT_DXT2;
	else
		format = D3DFMT_DXT1;

	UINT uTexBias=0;
	if (IsLowTextureMemory())
		uTexBias=1;

	UINT uMinMipMapIndex=0;
	if (uTexBias>0)
	{
		if (mipmapCount>uTexBias)
		{
			uMinMipMapIndex=uTexBias;
			image.m_nWidth>>=uTexBias;
			image.m_nHeight>>=uTexBias;
			mipmapCount-=uTexBias;
		}
	}

	if (FAILED(D3DXCreateTexture(	ms_lpd3dDevice, image.m_nWidth, image.m_nHeight,
										mipmapCount, 0, format, D3DPOOL_SYSTEMMEM, &lpd3dStaging)))
	{
		TraceError("CreateDDSTexture: Cannot creatre texture" );
		return false;
	}

	if (FAILED(D3DXCreateTexture(	ms_lpd3dDevice, image.m_nWidth, image.m_nHeight,
									mipmapCount, 0, format, pool, &lpd3dTexture)))
	{
		TraceError("CreateDDSTexture: Cannot creatre texture");
		lpd3dStaging->Release();
		return false;
	}

	for (DWORD i = 0; i < mipmapCount; ++i)
	{
		D3DLOCKED_RECT lockedRect;
		HRESULT hr = lpd3dStaging->LockRect(i, &lockedRect, NULL, 0);
		if (FAILED(hr))
		{
			D3DSURFACE_DESC desc;
			if (SUCCEEDED(lpd3dStaging->GetLevelDesc(i, &desc)))
				TraceError("CreateDDSTexture: LockRect failed hr=0x%08X pool=%u format=%u level=%u", hr, desc.Pool, desc.Format, i);
			else
				TraceError("CreateDDSTexture: LockRect failed hr=0x%08X level=%u", hr, i);
		}
		else
		{
			image.Copy(i+uMinMipMapIndex, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
			lpd3dStaging->UnlockRect(i);
		}
	}

	HRESULT hrUpdate = ms_lpd3dDevice->UpdateTexture(lpd3dStaging, lpd3dTexture);
	if (FAILED(hrUpdate))
	{
		TraceError("CreateDDSTexture: UpdateTexture failed hr=0x%08X", hrUpdate);
		lpd3dStaging->Release();
		lpd3dTexture->Release();
		return false;
	}

	lpd3dStaging->Release();

	if(ms_bSupportDXT)
	{
		m_lpd3dTexture = lpd3dTexture;
	}
	else
	{
		if(image.m_CompFormat == PF_DXT2 || image.m_CompFormat == PF_DXT3 ||
		   image.m_CompFormat == PF_DXT4 || image.m_CompFormat == PF_DXT5)
			format = D3DFMT_A4R4G4B4;
		else
			format = D3DFMT_A1R5G5B5;

		UINT imgWidth=image.m_nWidth;
		UINT imgHeight=image.m_nHeight;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;

		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE && uTexBias>0 && mipmapCount==0)
		{
			imgWidth>>=uTexBias;
			imgHeight>>=uTexBias;		
		}

		if (FAILED(D3DXCreateTexture(	ms_lpd3dDevice, imgWidth, imgHeight, 
									mipmapCount, 0, format, D3DPOOL_DEFAULT, &m_lpd3dTexture)))
		{
				TraceError("CreateDDSTexture: Cannot creatre texture");
				return false;
		}

		IDirect3DTexture9* pkTexSrc=lpd3dTexture;
		IDirect3DTexture9* pkTexDst=m_lpd3dTexture;

		for(int i=0; i<mipmapCount; ++i) {

			IDirect3DSurface9* ppsSrc = NULL;
			IDirect3DSurface9* ppsDst = NULL;

			if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
			{
				if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
				{
					D3DXLoadSurfaceFromSurface(ppsDst, NULL, NULL, ppsSrc, NULL, NULL, D3DX_FILTER_NONE, 0);
					ppsDst->Release();
				}
				ppsSrc->Release();
			}
		}

		lpd3dTexture->Release();
	}

	m_width = image.m_nWidth;
	m_height = image.m_nHeight;
	m_bEmpty = false;

	return true;
}

bool CGraphicImageTexture::CreateFromMemoryFile(UINT bufSize, const void * c_pvBuf, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

	static CDXTCImage image;

	if (image.LoadHeaderFromMemory((const BYTE *) c_pvBuf))	// Check if it is DDS
	{
		return (CreateDDSTexture(image, (const BYTE *) c_pvBuf));
	}
	else
	{
		D3DXIMAGE_INFO imageInfo;
		if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
					ms_lpd3dDevice,
					c_pvBuf,
					bufSize,
						D3DX_DEFAULT_NONPOW2,
						D3DX_DEFAULT_NONPOW2,
					D3DX_DEFAULT,
					0,
					d3dFmt,
						D3DPOOL_DEFAULT,
					dwFilter,
					dwFilter,
					0xffff00ff,
					&imageInfo,
					NULL,
					&m_lpd3dTexture)))
		{
			TraceError("CreateFromMemoryFile: Cannot create texture");
			return false;
		}

		m_width = imageInfo.Width;
		m_height = imageInfo.Height;

		D3DFORMAT format=imageInfo.Format;
		switch(imageInfo.Format) {
			case D3DFMT_A8R8G8B8:
				format = D3DFMT_A4R4G4B4;
				break;

			case D3DFMT_X8R8G8B8:
			case D3DFMT_R8G8B8:
				format = D3DFMT_A1R5G5B5;
				break;
		}

		UINT uTexBias=0;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;
		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE)
			uTexBias=1;

		if (IsLowTextureMemory())
		if (uTexBias || format!=imageInfo.Format)
		{
			IDirect3DTexture9* pkTexSrc=m_lpd3dTexture;
			IDirect3DTexture9* pkTexDst;
			
			
			if (SUCCEEDED(D3DXCreateTexture(	
				ms_lpd3dDevice, 
				imageInfo.Width>>uTexBias, 
				imageInfo.Height>>uTexBias, 
				imageInfo.MipLevels, 
				0, 
				format, 
				D3DPOOL_DEFAULT,
				&pkTexDst)))
			{
				m_lpd3dTexture=pkTexDst;
				
				for(int i=0; i<imageInfo.MipLevels; ++i) {

					IDirect3DSurface9* ppsSrc = NULL;
					IDirect3DSurface9* ppsDst = NULL;

					if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
					{
						if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
						{
							D3DXLoadSurfaceFromSurface(ppsDst, NULL, NULL, ppsSrc, NULL, NULL, D3DX_FILTER_LINEAR, 0);
							ppsDst->Release();
						}
						ppsSrc->Release();
					}
				}

				pkTexSrc->Release();
			}
		}
	}

	m_bEmpty = false;
	return true;
}

void CGraphicImageTexture::SetFileName(const char * c_szFileName)
{
	m_stFileName=c_szFileName;
}

bool CGraphicImageTexture::CreateFromDiskFile(const char * c_szFileName, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	Destroy();

	SetFileName(c_szFileName);

	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;
	return CreateDeviceObjects();
}

CGraphicImageTexture::CGraphicImageTexture()
{
	Initialize();
}

CGraphicImageTexture::~CGraphicImageTexture()
{
	Destroy();
}
