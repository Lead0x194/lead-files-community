#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpPipelineCacheDX12.h"

CGraphicPipelineCacheDX12::CGraphicPipelineCacheDX12()
	: m_pkDevice(NULL)
	, m_pkRootSignature(NULL)
	, m_uCount(0)
{
}

CGraphicPipelineCacheDX12::~CGraphicPipelineCacheDX12()
{
	Destroy();
}

bool CGraphicPipelineCacheDX12::Create(ID3D12Device* pkDevice, ID3D12RootSignature* pkRootSignature)
{
	Destroy();

	if (!pkDevice || !pkRootSignature)
		return false;

	m_pkDevice = pkDevice;
	m_pkRootSignature = pkRootSignature;
	return true;
}

void CGraphicPipelineCacheDX12::Destroy()
{
	for (TEntryMap::iterator itBucket = m_kEntryMap.begin(); itBucket != m_kEntryMap.end(); ++itBucket)
	{
		std::vector<TEntry>& rkChain = itBucket->second;
		for (size_t uPos = 0; uPos != rkChain.size(); ++uPos)
			safe_release(rkChain[uPos].pkPipelineState);
	}

	m_kEntryMap.clear();
	m_uCount = 0;
	m_pkDevice = NULL;
	m_pkRootSignature = NULL;
}

ID3D12PipelineState* CGraphicPipelineCacheDX12::GetPipelineState(const CGraphicPipelineKeyDX12& rkKey,
																 const D3D12_SHADER_BYTECODE& rkVertexShader,
																 const D3D12_SHADER_BYTECODE& rkPixelShader,
																 const D3D12_INPUT_ELEMENT_DESC* akElements,
																 UINT uElementCount)
{
	if (!m_pkDevice)
		return NULL;

	std::vector<TEntry>& rkChain = m_kEntryMap[rkKey.Hash()];
	for (size_t uPos = 0; uPos != rkChain.size(); ++uPos)
	{
		if (rkChain[uPos].kKey == rkKey)
			return rkChain[uPos].pkPipelineState;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC kDesc = {};
	kDesc.pRootSignature = m_pkRootSignature;
	kDesc.VS = rkVertexShader;
	kDesc.PS = rkPixelShader;
	rkKey.ToBlendDesc(&kDesc.BlendState);
	kDesc.SampleMask = 0xFFFFFFFF;
	rkKey.ToRasterizerDesc(&kDesc.RasterizerState);
	rkKey.ToDepthStencilDesc(&kDesc.DepthStencilState);
	kDesc.InputLayout.pInputElementDescs = akElements;
	kDesc.InputLayout.NumElements = uElementCount;
	kDesc.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(rkKey.m_uTopologyType);
	kDesc.NumRenderTargets = 1;
	kDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	kDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	kDesc.SampleDesc.Count = 1;

	ID3D12PipelineState* pkPipelineState = NULL;
	if (FAILED(m_pkDevice->CreateGraphicsPipelineState(&kDesc, IID_PPV_ARGS(&pkPipelineState))))
	{
		TraceError("CGraphicPipelineCacheDX12: PSO creation failed (hash %llu).",
				   static_cast<unsigned long long>(rkKey.Hash()));
		return NULL;
	}

	TEntry kEntry;
	kEntry.kKey = rkKey;
	kEntry.pkPipelineState = pkPipelineState;
	rkChain.push_back(kEntry);
	++m_uCount;
	return pkPipelineState;
}

UINT CGraphicPipelineCacheDX12::GetCount() const
{
	return m_uCount;
}
