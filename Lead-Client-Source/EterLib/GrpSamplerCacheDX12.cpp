#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpSamplerCacheDX12.h"

CGraphicSamplerCacheDX12::CGraphicSamplerCacheDX12()
	: m_pkDevice(NULL)
	, m_pkHeap(NULL)
	, m_uTableCapacity(0)
	, m_uTableCount(0)
	, m_uIncrementSize(0)
{
}

CGraphicSamplerCacheDX12::~CGraphicSamplerCacheDX12()
{
	Destroy();
}

bool CGraphicSamplerCacheDX12::Create(ID3D12Device* pkDevice, UINT uTableCapacity)
{
	Destroy();

	D3D12_DESCRIPTOR_HEAP_DESC kHeapDesc = {};
	kHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	kHeapDesc.NumDescriptors = uTableCapacity * 2;
	kHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	if (FAILED(pkDevice->CreateDescriptorHeap(&kHeapDesc, IID_PPV_ARGS(&m_pkHeap))))
	{
		TraceError("CGraphicSamplerCacheDX12: heap creation failed (%u tables).", uTableCapacity);
		return false;
	}

	m_pkDevice = pkDevice;
	m_uTableCapacity = uTableCapacity;
	m_uTableCount = 0;
	m_uIncrementSize = pkDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	return true;
}

void CGraphicSamplerCacheDX12::Destroy()
{
	safe_release(m_pkHeap);
	m_kTableMap.clear();
	m_pkDevice = NULL;
	m_uTableCapacity = 0;
	m_uTableCount = 0;
	m_uIncrementSize = 0;
}

ID3D12DescriptorHeap* CGraphicSamplerCacheDX12::GetHeap() const
{
	return m_pkHeap;
}

bool CGraphicSamplerCacheDX12::GetTable(const CGraphicSamplerKeyDX12& rkKey0,
										const CGraphicSamplerKeyDX12& rkKey1,
										D3D12_GPU_DESCRIPTOR_HANDLE* pkTableOut)
{
	if (!m_pkHeap)
		return false;

	const UINT64 uHash0 = rkKey0.Hash();
	const UINT64 uHash1 = rkKey1.Hash();
	const UINT64 uCombined = uHash0 * 1099511628211ULL ^ uHash1;

	std::vector<TEntry>& rkChain = m_kTableMap[uCombined];

	UINT uTableIndex = 0xFFFFFFFF;
	for (size_t uPos = 0; uPos != rkChain.size(); ++uPos)
	{
		if (rkChain[uPos].uHash0 == uHash0 && rkChain[uPos].uHash1 == uHash1)
		{
			uTableIndex = rkChain[uPos].uTableIndex;
			break;
		}
	}

	if (0xFFFFFFFF == uTableIndex)
	{
		if (m_uTableCount >= m_uTableCapacity)
		{
			TraceError("CGraphicSamplerCacheDX12: heap full (%u tables).", m_uTableCapacity);
			return false;
		}

		uTableIndex = m_uTableCount++;

		D3D12_CPU_DESCRIPTOR_HANDLE kWriteHandle = m_pkHeap->GetCPUDescriptorHandleForHeapStart();
		kWriteHandle.ptr += static_cast<SIZE_T>(uTableIndex) * 2 * m_uIncrementSize;

		D3D12_SAMPLER_DESC kSamplerDesc;
		rkKey0.ToSamplerDesc(&kSamplerDesc);
		m_pkDevice->CreateSampler(&kSamplerDesc, kWriteHandle);

		kWriteHandle.ptr += m_uIncrementSize;
		rkKey1.ToSamplerDesc(&kSamplerDesc);
		m_pkDevice->CreateSampler(&kSamplerDesc, kWriteHandle);

		TEntry kEntry;
		kEntry.uHash0 = uHash0;
		kEntry.uHash1 = uHash1;
		kEntry.uTableIndex = uTableIndex;
		rkChain.push_back(kEntry);
	}

	*pkTableOut = m_pkHeap->GetGPUDescriptorHandleForHeapStart();
	pkTableOut->ptr += static_cast<UINT64>(uTableIndex) * 2 * m_uIncrementSize;
	return true;
}

UINT CGraphicSamplerCacheDX12::GetCount() const
{
	return m_uTableCount;
}
