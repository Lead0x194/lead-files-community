#include "StdAfx.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "GrpImageTexture.h"
#include "ImageFileDecoder.h"

namespace
{
	// D3DX-style colorkey: pixels matching the ARGB key become transparent black.
	void ApplyColorKey(SDecodedImage& rkImage, DWORD dwARGBColorKey)
	{
		const BYTE byA = BYTE(dwARGBColorKey >> 24);
		const BYTE byR = BYTE(dwARGBColorKey >> 16);
		const BYTE byG = BYTE(dwARGBColorKey >> 8);
		const BYTE byB = BYTE(dwARGBColorKey);

		BYTE* pbyPixel = rkImage.kPixels.data();
		BYTE* pbyEnd = pbyPixel + rkImage.kPixels.size();
		for (; pbyPixel < pbyEnd; pbyPixel += 4)
		{
			if (pbyPixel[0] == byB && pbyPixel[1] == byG && pbyPixel[2] == byR && pbyPixel[3] == byA)
			{
				pbyPixel[0] = 0;
				pbyPixel[1] = 0;
				pbyPixel[2] = 0;
				pbyPixel[3] = 0;
			}
		}
	}

	// 2x2 box filter (clamped at odd edges) - equivalent of D3DX_FILTER_LINEAR mips.
	void DownsampleBox(const std::vector<BYTE>& c_rkSrc, UINT uSrcWidth, UINT uSrcHeight,
					   std::vector<BYTE>& rkDst, UINT uDstWidth, UINT uDstHeight)
	{
		rkDst.resize(size_t(uDstWidth) * uDstHeight * 4);

		for (UINT y = 0; y < uDstHeight; ++y)
		{
			const UINT uSrcY0 = y * 2;
			const UINT uSrcY1 = (uSrcY0 + 1 < uSrcHeight) ? uSrcY0 + 1 : uSrcY0;
			for (UINT x = 0; x < uDstWidth; ++x)
			{
				const UINT uSrcX0 = x * 2;
				const UINT uSrcX1 = (uSrcX0 + 1 < uSrcWidth) ? uSrcX0 + 1 : uSrcX0;

				const BYTE* pbySrc00 = &c_rkSrc[(size_t(uSrcY0) * uSrcWidth + uSrcX0) * 4];
				const BYTE* pbySrc01 = &c_rkSrc[(size_t(uSrcY0) * uSrcWidth + uSrcX1) * 4];
				const BYTE* pbySrc10 = &c_rkSrc[(size_t(uSrcY1) * uSrcWidth + uSrcX0) * 4];
				const BYTE* pbySrc11 = &c_rkSrc[(size_t(uSrcY1) * uSrcWidth + uSrcX1) * 4];

				BYTE* pbyDst = &rkDst[(size_t(y) * uDstWidth + x) * 4];
				for (UINT c = 0; c < 4; ++c)
					pbyDst[c] = BYTE((UINT(pbySrc00[c]) + pbySrc01[c] + pbySrc10[c] + pbySrc11[c] + 2) / 4);
			}
		}
	}

