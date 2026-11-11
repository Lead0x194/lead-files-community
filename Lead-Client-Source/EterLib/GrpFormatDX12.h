#pragma once

// D3D9 to DXGI format translation plus the CPU widening the DX12 backend
// needs for the 16bpp texture formats DXGI cannot rely on (B5G6R5 and friends
// are optional there): those load as B8G8R8A8 after conversion.

#include <d3d9.h>
#include <d3d12.h>

class CGraphicFormatDX12
{
	public:
		// DXGI_FORMAT_UNKNOWN when unmapped. *pbNeedsWidening is set when the
		// source must convert to 32bpp first (16bpp color formats).
		static DXGI_FORMAT	ToTextureFormatDX12(D3DFORMAT eFormatD3D9, bool* pbNeedsWidening);

		static DXGI_FORMAT	ToIndexFormatDX12(D3DFORMAT eFormatD3D9);
		static DXGI_FORMAT	ToDepthFormatDX12(D3DFORMAT eFormatD3D9);

		static bool	IsBlockCompressed(DXGI_FORMAT eFormat);

		// Tight pitch of one row (one block row for BC formats); 0 if unknown.
		static UINT	GetRowPitch(DXGI_FORMAT eFormat, UINT uWidth);

		// Row count CopyTextureRegion walks (block rows for BC formats).
		static UINT	GetRowCount(DXGI_FORMAT eFormat, UINT uHeight);

		// 16bpp to B8G8R8A8 widening (destination pixel = 0xAARRGGBB DWORD).
		static void	WidenR5G6B5(const void* pvSource, UINT uPixelCount, DWORD* adwDest);
		static void	WidenA1R5G5B5(const void* pvSource, UINT uPixelCount, DWORD* adwDest);
		static void	WidenA4R4G4B4(const void* pvSource, UINT uPixelCount, DWORD* adwDest);
};
