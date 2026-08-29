#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpReadbackDX12.h"

CGraphicReadbackDX12::CGraphicReadbackDX12()
	: m_pkDevice(NULL)
	, m_pkCommandAllocator(NULL)
	, m_pkCommandList(NULL)
	, m_pkFence(NULL)
	, m_hFenceEvent(NULL)
	, m_uFenceValue(0)
{
}

CGraphicReadbackDX12::~CGraphicReadbackDX12()
{
	Destroy();
}

bool CGraphicReadbackDX12::Create(ID3D12Device* pkDevice)
{
	Destroy();

	if (FAILED(pkDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
												IID_PPV_ARGS(&m_pkCommandAllocator))))
	{
		TraceError("CGraphicReadbackDX12: allocator creation failed.");
		return false;
	}

	if (FAILED(pkDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
										   m_pkCommandAllocator, NULL,
										   IID_PPV_ARGS(&m_pkCommandList))))
	{
		TraceError("CGraphicReadbackDX12: command list creation failed.");
		Destroy();
		return false;
	}

	// Command lists start open; keep it closed between readbacks.
	m_pkCommandList->Close();

	if (FAILED(pkDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pkFence))))
	{
		TraceError("CGraphicReadbackDX12: fence creation failed.");
		Destroy();
		return false;
	}

	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hFenceEvent)
	{
		TraceError("CGraphicReadbackDX12: fence event creation failed.");
		Destroy();
		return false;
	}

	m_pkDevice = pkDevice;
	m_uFenceValue = 0;
	return true;
}

void CGraphicReadbackDX12::Destroy()
{
	if (m_hFenceEvent)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = NULL;
	}

	safe_release(m_pkFence);
	safe_release(m_pkCommandList);
	safe_release(m_pkCommandAllocator);
	m_pkDevice = NULL;
	m_uFenceValue = 0;
}

bool CGraphicReadbackDX12::ReadTexture2D(ID3D12CommandQueue* pkQueue,
										 ID3D12Resource* pkTexture,
										 D3D12_RESOURCE_STATES eCurrentState,
										 void* pvDestPixels,
										 UINT uDestRowPitch)
{
	if (!m_pkDevice || !pkTexture || !pvDestPixels)
		return false;

	const D3D12_RESOURCE_DESC kTextureDesc = pkTexture->GetDesc();

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT kFootprint = {};
	UINT uRowCount = 0;
	UINT64 uRowSizeInBytes = 0;
	UINT64 uTotalBytes = 0;
	m_pkDevice->GetCopyableFootprints(&kTextureDesc, 0, 1, 0, &kFootprint,
									  &uRowCount, &uRowSizeInBytes, &uTotalBytes);

	D3D12_HEAP_PROPERTIES kReadbackHeap = {};
	kReadbackHeap.Type = D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC kBufferDesc = {};
	kBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	kBufferDesc.Width = uTotalBytes;
	kBufferDesc.Height = 1;
	kBufferDesc.DepthOrArraySize = 1;
	kBufferDesc.MipLevels = 1;
	kBufferDesc.SampleDesc.Count = 1;
	kBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* pkReadbackBuffer = NULL;
	if (FAILED(m_pkDevice->CreateCommittedResource(&kReadbackHeap, D3D12_HEAP_FLAG_NONE, &kBufferDesc,
												   D3D12_RESOURCE_STATE_COPY_DEST, NULL,
												   IID_PPV_ARGS(&pkReadbackBuffer))))
	{
		TraceError("CGraphicReadbackDX12: readback buffer creation failed (%llu bytes).",
				   static_cast<unsigned long long>(uTotalBytes));
		return false;
	}

	m_pkCommandAllocator->Reset();
	m_pkCommandList->Reset(m_pkCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER kBarrier = {};
	kBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	kBarrier.Transition.pResource = pkTexture;
	kBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	const bool bNeedsTransition = D3D12_RESOURCE_STATE_COPY_SOURCE != eCurrentState;
	if (bNeedsTransition)
	{
		kBarrier.Transition.StateBefore = eCurrentState;
		kBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		m_pkCommandList->ResourceBarrier(1, &kBarrier);
	}

	D3D12_TEXTURE_COPY_LOCATION kCopySource = {};
	kCopySource.pResource = pkTexture;
	kCopySource.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	kCopySource.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION kCopyDest = {};
	kCopyDest.pResource = pkReadbackBuffer;
	kCopyDest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	kCopyDest.PlacedFootprint = kFootprint;

	m_pkCommandList->CopyTextureRegion(&kCopyDest, 0, 0, 0, &kCopySource, NULL);

	if (bNeedsTransition)
	{
		kBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		kBarrier.Transition.StateAfter = eCurrentState;
		m_pkCommandList->ResourceBarrier(1, &kBarrier);
	}

	if (!__ExecuteAndWait(pkQueue))
	{
		safe_release(pkReadbackBuffer);
		return false;
	}

	BYTE* pbyMapped = NULL;
	if (FAILED(pkReadbackBuffer->Map(0, NULL, reinterpret_cast<void**>(&pbyMapped))))
	{
		TraceError("CGraphicReadbackDX12: map failed.");
		safe_release(pkReadbackBuffer);
		return false;
	}

	// Repack: footprint rows are 256-aligned, the destination is tight.
	BYTE* pbyDest = static_cast<BYTE*>(pvDestPixels);
	const size_t uCopyBytes = static_cast<size_t>(uRowSizeInBytes < uDestRowPitch ? uRowSizeInBytes : uDestRowPitch);
	for (UINT uRow = 0; uRow != uRowCount; ++uRow)
		memcpy(pbyDest + uRow * static_cast<size_t>(uDestRowPitch),
			   pbyMapped + kFootprint.Offset + uRow * static_cast<size_t>(kFootprint.Footprint.RowPitch),
			   uCopyBytes);

	pkReadbackBuffer->Unmap(0, NULL);
	safe_release(pkReadbackBuffer);
	return true;
}

bool CGraphicReadbackDX12::__ExecuteAndWait(ID3D12CommandQueue* pkQueue)
{
	if (FAILED(m_pkCommandList->Close()))
	{
		TraceError("CGraphicReadbackDX12: command list close failed.");
		return false;
	}

	ID3D12CommandList* apkLists[] = { m_pkCommandList };
	pkQueue->ExecuteCommandLists(1, apkLists);

	++m_uFenceValue;
	if (FAILED(pkQueue->Signal(m_pkFence, m_uFenceValue)))
	{
		TraceError("CGraphicReadbackDX12: fence signal failed.");
		return false;
	}

	if (m_pkFence->GetCompletedValue() < m_uFenceValue)
	{
		if (FAILED(m_pkFence->SetEventOnCompletion(m_uFenceValue, m_hFenceEvent)))
		{
			TraceError("CGraphicReadbackDX12: fence wait setup failed.");
			return false;
		}
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

	return true;
}
