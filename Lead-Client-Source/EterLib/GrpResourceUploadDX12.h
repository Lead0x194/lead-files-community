#pragma once

// One-shot staging uploader for static DX12 resources: creates default-heap
// buffers/textures and fills them through a transient upload buffer on its
// own copy command list, blocking until the GPU finishes. Static data only -
// per-frame dynamic data goes through the upload ring instead.

#include <d3d12.h>

class CGraphicResourceUploaderDX12
{
	public:
		CGraphicResourceUploaderDX12();
		~CGraphicResourceUploaderDX12();

		bool	Create(ID3D12Device* pkDevice);
		void	Destroy();

		// Default-heap buffer filled with pvData, transitioned to eFinalState
		// (vertex/constant or index read). NULL on failure.
		ID3D12Resource*	CreateStaticBuffer(ID3D12CommandQueue* pkQueue,
										   const void* pvData,
										   UINT64 uByteSize,
										   D3D12_RESOURCE_STATES eFinalState);

		// Single-mip 2D texture transitioned to pixel-shader readable.
		// uSrcRowPitch is bytes per pixel row (per block row for BC formats).
		ID3D12Resource*	CreateTexture2D(ID3D12CommandQueue* pkQueue,
										UINT uWidth,
										UINT uHeight,
										DXGI_FORMAT eFormat,
										const void* pvPixels,
										UINT uSrcRowPitch);

	private:
		bool	__ExecuteAndWait(ID3D12CommandQueue* pkQueue);

		ID3D12Device*				m_pkDevice;
		ID3D12CommandAllocator*		m_pkCommandAllocator;
		ID3D12GraphicsCommandList*	m_pkCommandList;
		ID3D12Fence*				m_pkFence;
		HANDLE						m_hFenceEvent;
		UINT64						m_uFenceValue;
};
