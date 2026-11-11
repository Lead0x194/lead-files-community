#pragma once

// The one root signature the DX12 backend binds everything through, shaped
// after the engine's shader register convention: a vertex constant block
// (c0-c31 as cbuffer b0), a pixel constant block (c0-c1 as cbuffer b0), two
// pixel textures (t0-t1) and their samplers (s0-s1).

#include <d3d12.h>

class CGraphicRootSignatureDX12
{
	public:
		enum ERootParam
		{
			ROOT_PARAM_VS_CONSTANTS,
			ROOT_PARAM_PS_CONSTANTS,
			ROOT_PARAM_SRV_TABLE,
			ROOT_PARAM_SAMPLER_TABLE,

			ROOT_PARAM_COUNT,
		};

		enum
		{
			SRV_COUNT = 2,
			SAMPLER_COUNT = 2,
		};

		CGraphicRootSignatureDX12();
		~CGraphicRootSignatureDX12();

		bool	Create(ID3D12Device* pkDevice);
		void	Destroy();

		ID3D12RootSignature*	GetRootSignature() const;

	private:
		ID3D12RootSignature*	m_pkRootSignature;
};
