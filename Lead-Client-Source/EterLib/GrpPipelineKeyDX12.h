#pragma once

// DX12 pipeline-state key: a packed snapshot of every D3D9 render state that
// folds into a D3D12 graphics PSO, plus the caller-supplied shader/layout
// identity. Pure CPU code - hashing and desc translation need no device.

#include <d3d12.h>

class CStateManager;

class CGraphicPipelineKeyDX12
{
	public:
		CGraphicPipelineKeyDX12();

		void	Reset();

		// Fills the render-state portion from the state manager's cache;
		// shader/declaration/topology identity stays with the caller.
		void	CaptureRenderStates(CStateManager& rkStateManager);

		UINT64	Hash() const;

		bool	operator==(const CGraphicPipelineKeyDX12& rkKey) const;
		bool	operator!=(const CGraphicPipelineKeyDX12& rkKey) const;

		// Translated PSO building blocks (alpha test is absent by design -
		// on DX12 it becomes a shader clip() variant keyed by m_bAlphaTestEnable).
		void	ToBlendDesc(D3D12_BLEND_DESC* pkDesc) const;
		void	ToDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC* pkDesc) const;
		void	ToRasterizerDesc(D3D12_RASTERIZER_DESC* pkDesc) const;

		static D3D12_BLEND				ToBlendDX12(DWORD dwBlendD3D9);
		static D3D12_BLEND_OP			ToBlendOpDX12(DWORD dwBlendOpD3D9);
		static D3D12_COMPARISON_FUNC	ToComparisonFuncDX12(DWORD dwCmpFuncD3D9);
		static D3D12_STENCIL_OP			ToStencilOpDX12(DWORD dwStencilOpD3D9);
		static D3D12_CULL_MODE			ToCullModeDX12(DWORD dwCullModeD3D9);
		static D3D12_FILL_MODE			ToFillModeDX12(DWORD dwFillModeD3D9);

	public:
		// Shader / input-layout identity, set by the caller.
		UINT64	m_uVertexShaderID;
		UINT64	m_uPixelShaderID;
		UINT64	m_uDeclarationID;

		// D3D12_PRIMITIVE_TOPOLOGY_TYPE_* value.
		UINT32	m_uTopologyType;

		// Blend (raw D3DBLEND / D3DBLENDOP values).
		UINT32	m_uSrcBlend;
		UINT32	m_uDestBlend;
		UINT32	m_uBlendOp;

		// Depth (raw D3DCMPFUNC value; bias pair is the raw D3D9 DWORD bits).
		UINT32	m_uZFunc;
		UINT32	m_uDepthBias;
		UINT32	m_uSlopeScaleDepthBias;

		// Stencil (raw D3DCMPFUNC / D3DSTENCILOP values; ref is dynamic on DX12).
		UINT32	m_uStencilFunc;
		UINT32	m_uStencilFail;
		UINT32	m_uStencilZFail;
		UINT32	m_uStencilPass;
		UINT32	m_uStencilReadMask;
		UINT32	m_uStencilWriteMask;

		// Rasterizer / output merger.
		UINT32	m_uCullMode;
		UINT32	m_uFillMode;
		UINT32	m_uColorWriteEnable;

		// Alpha test selects the clip() shader variant (func in low bits).
		UINT32	m_uAlphaFunc;

		UINT8	m_bAlphaBlendEnable;
		UINT8	m_bAlphaTestEnable;
		UINT8	m_bZEnable;
		UINT8	m_bZWriteEnable;
		UINT8	m_bStencilEnable;

		// Explicit tail padding: keeps sizeof a multiple of 8 with no compiler
		// gaps, so whole-struct hashing and memcmp stay deterministic.
		UINT8	m_auPadding[7];
};
