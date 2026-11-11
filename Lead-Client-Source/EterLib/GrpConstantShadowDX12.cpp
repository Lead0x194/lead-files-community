#include "StdAfx.h"
#include "GrpConstantShadowDX12.h"

CGraphicConstantShadowDX12::CGraphicConstantShadowDX12()
{
	Reset();
}

void CGraphicConstantShadowDX12::Reset()
{
	memset(m_afVSConstants, 0, sizeof(m_afVSConstants));
	memset(m_afPSConstants, 0, sizeof(m_afPSConstants));
	m_uVSAddress = 0;
	m_uPSAddress = 0;
	m_bVSDirty = true;
	m_bPSDirty = true;
}

void CGraphicConstantShadowDX12::OnFrameBegin()
{
	// Last frame's ring memory is being reclaimed; force fresh uploads.
	m_bVSDirty = true;
	m_bPSDirty = true;
}

bool CGraphicConstantShadowDX12::SetVSConstants(UINT uStartRegister, const float* afData, UINT uVector4Count)
{
	if (!afData || uStartRegister + uVector4Count > VS_REGISTER_COUNT)
		return false;

	memcpy(m_afVSConstants[uStartRegister], afData, uVector4Count * 4 * sizeof(float));
	m_bVSDirty = true;
	return true;
}

bool CGraphicConstantShadowDX12::SetPSConstants(UINT uStartRegister, const float* afData, UINT uVector4Count)
{
	if (!afData || uStartRegister + uVector4Count > PS_REGISTER_COUNT)
		return false;

	memcpy(m_afPSConstants[uStartRegister], afData, uVector4Count * 4 * sizeof(float));
	m_bPSDirty = true;
	return true;
}

bool CGraphicConstantShadowDX12::FlushVS(CGraphicUploadRingDX12& rkRing, D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut)
{
	if (m_bVSDirty)
	{
		CGraphicUploadRingDX12::TAllocation kAllocation;
		if (!rkRing.Allocate(sizeof(m_afVSConstants), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, &kAllocation))
			return false;

		memcpy(kAllocation.pvCPUAddress, m_afVSConstants, sizeof(m_afVSConstants));
		m_uVSAddress = kAllocation.uGPUAddress;
		m_bVSDirty = false;
	}

	*puAddressOut = m_uVSAddress;
	return 0 != m_uVSAddress;
}

bool CGraphicConstantShadowDX12::FlushPS(CGraphicUploadRingDX12& rkRing, D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut)
{
	if (m_bPSDirty)
	{
		CGraphicUploadRingDX12::TAllocation kAllocation;
		if (!rkRing.Allocate(sizeof(m_afPSConstants), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, &kAllocation))
			return false;

		memcpy(kAllocation.pvCPUAddress, m_afPSConstants, sizeof(m_afPSConstants));
		m_uPSAddress = kAllocation.uGPUAddress;
		m_bPSDirty = false;
	}

	*puAddressOut = m_uPSAddress;
	return 0 != m_uPSAddress;
}
