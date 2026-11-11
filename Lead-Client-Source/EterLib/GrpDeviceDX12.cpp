#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpDeviceDX12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

CGraphicDeviceDX12::CGraphicDeviceDX12()
	: m_pkFactory(NULL)
	, m_pkDevice(NULL)
	, m_pkCommandQueue(NULL)
	, m_pkSwapChain(NULL)
	, m_pkRTVHeap(NULL)
	, m_pkDSVHeap(NULL)
	, m_pkDepthBuffer(NULL)
	, m_pkCommandList(NULL)
	, m_pkFence(NULL)
	, m_hFenceEvent(NULL)
	, m_uFrameIndex(0)
	, m_uRTVDescriptorSize(0)
	, m_bCreated(false)
	, m_hCreateWindow(NULL)
	, m_uCreateWidth(0)
	, m_uCreateHeight(0)
	, m_bCreateWindowed(true)
	, m_bDeviceRemoved(false)
{
	for (UINT u = 0; u < FRAME_COUNT; ++u)
	{
		m_apkRenderTargets[u] = NULL;
		m_apkCommandAllocators[u] = NULL;
		m_auFenceValues[u] = 0;
	}
}

CGraphicDeviceDX12::~CGraphicDeviceDX12()
{
	Destroy();
}

bool CGraphicDeviceDX12::IsCreated() const
{
	return m_bCreated;
}

ID3D12Device* CGraphicDeviceDX12::GetDevice() const
{
	return m_pkDevice;
}

ID3D12GraphicsCommandList* CGraphicDeviceDX12::GetCommandList() const
{
	return m_pkCommandList;
}

bool CGraphicDeviceDX12::Create(HWND hWnd, UINT uWidth, UINT uHeight, bool bWindowed)
{
	Destroy();

	if (!__CreateDevice())
	{
		Destroy();
		return false;
	}

	if (!__CreateSwapChain(hWnd, uWidth, uHeight, bWindowed))
	{
		Destroy();
		return false;
	}

	if (!__CreateFrameResources())
	{
		Destroy();
		return false;
	}

	if (!__CreateDepthBuffer(uWidth, uHeight))
	{
		Destroy();
		return false;
	}

	m_hCreateWindow = hWnd;
	m_uCreateWidth = uWidth;
	m_uCreateHeight = uHeight;
	m_bCreateWindowed = bWindowed;
	m_bDeviceRemoved = false;
	m_bCreated = true;
	return true;
}

bool CGraphicDeviceDX12::IsDeviceRemoved() const
{
	return m_bDeviceRemoved;
}

bool CGraphicDeviceDX12::Recreate()
{
	const HWND hWnd = m_hCreateWindow;
	const UINT uWidth = m_uCreateWidth;
	const UINT uHeight = m_uCreateHeight;
	const bool bWindowed = m_bCreateWindowed;

	if (!hWnd)
		return false;

	Destroy();

	if (!Create(hWnd, uWidth, uHeight, bWindowed))
	{
		TraceError("CGraphicDeviceDX12: recreate after device removal failed.");
		return false;
	}

	return true;
}

bool CGraphicDeviceDX12::__CreateDevice()
{
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_pkFactory))))
	{
		TraceError("CGraphicDeviceDX12: CreateDXGIFactory1 failed.");
		return false;
	}

	if (FAILED(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pkDevice))))
	{
		TraceError("CGraphicDeviceDX12: D3D12CreateDevice failed.");
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC kQueueDesc = {};
	kQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	kQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_pkDevice->CreateCommandQueue(&kQueueDesc, IID_PPV_ARGS(&m_pkCommandQueue))))
	{
		TraceError("CGraphicDeviceDX12: CreateCommandQueue failed.");
		return false;
	}

	if (FAILED(m_pkDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pkFence))))
	{
		TraceError("CGraphicDeviceDX12: CreateFence failed.");
		return false;
	}

	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hFenceEvent)
	{
		TraceError("CGraphicDeviceDX12: CreateEvent failed.");
		return false;
	}

	return true;
}

bool CGraphicDeviceDX12::__CreateSwapChain(HWND hWnd, UINT uWidth, UINT uHeight, bool /*bWindowed*/)
{
	// Flip model has no exclusive-fullscreen mode; fullscreen ships as a
	// borderless window sized by the caller.
	DXGI_SWAP_CHAIN_DESC1 kSwapDesc = {};
	kSwapDesc.BufferCount = FRAME_COUNT;
	kSwapDesc.Width = uWidth;
	kSwapDesc.Height = uHeight;
	kSwapDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	kSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	kSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	kSwapDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* pkSwapChain1 = NULL;
	if (FAILED(m_pkFactory->CreateSwapChainForHwnd(m_pkCommandQueue, hWnd, &kSwapDesc, NULL, NULL, &pkSwapChain1)))
	{
		TraceError("CGraphicDeviceDX12: CreateSwapChainForHwnd failed.");
		return false;
	}

	const HRESULT hrQuery = pkSwapChain1->QueryInterface(IID_PPV_ARGS(&m_pkSwapChain));
	pkSwapChain1->Release();
	if (FAILED(hrQuery))
	{
		TraceError("CGraphicDeviceDX12: IDXGISwapChain3 query failed.");
		return false;
	}

	m_pkFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_uFrameIndex = m_pkSwapChain->GetCurrentBackBufferIndex();
	return true;
}

