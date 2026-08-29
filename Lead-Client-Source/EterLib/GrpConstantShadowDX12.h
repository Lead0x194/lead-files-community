#pragma once

// CPU shadow of the D3D9 shader constant registers (VS c0-c31, PS c0-c7).
// Set* mirrors SetVertexShaderConstantF semantics; Flush* snapshots the block
// into ring memory and returns the GPU address for the root CBV binding.
// Ring memory is per-frame, so OnFrameBegin re-dirties both blocks.

#include <d3d12.h>

#include "GrpUploadRingDX12.h"

class CGraphicConstantShadowDX12
{
	public:
		enum
		{
			VS_REGISTER_COUNT = 32,
			PS_REGISTER_COUNT = 8,
		};

		CGraphicConstantShadowDX12();

		void	Reset();
		void	OnFrameBegin();

		bool	SetVSConstants(UINT uStartRegister, const float* afData, UINT uVector4Count);
		bool	SetPSConstants(UINT uStartRegister, const float* afData, UINT uVector4Count);

		// Uploads only when dirty; otherwise returns the address of the last
		// flush, which stays valid for the rest of the frame.
		bool	FlushVS(CGraphicUploadRingDX12& rkRing, D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut);
		bool	FlushPS(CGraphicUploadRingDX12& rkRing, D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut);

	private:
		float						m_afVSConstants[VS_REGISTER_COUNT][4];
		float						m_afPSConstants[PS_REGISTER_COUNT][4];
		D3D12_GPU_VIRTUAL_ADDRESS	m_uVSAddress;
		D3D12_GPU_VIRTUAL_ADDRESS	m_uPSAddress;
		bool						m_bVSDirty;
		bool						m_bPSDirty;
};
