#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpGammaPassDX12.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace
{

const char* const c_szGammaShader =
	"Texture2D g_kSceneTexture : register(t0);\n"
	"SamplerState g_kSceneSampler : register(s0);\n"
	"cbuffer GammaBlock : register(b0) { float4 g_vGamma; };\n"
	"void VSMain(uint uVertexID : SV_VertexID, out float4 vPosition : SV_Position, out float2 vTexCoord : TEXCOORD0)\n"
	"{\n"
	"	vTexCoord = float2((uVertexID << 1) & 2, uVertexID & 2);\n"
	"	vPosition = float4(vTexCoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);\n"
	"}\n"
	"float4 PSMain(float4 vPosition : SV_Position, float2 vTexCoord : TEXCOORD0) : SV_Target\n"
	"{\n"
	"	float4 kColor = g_kSceneTexture.Sample(g_kSceneSampler, vTexCoord);\n"
	"	return float4(saturate(kColor.rgb * g_vGamma.x), kColor.a);\n"
	"}\n";

ID3DBlob* __CompileShader(const char* c_szEntryPoint, const char* c_szTarget)
{
	ID3DBlob* pkCode = NULL;
	ID3DBlob* pkErrors = NULL;
	if (FAILED(D3DCompile(c_szGammaShader, strlen(c_szGammaShader), NULL, NULL, NULL,
						  c_szEntryPoint, c_szTarget, 0, 0, &pkCode, &pkErrors)))
	{
		TraceError("CGraphicGammaPassDX12: %s compile failed: %s", c_szEntryPoint,
				   pkErrors ? static_cast<const char*>(pkErrors->GetBufferPointer()) : "(no detail)");
		safe_release(pkErrors);
		return NULL;
	}
	safe_release(pkErrors);
	return pkCode;
}

}

CGraphicGammaPassDX12::CGraphicGammaPassDX12()
	: m_pkRootSignature(NULL)
	, m_pkPipelineState(NULL)
{
}

CGraphicGammaPassDX12::~CGraphicGammaPassDX12()
{
	Destroy();
}

bool CGraphicGammaPassDX12::Create(ID3D12Device* pkDevice)
{
	Destroy();

	D3D12_DESCRIPTOR_RANGE kSRVRange = {};
	kSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	kSRVRange.NumDescriptors = 1;
	kSRVRange.BaseShaderRegister = 0;
	kSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER akParams[2] = {};
	akParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	akParams[0].Constants.ShaderRegister = 0;
	akParams[0].Constants.Num32BitValues = 4;
	akParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	akParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	akParams[1].DescriptorTable.NumDescriptorRanges = 1;
	akParams[1].DescriptorTable.pDescriptorRanges = &kSRVRange;
	akParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC kSampler = {};
	kSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	kSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	kSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	kSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	kSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	kSampler.MaxLOD = D3D12_FLOAT32_MAX;
	kSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC kRootDesc = {};
	kRootDesc.NumParameters = 2;
	kRootDesc.pParameters = akParams;
	kRootDesc.NumStaticSamplers = 1;
	kRootDesc.pStaticSamplers = &kSampler;

	ID3DBlob* pkBlob = NULL;
	ID3DBlob* pkErrors = NULL;
	if (FAILED(D3D12SerializeRootSignature(&kRootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pkBlob, &pkErrors)))
	{
		TraceError("CGraphicGammaPassDX12: root signature serialization failed: %s",
				   pkErrors ? static_cast<const char*>(pkErrors->GetBufferPointer()) : "(no detail)");
		safe_release(pkErrors);
		safe_release(pkBlob);
		return false;
	}
	safe_release(pkErrors);

	const HRESULT hrRoot = pkDevice->CreateRootSignature(0, pkBlob->GetBufferPointer(),
														 pkBlob->GetBufferSize(),
														 IID_PPV_ARGS(&m_pkRootSignature));
	safe_release(pkBlob);
	if (FAILED(hrRoot))
	{
		TraceError("CGraphicGammaPassDX12: root signature creation failed.");
		return false;
	}

	ID3DBlob* pkVertexShader = __CompileShader("VSMain", "vs_5_0");
	ID3DBlob* pkPixelShader = __CompileShader("PSMain", "ps_5_0");
	if (!pkVertexShader || !pkPixelShader)
	{
		safe_release(pkVertexShader);
		safe_release(pkPixelShader);
		Destroy();
		return false;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC kDesc = {};
	kDesc.pRootSignature = m_pkRootSignature;
	kDesc.VS.pShaderBytecode = pkVertexShader->GetBufferPointer();
	kDesc.VS.BytecodeLength = pkVertexShader->GetBufferSize();
	kDesc.PS.pShaderBytecode = pkPixelShader->GetBufferPointer();
	kDesc.PS.BytecodeLength = pkPixelShader->GetBufferSize();
	kDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	kDesc.SampleMask = 0xFFFFFFFF;
	kDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	kDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	kDesc.RasterizerState.DepthClipEnable = TRUE;
	kDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	kDesc.NumRenderTargets = 1;
	kDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	kDesc.SampleDesc.Count = 1;

	const HRESULT hrPipeline = pkDevice->CreateGraphicsPipelineState(&kDesc, IID_PPV_ARGS(&m_pkPipelineState));
	safe_release(pkVertexShader);
	safe_release(pkPixelShader);

	if (FAILED(hrPipeline))
	{
		TraceError("CGraphicGammaPassDX12: PSO creation failed.");
		Destroy();
		return false;
	}

	return true;
}

void CGraphicGammaPassDX12::Destroy()
{
	safe_release(m_pkPipelineState);
	safe_release(m_pkRootSignature);
}

bool CGraphicGammaPassDX12::IsCreated() const
{
	return NULL != m_pkPipelineState;
}

void CGraphicGammaPassDX12::Record(ID3D12GraphicsCommandList* pkCommandList,
								   D3D12_GPU_DESCRIPTOR_HANDLE kSceneTable,
								   float fGammaFactor)
{
	if (!m_pkPipelineState)
		return;

	const float afGamma[4] = { fGammaFactor, 0.0f, 0.0f, 0.0f };

	pkCommandList->SetGraphicsRootSignature(m_pkRootSignature);
	pkCommandList->SetPipelineState(m_pkPipelineState);
	pkCommandList->SetGraphicsRoot32BitConstants(0, 4, afGamma, 0);
	pkCommandList->SetGraphicsRootDescriptorTable(1, kSceneTable);
	pkCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pkCommandList->DrawInstanced(3, 1, 0, 0);
}
