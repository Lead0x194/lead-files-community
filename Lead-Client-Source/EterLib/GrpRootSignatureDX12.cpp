#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpRootSignatureDX12.h"

#pragma comment(lib, "d3d12.lib")

CGraphicRootSignatureDX12::CGraphicRootSignatureDX12()
	: m_pkRootSignature(NULL)
{
}

CGraphicRootSignatureDX12::~CGraphicRootSignatureDX12()
{
	Destroy();
}

bool CGraphicRootSignatureDX12::Create(ID3D12Device* pkDevice)
{
	Destroy();

	D3D12_DESCRIPTOR_RANGE kSRVRange = {};
	kSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	kSRVRange.NumDescriptors = SRV_COUNT;
	kSRVRange.BaseShaderRegister = 0;
	kSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE kSamplerRange = {};
	kSamplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	kSamplerRange.NumDescriptors = SAMPLER_COUNT;
	kSamplerRange.BaseShaderRegister = 0;
	kSamplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER akParams[ROOT_PARAM_COUNT] = {};

	akParams[ROOT_PARAM_VS_CONSTANTS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	akParams[ROOT_PARAM_VS_CONSTANTS].Descriptor.ShaderRegister = 0;
	akParams[ROOT_PARAM_VS_CONSTANTS].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	akParams[ROOT_PARAM_PS_CONSTANTS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	akParams[ROOT_PARAM_PS_CONSTANTS].Descriptor.ShaderRegister = 0;
	akParams[ROOT_PARAM_PS_CONSTANTS].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	akParams[ROOT_PARAM_SRV_TABLE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	akParams[ROOT_PARAM_SRV_TABLE].DescriptorTable.NumDescriptorRanges = 1;
	akParams[ROOT_PARAM_SRV_TABLE].DescriptorTable.pDescriptorRanges = &kSRVRange;
	akParams[ROOT_PARAM_SRV_TABLE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	akParams[ROOT_PARAM_SAMPLER_TABLE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	akParams[ROOT_PARAM_SAMPLER_TABLE].DescriptorTable.NumDescriptorRanges = 1;
	akParams[ROOT_PARAM_SAMPLER_TABLE].DescriptorTable.pDescriptorRanges = &kSamplerRange;
	akParams[ROOT_PARAM_SAMPLER_TABLE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC kDesc = {};
	kDesc.NumParameters = ROOT_PARAM_COUNT;
	kDesc.pParameters = akParams;
	kDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pkBlob = NULL;
	ID3DBlob* pkErrors = NULL;
	if (FAILED(D3D12SerializeRootSignature(&kDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pkBlob, &pkErrors)))
	{
		TraceError("CGraphicRootSignatureDX12: serialization failed: %s",
				   pkErrors ? static_cast<const char*>(pkErrors->GetBufferPointer()) : "(no detail)");
		safe_release(pkErrors);
		safe_release(pkBlob);
		return false;
	}
	safe_release(pkErrors);

	const HRESULT hrCreate = pkDevice->CreateRootSignature(0, pkBlob->GetBufferPointer(),
														   pkBlob->GetBufferSize(),
														   IID_PPV_ARGS(&m_pkRootSignature));
	safe_release(pkBlob);

	if (FAILED(hrCreate))
	{
		TraceError("CGraphicRootSignatureDX12: creation failed (0x%08x).",
				   static_cast<unsigned int>(hrCreate));
		return false;
	}

	return true;
}

void CGraphicRootSignatureDX12::Destroy()
{
	safe_release(m_pkRootSignature);
}

ID3D12RootSignature* CGraphicRootSignatureDX12::GetRootSignature() const
{
	return m_pkRootSignature;
}
