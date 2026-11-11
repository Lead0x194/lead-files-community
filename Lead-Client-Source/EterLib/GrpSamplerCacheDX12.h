#pragma once

// Persistent sampler-table cache: each unique pair of stage-0/stage-1 sampler
// keys gets two adjacent descriptors in a shader-visible heap, written once
// and rebound by GPU handle. Sampler heaps are tiny (2048 max), so entries
// live for the device lifetime.

#include <d3d12.h>
#include <unordered_map>
#include <vector>

#include "GrpSamplerKeyDX12.h"

class CGraphicSamplerCacheDX12
{
	public:
		CGraphicSamplerCacheDX12();
		~CGraphicSamplerCacheDX12();

		bool	Create(ID3D12Device* pkDevice, UINT uTableCapacity);
		void	Destroy();

		ID3D12DescriptorHeap*	GetHeap() const;

		// GPU handle of the s0+s1 table for the key pair; writes the two
		// descriptors on first sight. Fails once the heap is full.
		bool	GetTable(const CGraphicSamplerKeyDX12& rkKey0,
						 const CGraphicSamplerKeyDX12& rkKey1,
						 D3D12_GPU_DESCRIPTOR_HANDLE* pkTableOut);

		UINT	GetCount() const;

	private:
		struct TEntry
		{
			UINT64	uHash0;
			UINT64	uHash1;
			UINT	uTableIndex;
		};

		// Chained on both key hashes so combined-hash collisions stay correct.
		typedef std::unordered_map<UINT64, std::vector<TEntry> > TTableMap;

		ID3D12Device*			m_pkDevice;
		ID3D12DescriptorHeap*	m_pkHeap;
		UINT					m_uTableCapacity;
		UINT					m_uTableCount;
		UINT					m_uIncrementSize;
		TTableMap				m_kTableMap;
};
