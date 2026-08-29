#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpResourceUploadDX12.h"

CGraphicResourceUploaderDX12::CGraphicResourceUploaderDX12()
	: m_pkDevice(NULL)
	, m_pkCommandAllocator(NULL)
	, m_pkCommandList(NULL)
	, m_pkFence(NULL)
	, m_hFenceEvent(NULL)
	, m_uFenceValue(0)
{
}

CGraphicResourceUploaderDX12::~CGraphicResourceUploaderDX12()
{
	Destroy();
}

bool CGraphicResourceUploaderDX12::Create(ID3D12Device* pkDevice)
{
	Destroy();

	if (FAILED(pkDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
												IID_PPV_ARGS(&m_pkCommandAllocator))))
	{
		TraceError("CGraphicResourceUploaderDX12: allocator creation failed.");
		return false;
	}

	if (FAILED(pkDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
										   m_pkCommandAllocator, NULL,
										   IID_PPV_ARGS(&m_pkCommandList))))
	{
		TraceError("CGraphicResourceUploaderDX12: command list creation failed.");
		Destroy();
		return false;
	}

	// Command lists start open; keep it closed between uploads.
	m_pkCommandList->Close();

	if (FAILED(pkDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pkFence))))
	{
		TraceError("CGraphicResourceUploaderDX12: fence creation failed.");
		Destroy();
		return false;
	}

	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hFenceEvent)
	{
		TraceError("CGraphicResourceUploaderDX12: fence event creation failed.");
		Destroy();
		return false;
	}

	m_pkDevice = pkDevice;
	m_uFenceValue = 0;
	return true;
}

void CGraphicResourceUploaderDX12::Destroy()
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

ID3D12Resource* CGraphicResourceUploaderDX12::CreateStaticBuffer(ID3D12CommandQueue* pkQueue,
																 const void* pvData,
																 UINT64 uByteSize,
																 D3D12_RESOURCE_STATES eFinalState)
{
	if (!m_pkDevice || !pvData || !uByteSize)
		return NULL;

	D3D12_HEAP_PROPERTIES kDefaultHeap = {};
	kDefaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC kBufferDesc = {};
	kBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	kBufferDesc.Width = uByteSize;
	kBufferDesc.Height = 1;
	kBufferDesc.DepthOrArraySize = 1;
	kBufferDesc.MipLevels = 1;
	kBufferDesc.SampleDesc.Count = 1;
	kBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* pkBuffer = NULL;
	if (FAILED(m_pkDevice->CreateCommittedResource(&kDefaultHeap, D3D12_HEAP_FLAG_NONE, &kBufferDesc,
												   D3D12_RESOURCE_STATE_COPY_DEST, NULL,
												   IID_PPV_ARGS(&pkBuffer))))
	{
		TraceError("CGraphicResourceUploaderDX12: static buffer creation failed (%llu bytes).",
				   static_cast<unsigned long long>(uByteSize));
		return NULL;
	}

	D3D12_HEAP_PROPERTIES kUploadHeap = {};
	kUploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

	ID3D12Resource* pkStaging = NULL;
	if (FAILED(m_pkDevice->CreateCommittedResource(&kUploadHeap, D3D12_HEAP_FLAG_NONE, &kBufferDesc,
												   D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
												   IID_PPV_ARGS(&pkStaging))))
	{
		TraceError("CGraphicResourceUploaderDX12: staging buffer creation failed.");
		safe_release(pkBuffer);
		return NULL;
	}

	void* pvMapped = NULL;
	if (FAILED(pkStaging->Map(0, NULL, &pvMapped)))
	{
		TraceError("CGraphicResourceUploaderDX12: staging map failed.");
		safe_release(pkStaging);
		safe_release(pkBuffer);
		return NULL;
	}

	memcpy(pvMapped, pvData, static_cast<size_t>(uByteSize));
	pkStaging->Unmap(0, NULL);

	m_pkCommandAllocator->Reset();
	m_pkCommandList->Reset(m_pkCommandAllocator, NULL);
	m_pkCommandList->CopyBufferRegion(pkBuffer, 0, pkStaging, 0, uByteSize);

	D3D12_RESOURCE_BARRIER kBarrier = {};
	kBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	kBarrier.Transition.pResource = pkBuffer;
	kBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	kBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	kBarrier.Transition.StateAfter = eFinalState;
	m_pkCommandList->ResourceBarrier(1, &kBarrier);

	if (!__ExecuteAndWait(pkQueue))
	{
		safe_release(pkStaging);
		safe_release(pkBuffer);
		return NULL;
	}

	safe_release(pkStaging);
	return pkBuffer;
}

