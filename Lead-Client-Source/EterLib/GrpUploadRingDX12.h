#pragma once

// Fenced per-frame upload ring for the DX12 backend: replaces every
// D3DLOCK_DISCARD dynamic lock and DrawPrimitiveUP-style transient upload.
// Allocations are valid for the frame they were made in; the ring reclaims
// space once the GPU passes the matching fence value.

#include <d3d12.h>

class CGraphicUploadRingDX12
{
	public:
		struct TAllocation
		{
			void*						pvCPUAddress;
			D3D12_GPU_VIRTUAL_ADDRESS	uGPUAddress;
			ID3D12Resource*				pkResource;
			UINT64						uOffset;
		};

		CGraphicUploadRingDX12();
		~CGraphicUploadRingDX12();

		bool	Create(ID3D12Device* pkDevice, UINT64 uByteSize);
		void	Destroy();

		// Reserve uByteSize bytes aligned to uAlignment (256 for constant data,
		// otherwise 4); fails only when a single request exceeds the ring.
		bool	Allocate(UINT64 uByteSize, UINT64 uAlignment, TAllocation* pkAllocation);

		// Frame lifecycle: OnFrameCompleted(fence value the GPU passed) frees
		// the oldest in-flight region; OnFrameSubmitted tags the current head.
		void	OnFrameSubmitted(UINT64 uFenceValue);
		void	OnFrameCompleted(UINT64 uCompletedFenceValue);

	private:
		struct TFrameSpan
		{
			UINT64	uHead;
			UINT64	uFenceValue;
		};

		enum { MAX_SPANS = 8 };

		ID3D12Resource*	m_pkBuffer;
		BYTE*			m_pbyMapped;
		UINT64			m_uByteSize;
		UINT64			m_uHead;
		UINT64			m_uTail;
		TFrameSpan		m_akSpans[MAX_SPANS];
		UINT			m_uSpanCount;
};