	// Full-mip-chain A8R8G8B8 texture from decoded pixels (staging -> default, like the DDS path).
	LPDIRECT3DTEXTURE9 CreateTextureFromDecodedImage(LPDIRECT3DDEVICE9 lpDevice, const SDecodedImage& c_rkImage)
	{
		UINT uMipCount = 1;
		{
			UINT uWidth = c_rkImage.uWidth;
			UINT uHeight = c_rkImage.uHeight;
			while (uWidth > 1 || uHeight > 1)
			{
				uWidth = uWidth > 1 ? uWidth / 2 : 1;
				uHeight = uHeight > 1 ? uHeight / 2 : 1;
				++uMipCount;
			}
		}

		LPDIRECT3DTEXTURE9 lpd3dStaging = NULL;
		LPDIRECT3DTEXTURE9 lpd3dTexture = NULL;

		if (FAILED(lpDevice->CreateTexture(c_rkImage.uWidth, c_rkImage.uHeight,
										   uMipCount, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &lpd3dStaging, NULL)))
			return NULL;

		if (FAILED(lpDevice->CreateTexture(c_rkImage.uWidth, c_rkImage.uHeight,
										   uMipCount, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpd3dTexture, NULL)))
		{
			lpd3dStaging->Release();
			return NULL;
		}

		std::vector<BYTE> kLevelPixels = c_rkImage.kPixels;
		std::vector<BYTE> kNextPixels;
		UINT uLevelWidth = c_rkImage.uWidth;
		UINT uLevelHeight = c_rkImage.uHeight;

		for (UINT uLevel = 0; uLevel < uMipCount; ++uLevel)
		{
			D3DLOCKED_RECT lockedRect;
			if (SUCCEEDED(lpd3dStaging->LockRect(uLevel, &lockedRect, NULL, 0)))
			{
				const BYTE* pbySrcRow = kLevelPixels.data();
				BYTE* pbyDstRow = (BYTE*)lockedRect.pBits;
				for (UINT y = 0; y < uLevelHeight; ++y)
				{
					memcpy(pbyDstRow, pbySrcRow, size_t(uLevelWidth) * 4);
					pbySrcRow += size_t(uLevelWidth) * 4;
					pbyDstRow += lockedRect.Pitch;
				}
				lpd3dStaging->UnlockRect(uLevel);
			}

			if (uLevel + 1 < uMipCount)
			{
				const UINT uNextWidth = uLevelWidth > 1 ? uLevelWidth / 2 : 1;
				const UINT uNextHeight = uLevelHeight > 1 ? uLevelHeight / 2 : 1;
				DownsampleBox(kLevelPixels, uLevelWidth, uLevelHeight, kNextPixels, uNextWidth, uNextHeight);
				kLevelPixels.swap(kNextPixels);
				uLevelWidth = uNextWidth;
				uLevelHeight = uNextHeight;
			}
		}

		if (FAILED(lpDevice->UpdateTexture(lpd3dStaging, lpd3dTexture)))
		{
			lpd3dStaging->Release();
			lpd3dTexture->Release();
			return NULL;
		}

		lpd3dStaging->Release();
		return lpd3dTexture;
	}
}

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

	if(image.m_CompFormat == PF_DXT5)
		format = D3DFMT_DXT5;
	else if(image.m_CompFormat == PF_DXT3)
		format = D3DFMT_DXT3;
	else
		format = D3DFMT_DXT1;

	if (FAILED(ms_lpd3dDevice->CreateTexture(	image.m_nWidth, image.m_nHeight,
										mipmapCount, 0, format, D3DPOOL_SYSTEMMEM, &lpd3dStaging, NULL)))
	{
		TraceError("CreateDDSTexture: Cannot creatre texture" );
		return false;
	}

	if (FAILED(ms_lpd3dDevice->CreateTexture(	image.m_nWidth, image.m_nHeight,
									mipmapCount, 0, format, D3DPOOL_DEFAULT, &lpd3dTexture, NULL)))
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
			image.Copy(i, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
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

	m_lpd3dTexture = lpd3dTexture;

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
		if (D3DFMT_UNKNOWN != d3dFmt && D3DFMT_A8R8G8B8 != d3dFmt)
		{
			TraceError("CreateFromMemoryFile: Unsupported format request %u", d3dFmt);
			return false;
		}

		SDecodedImage kImage;
		if (!DecodeImageFileFromMemory(c_pvBuf, bufSize, &kImage))
		{
			TraceError("CreateFromMemoryFile: Cannot create texture");
			return false;
		}

		ApplyColorKey(kImage, 0xffff00ff);

		m_lpd3dTexture = CreateTextureFromDecodedImage(ms_lpd3dDevice, kImage);
		if (!m_lpd3dTexture)
		{
			TraceError("CreateFromMemoryFile: Cannot create texture");
			return false;
		}

		m_width = kImage.uWidth;
		m_height = kImage.uHeight;
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
