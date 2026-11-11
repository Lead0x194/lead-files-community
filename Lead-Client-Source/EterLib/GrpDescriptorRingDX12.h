#pragma once

// Fenced ring over a shader-visible descriptor heap: per-draw SRV and sampler
// tables are staged here, valid for the frame they were written in. Same span
// bookkeeping as the upload ring, counted in descriptors instead of bytes.

#include <d3d12.h>

class CGraphicDescriptorRingDX12
{
	public:
		struct TTable
		{
			D3D12_CPU_DESCRIPTOR_HANDLE	kCPUHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE	kGPUHandle;
		};

		CGraphicDescriptorRingDX12();
		~CGraphicDescriptorRingDX12();

		// eHeapType: CBV_SRV_UAV or SAMPLER; the heap is shader-visible.
		bool	Create(ID3D12Device* pkDevice,
					   D3D12_DESCRIPTOR_HEAP_TYPE eHeapType,
					   UINT uDescriptorCapacity);
		void	Destroy();

		ID3D12DescriptorHeap*	GetHeap() const;

		// Reserves uDescriptorCount contiguous slots; the caller writes the
		// descriptors through kCPUHandle and binds the table at kGPUHandle.
		bool	Allocate(UINT uDescriptorCount, TTable* pkTable);

		void	OnFrameSubmitted(UINT64 uFenceValue);
		void	OnFrameCompleted(UINT64 uCompletedFenceValue);

	private:
		struct TFrameSpan
		{
			UINT	uHead;
			UINT64	uFenceValue;
		};

		enum { MAX_SPANS = 8 };

		ID3D12DescriptorHeap*	m_pkHeap;
		UINT					m_uCapacity;
		UINT					m_uIncrementSize;
		UINT					m_uHead;
		UINT					m_uTail;
		TFrameSpan				m_akSpans[MAX_SPANS];
		UINT					m_uSpanCount;
};
