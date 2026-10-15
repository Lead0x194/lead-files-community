#pragma once

// Transient geometry through the upload ring: what DrawPrimitiveUP and
// DISCARD-locked dynamic buffers did on DX9. Each Write* copies the data into
// ring memory valid for the current frame and returns the buffer view that
// binds it.

#include <d3d12.h>

#include "GrpUploadRingDX12.h"

class CGraphicDynamicDrawDX12
{
	public:
		static bool	WriteVertices(CGraphicUploadRingDX12& rkRing,
								  const void* pvVertices,
								  UINT uStrideBytes,
								  UINT uVertexCount,
								  D3D12_VERTEX_BUFFER_VIEW* pkViewOut);

		static bool	WriteIndices(CGraphicUploadRingDX12& rkRing,
								 const WORD* awIndices,
								 UINT uIndexCount,
								 D3D12_INDEX_BUFFER_VIEW* pkViewOut);

		// 256-aligned constant block for a root CBV binding.
		static bool	WriteConstants(CGraphicUploadRingDX12& rkRing,
								   const void* pvConstants,
								   UINT uByteSize,
								   D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut);
};
