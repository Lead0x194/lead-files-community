#pragma once

// D3D9 vertex declaration to D3D12 input layout translation. Pure CPU code:
// the resulting element array feeds D3D12_GRAPHICS_PIPELINE_STATE_DESC.
// D3DCOLOR maps to B8G8R8A8_UNORM so the BGRA byte order needs no swizzle;
// desktop feature-level-11 hardware supports it as an IA format.

#include <d3d9.h>
#include <d3d12.h>

class CGraphicInputLayoutDX12
{
	public:
		enum { MAX_ELEMENTS = 16 };

		// Walks pkElements9 up to D3DDECL_END(); fails on overflow or a
		// declaration type/usage with no D3D12 equivalent.
		static bool	Build(const D3DVERTEXELEMENT9* pkElements9,
						  D3D12_INPUT_ELEMENT_DESC* akElementsOut,
						  UINT uMaxElementCount,
						  UINT* puElementCountOut);

		// DXGI_FORMAT_UNKNOWN when the D3DDECLTYPE has no equivalent.
		static DXGI_FORMAT	ToFormatDX12(BYTE byDeclTypeD3D9);

		// NULL when the D3DDECLUSAGE has no equivalent (POSITIONT: pre-
		// transformed vertices get rebuilt as regular draws on DX12).
		static const char*	ToSemanticNameDX12(BYTE byDeclUsageD3D9);
};
