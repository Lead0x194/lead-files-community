#pragma once

// D3D9 sampler-state snapshot for one texture stage and its translation to a
// D3D12_SAMPLER_DESC. Hashable so the backend's sampler-descriptor cache
// creates each unique filter/address combination once.

#include <d3d12.h>

class CStateManager;

class CGraphicSamplerKeyDX12
{
	public:
		CGraphicSamplerKeyDX12();

		void	Reset();

		void	Capture(CStateManager& rkStateManager, DWORD dwStage);

		UINT64	Hash() const;

		bool	operator==(const CGraphicSamplerKeyDX12& rkKey) const;

		void	ToSamplerDesc(D3D12_SAMPLER_DESC* pkDesc) const;

		static D3D12_TEXTURE_ADDRESS_MODE	ToAddressModeDX12(DWORD dwAddressD3D9);

	public:
		// Raw D3DTEXTUREFILTERTYPE / D3DTEXTUREADDRESS values; DWORD because
		// CStateManager::GetSamplerState fills them through DWORD pointers.
		DWORD	m_uMinFilter;
		DWORD	m_uMagFilter;
		DWORD	m_uMipFilter;
		DWORD	m_uAddressU;
		DWORD	m_uAddressV;
		DWORD	m_uMaxAnisotropy;
};