bool CGraphicDeviceDX12::__CreateFrameResources()
{
	D3D12_DESCRIPTOR_HEAP_DESC kRTVHeapDesc = {};
	kRTVHeapDesc.NumDescriptors = FRAME_COUNT;
	kRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	if (FAILED(m_pkDevice->CreateDescriptorHeap(&kRTVHeapDesc, IID_PPV_ARGS(&m_pkRTVHeap))))
	{
		TraceError("CGraphicDeviceDX12: RTV heap creation failed.");
		return false;
	}

	m_uRTVDescriptorSize = m_pkDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE kRTVHandle = m_pkRTVHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT u = 0; u < FRAME_COUNT; ++u)
	{
		if (FAILED(m_pkSwapChain->GetBuffer(u, IID_PPV_ARGS(&m_apkRenderTargets[u]))))
		{
			TraceError("CGraphicDeviceDX12: swapchain GetBuffer(%u) failed.", u);
			return false;
		}
		m_pkDevice->CreateRenderTargetView(m_apkRenderTargets[u], NULL, kRTVHandle);
		kRTVHandle.ptr += m_uRTVDescriptorSize;

		if (FAILED(m_pkDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_apkCommandAllocators[u]))))
		{
			TraceError("CGraphicDeviceDX12: CreateCommandAllocator(%u) failed.", u);
			return false;
		}
	}

	if (FAILED(m_pkDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
											 m_apkCommandAllocators[m_uFrameIndex], NULL,
											 IID_PPV_ARGS(&m_pkCommandList))))
	{
		TraceError("CGraphicDeviceDX12: CreateCommandList failed.");
		return false;
	}

	// Command lists record on creation; close until BeginFrame.
	m_pkCommandList->Close();
	return true;
}

bool CGraphicDeviceDX12::__CreateDepthBuffer(UINT uWidth, UINT uHeight)
{
	D3D12_DESCRIPTOR_HEAP_DESC kDSVHeapDesc = {};
	kDSVHeapDesc.NumDescriptors = 1;
	kDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	if (FAILED(m_pkDevice->CreateDescriptorHeap(&kDSVHeapDesc, IID_PPV_ARGS(&m_pkDSVHeap))))
	{
		TraceError("CGraphicDeviceDX12: DSV heap creation failed.");
		return false;
	}

	D3D12_HEAP_PROPERTIES kHeapProps = {};
	kHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC kDepthDesc = {};
	kDepthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	kDepthDesc.Width = uWidth;
	kDepthDesc.Height = uHeight;
	kDepthDesc.DepthOrArraySize = 1;
	kDepthDesc.MipLevels = 1;
	kDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	kDepthDesc.SampleDesc.Count = 1;
	kDepthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE kClearValue = {};
	kClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	kClearValue.DepthStencil.Depth = 1.0f;

	if (FAILED(m_pkDevice->CreateCommittedResource(&kHeapProps, D3D12_HEAP_FLAG_NONE, &kDepthDesc,
												   D3D12_RESOURCE_STATE_DEPTH_WRITE, &kClearValue,
												   IID_PPV_ARGS(&m_pkDepthBuffer))))
	{
		TraceError("CGraphicDeviceDX12: depth buffer creation failed.");
		return false;
	}

	m_pkDevice->CreateDepthStencilView(m_pkDepthBuffer, NULL, m_pkDSVHeap->GetCPUDescriptorHandleForHeapStart());
	return true;
}

