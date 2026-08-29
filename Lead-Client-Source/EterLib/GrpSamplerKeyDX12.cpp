#include "StdAfx.h"
#include "GrpSamplerKeyDX12.h"
#include "StateManager.h"

CGraphicSamplerKeyDX12::CGraphicSamplerKeyDX12()
{
	Reset();
}

void CGraphicSamplerKeyDX12::Reset()
{
	m_uMinFilter = D3DTEXF_POINT;
	m_uMagFilter = D3DTEXF_POINT;
	m_uMipFilter = D3DTEXF_NONE;
	m_uAddressU = D3DTADDRESS_WRAP;
	m_uAddressV = D3DTADDRESS_WRAP;
	m_uMaxAnisotropy = 1;
}

void CGraphicSamplerKeyDX12::Capture(CStateManager& rkStateManager, DWORD dwStage)
{
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_MINFILTER, &m_uMinFilter);
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_MAGFILTER, &m_uMagFilter);
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_MIPFILTER, &m_uMipFilter);
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_ADDRESSU, &m_uAddressU);
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_ADDRESSV, &m_uAddressV);
	rkStateManager.GetSamplerState(dwStage, D3DSAMP_MAXANISOTROPY, &m_uMaxAnisotropy);
}

UINT64 CGraphicSamplerKeyDX12::Hash() const
{
	// Six fields with small value ranges pack into one word directly.
	return static_cast<UINT64>(m_uMinFilter & 0xFF)
		| (static_cast<UINT64>(m_uMagFilter & 0xFF) << 8)
		| (static_cast<UINT64>(m_uMipFilter & 0xFF) << 16)
		| (static_cast<UINT64>(m_uAddressU & 0xFF) << 24)
		| (static_cast<UINT64>(m_uAddressV & 0xFF) << 32)
		| (static_cast<UINT64>(m_uMaxAnisotropy & 0xFF) << 40);
}

bool CGraphicSamplerKeyDX12::operator==(const CGraphicSamplerKeyDX12& rkKey) const
{
	return Hash() == rkKey.Hash();
}

void CGraphicSamplerKeyDX12::ToSamplerDesc(D3D12_SAMPLER_DESC* pkDesc) const
{
	memset(pkDesc, 0, sizeof(*pkDesc));

	if (D3DTEXF_ANISOTROPIC == m_uMinFilter || D3DTEXF_ANISOTROPIC == m_uMagFilter)
	{
		pkDesc->Filter = D3D12_FILTER_ANISOTROPIC;
	}
	else
	{
		// D3D12 basic filters encode as bits: mip 0x1, mag 0x4, min 0x10.
		UINT uFilter = 0;
		if (D3DTEXF_LINEAR == m_uMinFilter)
			uFilter |= 0x10;
		if (D3DTEXF_LINEAR == m_uMagFilter)
			uFilter |= 0x04;
		if (D3DTEXF_LINEAR == m_uMipFilter)
			uFilter |= 0x01;
		pkDesc->Filter = static_cast<D3D12_FILTER>(uFilter);
	}

	pkDesc->AddressU = ToAddressModeDX12(m_uAddressU);
	pkDesc->AddressV = ToAddressModeDX12(m_uAddressV);
	pkDesc->AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pkDesc->MipLODBias = 0.0f;
	pkDesc->MaxAnisotropy = m_uMaxAnisotropy < 1 ? 1 : m_uMaxAnisotropy;
	pkDesc->ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pkDesc->MinLOD = 0.0f;

	// D3DTEXF_NONE disables mipmapping; clamping to the top level matches.
	pkDesc->MaxLOD = (D3DTEXF_NONE == m_uMipFilter) ? 0.0f : D3D12_FLOAT32_MAX;
}

D3D12_TEXTURE_ADDRESS_MODE CGraphicSamplerKeyDX12::ToAddressModeDX12(DWORD dwAddressD3D9)
{
	switch (dwAddressD3D9)
	{
		case D3DTADDRESS_WRAP:			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case D3DTADDRESS_MIRROR:		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case D3DTADDRESS_CLAMP:			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case D3DTADDRESS_BORDER:		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case D3DTADDRESS_MIRRORONCE:	return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	}
	return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}
