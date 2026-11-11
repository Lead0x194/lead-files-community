#pragma once

// Fullscreen gamma pass: DX12 has no SetGammaRamp, so the brightness slider
// becomes a post-pass multiplying the scene by the gamma factor - the same
// math the D3D9 ramp applied per channel. Owns its root signature and PSO;
// the caller binds the scene SRV table and records into its command list.

#include <d3d12.h>

class CGraphicGammaPassDX12
{
	public:
		CGraphicGammaPassDX12();
		~CGraphicGammaPassDX12();

		bool	Create(ID3D12Device* pkDevice);
		void	Destroy();

		bool	IsCreated() const;

		// Draws the fullscreen triangle; viewport, scissor and render target
		// are frame state the caller has already set. kSceneTable points at
		// the scene texture SRV in the bound shader-visible heap.
		void	Record(ID3D12GraphicsCommandList* pkCommandList,
					   D3D12_GPU_DESCRIPTOR_HANDLE kSceneTable,
					   float fGammaFactor);

	private:
		ID3D12RootSignature*	m_pkRootSignature;
		ID3D12PipelineState*	m_pkPipelineState;
};