ID3D12Resource* CGraphicResourceUploaderDX12::CreateTexture2D(ID3D12CommandQueue* pkQueue,
															  UINT uWidth,
															  UINT uHeight,
															  DXGI_FORMAT eFormat,
															  const void* pvPixels,
															  UINT uSrcRowPitch)
{
	if (!m_pkDevice || !pvPixels || !uWidth || !uHeight)
		return NULL;

	D3D12_HEAP_PROPERTIES kDefaultHeap = {};
	kDefaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC kTextureDesc = {};
	kTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	kTextureDesc.Width = uWidth;
	kTextureDesc.Height = uHeight;
	kTextureDesc.DepthOrArraySize = 1;
	kTextureDesc.MipLevels = 1;
	kTextureDesc.Format = eFormat;
	kTextureDesc.SampleDesc.Count = 1;

	ID3D12Resource* pkTexture = NULL;
	if (FAILED(m_pkDevice->CreateCommittedResource(&kDefaultHeap, D3D12_HEAP_FLAG_NONE, &kTextureDesc,
												   D3D12_RESOURCE_STATE_COPY_DEST, NULL,
												   IID_PPV_ARGS(&pkTexture))))
	{
		TraceError("CGraphicResourceUploaderDX12: texture creation failed (%ux%u fmt %d).",
				   uWidth, uHeight, static_cast<int>(eFormat));
		return NULL;
	}

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT kFootprint = {};
	UINT uRowCount = 0;
	UINT64 uRowSizeInBytes = 0;
	UINT64 uTotalBytes = 0;
	m_pkDevice->GetCopyableFootprints(&kTextureDesc, 0, 1, 0, &kFootprint,
									  &uRowCount, &uRowSizeInBytes, &uTotalBytes);

	D3D12_HEAP_PROPERTIES kUploadHeap = {};
	kUploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC kStagingDesc = {};
	kStagingDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	kStagingDesc.Width = uTotalBytes;
	kStagingDesc.Height = 1;
	kStagingDesc.DepthOrArraySize = 1;
	kStagingDesc.MipLevels = 1;
	kStagingDesc.SampleDesc.Count = 1;
	kStagingDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* pkStaging = NULL;
	if (FAILED(m_pkDevice->CreateCommittedResource(&kUploadHeap, D3D12_HEAP_FLAG_NONE, &kStagingDesc,
												   D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
												   IID_PPV_ARGS(&pkStaging))))
	{
		TraceError("CGraphicResourceUploaderDX12: texture staging creation failed.");
		safe_release(pkTexture);
		return NULL;
	}

	BYTE* pbyMapped = NULL;
	if (FAILED(pkStaging->Map(0, NULL, reinterpret_cast<void**>(&pbyMapped))))
	{
		TraceError("CGraphicResourceUploaderDX12: texture staging map failed.");
		safe_release(pkStaging);
		safe_release(pkTexture);
		return NULL;
	}

	// Repack rows: the footprint pitch is 256-aligned, the source is tight.
	const BYTE* pbySource = static_cast<const BYTE*>(pvPixels);
	const size_t uCopyBytes = static_cast<size_t>(uRowSizeInBytes < uSrcRowPitch ? uRowSizeInBytes : uSrcRowPitch);
	for (UINT uRow = 0; uRow != uRowCount; ++uRow)
		memcpy(pbyMapped + kFootprint.Offset + uRow * static_cast<size_t>(kFootprint.Footprint.RowPitch),
			   pbySource + uRow * static_cast<size_t>(uSrcRowPitch),
			   uCopyBytes);

	pkStaging->Unmap(0, NULL);

	m_pkCommandAllocator->Reset();
	m_pkCommandList->Reset(m_pkCommandAllocator, NULL);

	D3D12_TEXTURE_COPY_LOCATION kCopyDest = {};
	kCopyDest.pResource = pkTexture;
	kCopyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	kCopyDest.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION kCopySource = {};
	kCopySource.pResource = pkStaging;
	kCopySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	kCopySource.PlacedFootprint = kFootprint;

	m_pkCommandList->CopyTextureRegion(&kCopyDest, 0, 0, 0, &kCopySource, NULL);

	D3D12_RESOURCE_BARRIER kBarrier = {};
	kBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	kBarrier.Transition.pResource = pkTexture;
	kBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	kBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	kBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_pkCommandList->ResourceBarrier(1, &kBarrier);

	if (!__ExecuteAndWait(pkQueue))
	{
		safe_release(pkStaging);
		safe_release(pkTexture);
		return NULL;
	}

	safe_release(pkStaging);
	return pkTexture;
}

bool CGraphicResourceUploaderDX12::__ExecuteAndWait(ID3D12CommandQueue* pkQueue)
{
	if (FAILED(m_pkCommandList->Close()))
	{
		TraceError("CGraphicResourceUploaderDX12: command list close failed.");
		return false;
	}

	ID3D12CommandList* apkLists[] = { m_pkCommandList };
	pkQueue->ExecuteCommandLists(1, apkLists);

	++m_uFenceValue;
	if (FAILED(pkQueue->Signal(m_pkFence, m_uFenceValue)))
	{
		TraceError("CGraphicResourceUploaderDX12: fence signal failed.");
		return false;
	}

	if (m_pkFence->GetCompletedValue() < m_uFenceValue)
	{
		if (FAILED(m_pkFence->SetEventOnCompletion(m_uFenceValue, m_hFenceEvent)))
		{
			TraceError("CGraphicResourceUploaderDX12: fence wait setup failed.");
			return false;
		}
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

	return true;
}