bool CGraphicDeviceDX12::BeginFrame()
{
	if (!m_bCreated)
		return false;

	ID3D12CommandAllocator* pkAllocator = m_apkCommandAllocators[m_uFrameIndex];
	if (FAILED(pkAllocator->Reset()))
		return false;

	if (FAILED(m_pkCommandList->Reset(pkAllocator, NULL)))
		return false;

	D3D12_RESOURCE_BARRIER kBarrier = {};
	kBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	kBarrier.Transition.pResource = m_apkRenderTargets[m_uFrameIndex];
	kBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	kBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	kBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pkCommandList->ResourceBarrier(1, &kBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE kRTVHandle = m_pkRTVHeap->GetCPUDescriptorHandleForHeapStart();
	kRTVHandle.ptr += static_cast<SIZE_T>(m_uFrameIndex) * m_uRTVDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE kDSVHandle = m_pkDSVHeap->GetCPUDescriptorHandleForHeapStart();
	m_pkCommandList->OMSetRenderTargets(1, &kRTVHandle, FALSE, &kDSVHandle);
	return true;
}

void CGraphicDeviceDX12::EndFrame()
{
	if (!m_bCreated)
		return;

	D3D12_RESOURCE_BARRIER kBarrier = {};
	kBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	kBarrier.Transition.pResource = m_apkRenderTargets[m_uFrameIndex];
	kBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	kBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	kBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pkCommandList->ResourceBarrier(1, &kBarrier);

	m_pkCommandList->Close();

	ID3D12CommandList* apkLists[] = { m_pkCommandList };
	m_pkCommandQueue->ExecuteCommandLists(1, apkLists);
}

bool CGraphicDeviceDX12::Present()
{
	if (!m_bCreated)
		return false;

	const HRESULT hr = m_pkSwapChain->Present(1, 0);
	if (DXGI_ERROR_DEVICE_REMOVED == hr || DXGI_ERROR_DEVICE_RESET == hr)
	{
		TraceError("CGraphicDeviceDX12: device removed (0x%08x).", static_cast<unsigned>(m_pkDevice->GetDeviceRemovedReason()));
		m_bDeviceRemoved = true;
		return false;
	}

	__MoveToNextFrame();
	return SUCCEEDED(hr);
}

bool CGraphicDeviceDX12::Resize(UINT uWidth, UINT uHeight)
{
	if (!m_bCreated || !uWidth || !uHeight)
		return false;

	__WaitForGPU();

	for (UINT u = 0; u < FRAME_COUNT; ++u)
	{
		safe_release(m_apkRenderTargets[u]);
		m_auFenceValues[u] = m_auFenceValues[m_uFrameIndex];
	}
	safe_release(m_pkDepthBuffer);
	safe_release(m_pkDSVHeap);

	if (FAILED(m_pkSwapChain->ResizeBuffers(FRAME_COUNT, uWidth, uHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0)))
	{
		TraceError("CGraphicDeviceDX12: ResizeBuffers failed.");
		return false;
	}

	m_uFrameIndex = m_pkSwapChain->GetCurrentBackBufferIndex();
	m_uCreateWidth = uWidth;
	m_uCreateHeight = uHeight;

	D3D12_CPU_DESCRIPTOR_HANDLE kRTVHandle = m_pkRTVHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT u = 0; u < FRAME_COUNT; ++u)
	{
		if (FAILED(m_pkSwapChain->GetBuffer(u, IID_PPV_ARGS(&m_apkRenderTargets[u]))))
			return false;
		m_pkDevice->CreateRenderTargetView(m_apkRenderTargets[u], NULL, kRTVHandle);
		kRTVHandle.ptr += m_uRTVDescriptorSize;
	}

	return __CreateDepthBuffer(uWidth, uHeight);
}

void CGraphicDeviceDX12::__WaitForGPU()
{
	if (!m_pkCommandQueue || !m_pkFence)
		return;

	const UINT64 uFenceValue = m_auFenceValues[m_uFrameIndex] + 1;
	m_pkCommandQueue->Signal(m_pkFence, uFenceValue);

	if (m_pkFence->GetCompletedValue() < uFenceValue)
	{
		m_pkFence->SetEventOnCompletion(uFenceValue, m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

	m_auFenceValues[m_uFrameIndex] = uFenceValue;
}

void CGraphicDeviceDX12::__MoveToNextFrame()
{
	const UINT64 uCurrentFenceValue = m_auFenceValues[m_uFrameIndex] + 1;
	m_pkCommandQueue->Signal(m_pkFence, uCurrentFenceValue);

	m_uFrameIndex = m_pkSwapChain->GetCurrentBackBufferIndex();

	if (m_pkFence->GetCompletedValue() < m_auFenceValues[m_uFrameIndex])
	{
		m_pkFence->SetEventOnCompletion(m_auFenceValues[m_uFrameIndex], m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

	m_auFenceValues[m_uFrameIndex] = uCurrentFenceValue;
}

void CGraphicDeviceDX12::Destroy()
{
	if (m_pkCommandQueue && m_pkFence)
		__WaitForGPU();

	for (UINT u = 0; u < FRAME_COUNT; ++u)
	{
		safe_release(m_apkRenderTargets[u]);
		safe_release(m_apkCommandAllocators[u]);
		m_auFenceValues[u] = 0;
	}

	safe_release(m_pkCommandList);
	safe_release(m_pkDepthBuffer);
	safe_release(m_pkDSVHeap);
	safe_release(m_pkRTVHeap);
	safe_release(m_pkSwapChain);
	safe_release(m_pkFence);
	safe_release(m_pkCommandQueue);
	safe_release(m_pkDevice);
	safe_release(m_pkFactory);

	if (m_hFenceEvent)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = NULL;
	}

	m_uFrameIndex = 0;
	m_uRTVDescriptorSize = 0;
	m_bCreated = false;
}
