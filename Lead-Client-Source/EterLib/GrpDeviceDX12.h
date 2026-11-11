#pragma once

// DX12 backend skeleton: device, queue, flip-model swapchain and per-frame
// fencing. Not reachable until CGraphicDevice routes BACKEND_DX12 here; the
// draw translation (state manager backend) plugs in on top of this spine.

#include <d3d12.h>
#include <dxgi1_4.h>

class CGraphicDeviceDX12
{
	public:
		enum
		{
			FRAME_COUNT = 3,
		};

		CGraphicDeviceDX12();
		~CGraphicDeviceDX12();

		bool	Create(HWND hWnd, UINT uWidth, UINT uHeight, bool bWindowed);
		void	Destroy();

		bool	IsCreated() const;

		bool	BeginFrame();
		void	EndFrame();
		bool	Present();

		bool	Resize(UINT uWidth, UINT uHeight);

		// TDR path: Present flags removal; the frame loop then rebuilds the
		// whole device with the parameters remembered from Create.
		bool	IsDeviceRemoved() const;
		bool	Recreate();

		ID3D12Device*				GetDevice() const;
		ID3D12GraphicsCommandList*	GetCommandList() const;

	private:
		bool	__CreateDevice();
		bool	__CreateSwapChain(HWND hWnd, UINT uWidth, UINT uHeight, bool bWindowed);
		bool	__CreateFrameResources();
		bool	__CreateDepthBuffer(UINT uWidth, UINT uHeight);
		void	__WaitForGPU();
		void	__MoveToNextFrame();

		IDXGIFactory4*				m_pkFactory;
		ID3D12Device*				m_pkDevice;
		ID3D12CommandQueue*			m_pkCommandQueue;
		IDXGISwapChain3*			m_pkSwapChain;
		ID3D12DescriptorHeap*		m_pkRTVHeap;
		ID3D12DescriptorHeap*		m_pkDSVHeap;
		ID3D12Resource*				m_apkRenderTargets[FRAME_COUNT];
		ID3D12Resource*				m_pkDepthBuffer;
		ID3D12CommandAllocator*		m_apkCommandAllocators[FRAME_COUNT];
		ID3D12GraphicsCommandList*	m_pkCommandList;
		ID3D12Fence*				m_pkFence;
		HANDLE						m_hFenceEvent;
		UINT64						m_auFenceValues[FRAME_COUNT];
		UINT						m_uFrameIndex;
		UINT						m_uRTVDescriptorSize;
		bool						m_bCreated;

		// Creation parameters kept for Recreate; survive Destroy.
		HWND						m_hCreateWindow;
		UINT						m_uCreateWidth;
		UINT						m_uCreateHeight;
		bool						m_bCreateWindowed;
		bool						m_bDeviceRemoved;
};
