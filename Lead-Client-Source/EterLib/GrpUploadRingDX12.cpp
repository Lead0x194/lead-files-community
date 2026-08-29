#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpUploadRingDX12.h"

CGraphicUploadRingDX12::CGraphicUploadRingDX12()
	: m_pkBuffer(NULL)
	, m_pbyMapped(NULL)
	, m_uByteSize(0)
	, m_uHead(0)
	, m_uTail(0)
	, m_uSpanCount(0)
{
}

CGraphicUploadRingDX12::~CGraphicUploadRingDX12()
{
	Destroy();
}

bool CGraphicUploadRingDX12::Create(ID3D12Device* pkDevice, UINT64 uByteSize)
{
	Destroy();

	D3D12_HEAP_PROPERTIES kHeapProps = {};
	kHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC kBufferDesc = {};
	kBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	kBufferDesc.Width = uByteSize;
	kBufferDesc.Height = 1;
	kBufferDesc.DepthOrArraySize = 1;
	kBufferDesc.MipLevels = 1;
	kBufferDesc.SampleDesc.Count = 1;
	kBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	if (FAILED(pkDevice->CreateCommittedResource(&kHeapProps, D3D12_HEAP_FLAG_NONE, &kBufferDesc,
												 D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
												 IID_PPV_ARGS(&m_pkBuffer))))
	{
		TraceError("CGraphicUploadRingDX12: buffer creation failed (%llu bytes).",
				   static_cast<unsigned long long>(uByteSize));
		return false;
	}

	// Upload heaps stay persistently mapped.
	if (FAILED(m_pkBuffer->Map(0, NULL, reinterpret_cast<void**>(&m_pbyMapped))))
	{
		TraceError("CGraphicUploadRingDX12: Map failed.");
		Destroy();
		return false;
	}

	m_uByteSize = uByteSize;
	m_uHead = 0;
	m_uTail = 0;
	m_uSpanCount = 0;
	return true;
}

void CGraphicUploadRingDX12::Destroy()
{
	if (m_pkBuffer && m_pbyMapped)
		m_pkBuffer->Unmap(0, NULL);

	safe_release(m_pkBuffer);
	m_pbyMapped = NULL;
	m_uByteSize = 0;
	m_uHead = 0;
	m_uTail = 0;
	m_uSpanCount = 0;
}

bool CGraphicUploadRingDX12::Allocate(UINT64 uByteSize, UINT64 uAlignment, TAllocation* pkAllocation)
{
	if (!m_pkBuffer || !uByteSize || uByteSize > m_uByteSize)
		return false;

	UINT64 uAligned = (m_uHead + (uAlignment - 1)) & ~(uAlignment - 1);

	// Wrap when the request does not fit the buffer tail.
	if (uAligned + uByteSize > m_uByteSize)
		uAligned = 0;

	// The region [tail, head) is still owned by in-flight frames; refuse to
	// overrun it - the caller then flushes or the ring was sized too small.
	if (m_uSpanCount > 0)
	{
		const UINT64 uOldestTail = m_uTail;
		const bool bWrapped = uAligned < m_uHead;
		if (bWrapped && uAligned + uByteSize > uOldestTail)
		{
			TraceError("CGraphicUploadRingDX12: ring exhausted (%llu in flight).",
					   static_cast<unsigned long long>(m_uSpanCount));
			return false;
		}
	}

	pkAllocation->pvCPUAddress = m_pbyMapped + uAligned;
	pkAllocation->uGPUAddress = m_pkBuffer->GetGPUVirtualAddress() + uAligned;
	pkAllocation->pkResource = m_pkBuffer;
	pkAllocation->uOffset = uAligned;

	m_uHead = uAligned + uByteSize;
	return true;
}

void CGraphicUploadRingDX12::OnFrameSubmitted(UINT64 uFenceValue)
{
	if (m_uSpanCount >= MAX_SPANS)
	{
		// Oldest span merges into its successor; allocation safety only ever
		// errs toward keeping memory alive longer.
		for (UINT u = 1; u < m_uSpanCount; ++u)
			m_akSpans[u - 1] = m_akSpans[u];
		--m_uSpanCount;
	}

	m_akSpans[m_uSpanCount].uHead = m_uHead;
	m_akSpans[m_uSpanCount].uFenceValue = uFenceValue;
	++m_uSpanCount;
}

void CGraphicUploadRingDX12::OnFrameCompleted(UINT64 uCompletedFenceValue)
{
	UINT uReleased = 0;
	while (uReleased < m_uSpanCount && m_akSpans[uReleased].uFenceValue <= uCompletedFenceValue)
	{
		m_uTail = m_akSpans[uReleased].uHead;
		++uReleased;
	}

	if (uReleased > 0)
	{
		for (UINT u = uReleased; u < m_uSpanCount; ++u)
			m_akSpans[u - uReleased] = m_akSpans[u];
		m_uSpanCount -= uReleased;
	}

	if (0 == m_uSpanCount)
		m_uTail = m_uHead;
}
