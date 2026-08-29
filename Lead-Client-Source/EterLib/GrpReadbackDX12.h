#pragma once

// One-shot GPU-to-CPU texture readback for the DX12 backend: the screenshot
// and capture paths that called GetRenderTargetData on D3D9. Copies through a
// transient readback buffer on its own command list, blocking until done.

#include <d3d12.h>

class CGraphicReadbackDX12
{
	public:
		CGraphicReadbackDX12();
		~CGraphicReadbackDX12();

		bool	Create(ID3D12Device* pkDevice);
		void	Destroy();

		// Reads subresource 0 of pkTexture (currently in eCurrentState, which
		// is restored) into pvDestPixels, uDestRowPitch bytes per tight row.
		bool	ReadTexture2D(ID3D12CommandQueue* pkQueue,
							  ID3D12Resource* pkTexture,
							  D3D12_RESOURCE_STATES eCurrentState,
							  void* pvDestPixels,
							  UINT uDestRowPitch);

	private:
		bool	__ExecuteAndWait(ID3D12CommandQueue* pkQueue);

		ID3D12Device*				m_pkDevice;
		ID3D12CommandAllocator*		m_pkCommandAllocator;
		ID3D12GraphicsCommandList*	m_pkCommandList;
		ID3D12Fence*				m_pkFence;
		HANDLE						m_hFenceEvent;
		UINT64						m_uFenceValue;
};
