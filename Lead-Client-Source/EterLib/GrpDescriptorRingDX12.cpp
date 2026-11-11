#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpDescriptorRingDX12.h"

CGraphicDescriptorRingDX12::CGraphicDescriptorRingDX12()
	: m_pkHeap(NULL)
	, m_uCapacity(0)
	, m_uIncrementSize(0)
	, m_uHead(0)
	, m_uTail(0)
	, m_uSpanCount(0)
{
}

CGraphicDescriptorRingDX12::~CGraphicDescriptorRingDX12()
{
	Destroy();
}

bool CGraphicDescriptorRingDX12::Create(ID3D12Device* pkDevice,
										D3D12_DESCRIPTOR_HEAP_TYPE eHeapType,
										UINT uDescriptorCapacity)
{
	Destroy();

	D3D12_DESCRIPTOR_HEAP_DESC kHeapDesc = {};
	kHeapDesc.Type = eHeapType;
	kHeapDesc.NumDescriptors = uDescriptorCapacity;
	kHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	if (FAILED(pkDevice->CreateDescriptorHeap(&kHeapDesc, IID_PPV_ARGS(&m_pkHeap))))
	{
		TraceError("CGraphicDescriptorRingDX12: heap creation failed (type %d, %u descriptors).",
				   static_cast<int>(eHeapType), uDescriptorCapacity);
		return false;
	}

	m_uCapacity = uDescriptorCapacity;
	m_uIncrementSize = pkDevice->GetDescriptorHandleIncrementSize(eHeapType);
	m_uHead = 0;
	m_uTail = 0;
	m_uSpanCount = 0;
	return true;
}

void CGraphicDescriptorRingDX12::Destroy()
{
	safe_release(m_pkHeap);
	m_uCapacity = 0;
	m_uIncrementSize = 0;
	m_uHead = 0;
	m_uTail = 0;
	m_uSpanCount = 0;
}

ID3D12DescriptorHeap* CGraphicDescriptorRingDX12::GetHeap() const
{
	return m_pkHeap;
}

bool CGraphicDescriptorRingDX12::Allocate(UINT uDescriptorCount, TTable* pkTable)
{
	if (!m_pkHeap || !uDescriptorCount || uDescriptorCount > m_uCapacity)
		return false;

	UINT uStart = m_uHead;

	// Tables must be contiguous; wrap when the heap tail cannot hold one.
	if (uStart + uDescriptorCount > m_uCapacity)
		uStart = 0;

	// The region [tail, head) still belongs to in-flight frames.
	if (m_uSpanCount > 0)
	{
		const bool bWrapped = uStart < m_uHead;
		if (bWrapped && uStart + uDescriptorCount > m_uTail)
		{
			TraceError("CGraphicDescriptorRingDX12: ring exhausted (%u in flight).", m_uSpanCount);
			return false;
		}
	}

	pkTable->kCPUHandle = m_pkHeap->GetCPUDescriptorHandleForHeapStart();
	pkTable->kCPUHandle.ptr += static_cast<SIZE_T>(uStart) * m_uIncrementSize;
	pkTable->kGPUHandle = m_pkHeap->GetGPUDescriptorHandleForHeapStart();
	pkTable->kGPUHandle.ptr += static_cast<UINT64>(uStart) * m_uIncrementSize;

	m_uHead = uStart + uDescriptorCount;
	return true;
}

void CGraphicDescriptorRingDX12::OnFrameSubmitted(UINT64 uFenceValue)
{
	if (m_uSpanCount >= MAX_SPANS)
	{
		// Merge the oldest span into its successor; memory only stays
		// reserved longer, never gets freed early.
		for (UINT u = 1; u < m_uSpanCount; ++u)
			m_akSpans[u - 1] = m_akSpans[u];
		--m_uSpanCount;
	}

	m_akSpans[m_uSpanCount].uHead = m_uHead;
	m_akSpans[m_uSpanCount].uFenceValue = uFenceValue;
	++m_uSpanCount;
}

void CGraphicDescriptorRingDX12::OnFrameCompleted(UINT64 uCompletedFenceValue)
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
