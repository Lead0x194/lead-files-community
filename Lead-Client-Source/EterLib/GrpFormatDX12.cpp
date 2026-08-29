#include "StdAfx.h"
#include "GrpFormatDX12.h"

DXGI_FORMAT CGraphicFormatDX12::ToTextureFormatDX12(D3DFORMAT eFormatD3D9, bool* pbNeedsWidening)
{
	*pbNeedsWidening = false;

	switch (eFormatD3D9)
	{
		case D3DFMT_A8R8G8B8:	return DXGI_FORMAT_B8G8R8A8_UNORM;
		case D3DFMT_X8R8G8B8:	return DXGI_FORMAT_B8G8R8X8_UNORM;
		case D3DFMT_A8:			return DXGI_FORMAT_A8_UNORM;

		// Shaders read luminance through .r; the pool samples these only as
		// masks, so no swizzle pass is needed.
		case D3DFMT_L8:			return DXGI_FORMAT_R8_UNORM;
		case D3DFMT_A8L8:		return DXGI_FORMAT_R8G8_UNORM;

		case D3DFMT_DXT1:		return DXGI_FORMAT_BC1_UNORM;
		case D3DFMT_DXT2:
		case D3DFMT_DXT3:		return DXGI_FORMAT_BC2_UNORM;
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:		return DXGI_FORMAT_BC3_UNORM;

		// 16bpp color: DXGI support is optional, so these widen on load.
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
			*pbNeedsWidening = true;
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		default:
			break;
	}
	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CGraphicFormatDX12::ToIndexFormatDX12(D3DFORMAT eFormatD3D9)
{
	if (D3DFMT_INDEX16 == eFormatD3D9)
		return DXGI_FORMAT_R16_UINT;
	if (D3DFMT_INDEX32 == eFormatD3D9)
		return DXGI_FORMAT_R32_UINT;
	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CGraphicFormatDX12::ToDepthFormatDX12(D3DFORMAT eFormatD3D9)
{
	switch (eFormatD3D9)
	{
		case D3DFMT_D24S8:
		case D3DFMT_D24X8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case D3DFMT_D16:	return DXGI_FORMAT_D16_UNORM;
		case D3DFMT_D32:	return DXGI_FORMAT_D32_FLOAT;
		default:			break;
	}
	return DXGI_FORMAT_UNKNOWN;
}

bool CGraphicFormatDX12::IsBlockCompressed(DXGI_FORMAT eFormat)
{
	switch (eFormat)
	{
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
			return true;
		default:
			return false;
	}
}

UINT CGraphicFormatDX12::GetRowPitch(DXGI_FORMAT eFormat, UINT uWidth)
{
	switch (eFormat)
	{
		case DXGI_FORMAT_BC1_UNORM:
			return ((uWidth + 3) / 4) * 8;
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
			return ((uWidth + 3) / 4) * 16;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return uWidth * 4;
		case DXGI_FORMAT_R8G8_UNORM:
			return uWidth * 2;
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_R8_UNORM:
			return uWidth;
		default:
			return 0;
	}
}

UINT CGraphicFormatDX12::GetRowCount(DXGI_FORMAT eFormat, UINT uHeight)
{
	if (IsBlockCompressed(eFormat))
		return (uHeight + 3) / 4;
	return uHeight;
}

void CGraphicFormatDX12::WidenR5G6B5(const void* pvSource, UINT uPixelCount, DWORD* adwDest)
{
	const WORD* awSource = static_cast<const WORD*>(pvSource);
	for (UINT uPos = 0; uPos != uPixelCount; ++uPos)
	{
		const WORD wPixel = awSource[uPos];
		const DWORD dwRed = (wPixel >> 11) & 0x1F;
		const DWORD dwGreen = (wPixel >> 5) & 0x3F;
		const DWORD dwBlue = wPixel & 0x1F;
		adwDest[uPos] = 0xFF000000
			| (((dwRed << 3) | (dwRed >> 2)) << 16)
			| (((dwGreen << 2) | (dwGreen >> 4)) << 8)
			| ((dwBlue << 3) | (dwBlue >> 2));
	}
}

void CGraphicFormatDX12::WidenA1R5G5B5(const void* pvSource, UINT uPixelCount, DWORD* adwDest)
{
	const WORD* awSource = static_cast<const WORD*>(pvSource);
	for (UINT uPos = 0; uPos != uPixelCount; ++uPos)
	{
		const WORD wPixel = awSource[uPos];
		const DWORD dwRed = (wPixel >> 10) & 0x1F;
		const DWORD dwGreen = (wPixel >> 5) & 0x1F;
		const DWORD dwBlue = wPixel & 0x1F;
		adwDest[uPos] = ((wPixel & 0x8000) ? 0xFF000000 : 0)
			| (((dwRed << 3) | (dwRed >> 2)) << 16)
			| (((dwGreen << 3) | (dwGreen >> 2)) << 8)
			| ((dwBlue << 3) | (dwBlue >> 2));
	}
}

void CGraphicFormatDX12::WidenA4R4G4B4(const void* pvSource, UINT uPixelCount, DWORD* adwDest)
{
	const WORD* awSource = static_cast<const WORD*>(pvSource);
	for (UINT uPos = 0; uPos != uPixelCount; ++uPos)
	{
		const WORD wPixel = awSource[uPos];
		const DWORD dwAlpha = (wPixel >> 12) & 0x0F;
		const DWORD dwRed = (wPixel >> 8) & 0x0F;
		const DWORD dwGreen = (wPixel >> 4) & 0x0F;
		const DWORD dwBlue = wPixel & 0x0F;
		adwDest[uPos] = ((dwAlpha * 17) << 24)
			| ((dwRed * 17) << 16)
			| ((dwGreen * 17) << 8)
			| (dwBlue * 17);
	}
}
