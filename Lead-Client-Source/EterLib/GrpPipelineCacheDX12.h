#pragma once

// Pipeline-state cache: hashes CGraphicPipelineKeyDX12 snapshots to compiled
// PSOs so every unique render-state/shader combination is built exactly once.
// Hash buckets chain on full key equality, so collisions stay correct.

#include <d3d12.h>
#include <unordered_map>
#include <vector>

#include "GrpPipelineKeyDX12.h"

class CGraphicPipelineCacheDX12
{
	public:
		CGraphicPipelineCacheDX12();
		~CGraphicPipelineCacheDX12();

		bool	Create(ID3D12Device* pkDevice, ID3D12RootSignature* pkRootSignature);
		void	Destroy();

		// Returns the cached PSO for pkKey or compiles one from the supplied
		// bytecode and input layout; NULL on creation failure.
		ID3D12PipelineState*	GetPipelineState(const CGraphicPipelineKeyDX12& rkKey,
												 const D3D12_SHADER_BYTECODE& rkVertexShader,
												 const D3D12_SHADER_BYTECODE& rkPixelShader,
												 const D3D12_INPUT_ELEMENT_DESC* akElements,
												 UINT uElementCount);

		UINT	GetCount() const;

	private:
		struct TEntry
		{
			CGraphicPipelineKeyDX12	kKey;
			ID3D12PipelineState*	pkPipelineState;
		};

		typedef std::unordered_map<UINT64, std::vector<TEntry> > TEntryMap;

		ID3D12Device*			m_pkDevice;
		ID3D12RootSignature*	m_pkRootSignature;
		TEntryMap				m_kEntryMap;
		UINT					m_uCount;
};
