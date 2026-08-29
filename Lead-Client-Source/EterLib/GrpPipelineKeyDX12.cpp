#include "StdAfx.h"
#include "GrpPipelineKeyDX12.h"
#include "StateManager.h"

CGraphicPipelineKeyDX12::CGraphicPipelineKeyDX12()
{
	Reset();
}

void CGraphicPipelineKeyDX12::Reset()
{
	memset(this, 0, sizeof(*this));
}

void CGraphicPipelineKeyDX12::CaptureRenderStates(CStateManager& rkStateManager)
{
	m_bAlphaBlendEnable = rkStateManager.GetRenderState(D3DRS_ALPHABLENDENABLE) ? 1 : 0;
	m_uSrcBlend = rkStateManager.GetRenderState(D3DRS_SRCBLEND);
	m_uDestBlend = rkStateManager.GetRenderState(D3DRS_DESTBLEND);
	m_uBlendOp = rkStateManager.GetRenderState(D3DRS_BLENDOP);

	m_bAlphaTestEnable = rkStateManager.GetRenderState(D3DRS_ALPHATESTENABLE) ? 1 : 0;
	m_uAlphaFunc = rkStateManager.GetRenderState(D3DRS_ALPHAFUNC);

	m_bZEnable = rkStateManager.GetRenderState(D3DRS_ZENABLE) ? 1 : 0;
	m_bZWriteEnable = rkStateManager.GetRenderState(D3DRS_ZWRITEENABLE) ? 1 : 0;
	m_uZFunc = rkStateManager.GetRenderState(D3DRS_ZFUNC);
	m_uDepthBias = rkStateManager.GetRenderState(D3DRS_DEPTHBIAS);
	m_uSlopeScaleDepthBias = rkStateManager.GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS);

	m_bStencilEnable = rkStateManager.GetRenderState(D3DRS_STENCILENABLE) ? 1 : 0;
	m_uStencilFunc = rkStateManager.GetRenderState(D3DRS_STENCILFUNC);
	m_uStencilFail = rkStateManager.GetRenderState(D3DRS_STENCILFAIL);
	m_uStencilZFail = rkStateManager.GetRenderState(D3DRS_STENCILZFAIL);
	m_uStencilPass = rkStateManager.GetRenderState(D3DRS_STENCILPASS);
	m_uStencilReadMask = rkStateManager.GetRenderState(D3DRS_STENCILMASK);
	m_uStencilWriteMask = rkStateManager.GetRenderState(D3DRS_STENCILWRITEMASK);

	m_uCullMode = rkStateManager.GetRenderState(D3DRS_CULLMODE);
	m_uFillMode = rkStateManager.GetRenderState(D3DRS_FILLMODE);
	m_uColorWriteEnable = rkStateManager.GetRenderState(D3DRS_COLORWRITEENABLE);
}

UINT64 CGraphicPipelineKeyDX12::Hash() const
{
	// FNV-1a; Reset() zeroes explicit padding so every byte is deterministic.
	const BYTE* pbyData = reinterpret_cast<const BYTE*>(this);
	UINT64 uHash = 14695981039346656037ULL;
	for (size_t uPos = 0; uPos != sizeof(*this); ++uPos)
	{
		uHash ^= pbyData[uPos];
		uHash *= 1099511628211ULL;
	}
	return uHash;
}

bool CGraphicPipelineKeyDX12::operator==(const CGraphicPipelineKeyDX12& rkKey) const
{
	return 0 == memcmp(this, &rkKey, sizeof(*this));
}

bool CGraphicPipelineKeyDX12::operator!=(const CGraphicPipelineKeyDX12& rkKey) const
{
	return !(*this == rkKey);
}

D3D12_BLEND CGraphicPipelineKeyDX12::ToBlendDX12(DWORD dwBlendD3D9)
{
	switch (dwBlendD3D9)
	{
		case D3DBLEND_ZERO:				return D3D12_BLEND_ZERO;
		case D3DBLEND_ONE:				return D3D12_BLEND_ONE;
		case D3DBLEND_SRCCOLOR:			return D3D12_BLEND_SRC_COLOR;
		case D3DBLEND_INVSRCCOLOR:		return D3D12_BLEND_INV_SRC_COLOR;
		case D3DBLEND_SRCALPHA:			return D3D12_BLEND_SRC_ALPHA;
		case D3DBLEND_INVSRCALPHA:		return D3D12_BLEND_INV_SRC_ALPHA;
		case D3DBLEND_DESTALPHA:		return D3D12_BLEND_DEST_ALPHA;
		case D3DBLEND_INVDESTALPHA:		return D3D12_BLEND_INV_DEST_ALPHA;
		case D3DBLEND_DESTCOLOR:		return D3D12_BLEND_DEST_COLOR;
		case D3DBLEND_INVDESTCOLOR:		return D3D12_BLEND_INV_DEST_COLOR;
		case D3DBLEND_SRCALPHASAT:		return D3D12_BLEND_SRC_ALPHA_SAT;
		case D3DBLEND_BOTHSRCALPHA:		return D3D12_BLEND_SRC_ALPHA;
		case D3DBLEND_BOTHINVSRCALPHA:	return D3D12_BLEND_INV_SRC_ALPHA;
		case D3DBLEND_BLENDFACTOR:		return D3D12_BLEND_BLEND_FACTOR;
		case D3DBLEND_INVBLENDFACTOR:	return D3D12_BLEND_INV_BLEND_FACTOR;
	}
	return D3D12_BLEND_ONE;
}

D3D12_BLEND_OP CGraphicPipelineKeyDX12::ToBlendOpDX12(DWORD dwBlendOpD3D9)
{
	switch (dwBlendOpD3D9)
	{
		case D3DBLENDOP_ADD:			return D3D12_BLEND_OP_ADD;
		case D3DBLENDOP_SUBTRACT:		return D3D12_BLEND_OP_SUBTRACT;
		case D3DBLENDOP_REVSUBTRACT:	return D3D12_BLEND_OP_REV_SUBTRACT;
		case D3DBLENDOP_MIN:			return D3D12_BLEND_OP_MIN;
		case D3DBLENDOP_MAX:			return D3D12_BLEND_OP_MAX;
	}
	return D3D12_BLEND_OP_ADD;
}

D3D12_COMPARISON_FUNC CGraphicPipelineKeyDX12::ToComparisonFuncDX12(DWORD dwCmpFuncD3D9)
{
	switch (dwCmpFuncD3D9)
	{
		case D3DCMP_NEVER:			return D3D12_COMPARISON_FUNC_NEVER;
		case D3DCMP_LESS:			return D3D12_COMPARISON_FUNC_LESS;
		case D3DCMP_EQUAL:			return D3D12_COMPARISON_FUNC_EQUAL;
		case D3DCMP_LESSEQUAL:		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case D3DCMP_GREATER:		return D3D12_COMPARISON_FUNC_GREATER;
		case D3DCMP_NOTEQUAL:		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case D3DCMP_GREATEREQUAL:	return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case D3DCMP_ALWAYS:			return D3D12_COMPARISON_FUNC_ALWAYS;
	}
	return D3D12_COMPARISON_FUNC_ALWAYS;
}

D3D12_STENCIL_OP CGraphicPipelineKeyDX12::ToStencilOpDX12(DWORD dwStencilOpD3D9)
{
	switch (dwStencilOpD3D9)
	{
		case D3DSTENCILOP_KEEP:		return D3D12_STENCIL_OP_KEEP;
		case D3DSTENCILOP_ZERO:		return D3D12_STENCIL_OP_ZERO;
		case D3DSTENCILOP_REPLACE:	return D3D12_STENCIL_OP_REPLACE;
		case D3DSTENCILOP_INCRSAT:	return D3D12_STENCIL_OP_INCR_SAT;
		case D3DSTENCILOP_DECRSAT:	return D3D12_STENCIL_OP_DECR_SAT;
		case D3DSTENCILOP_INVERT:	return D3D12_STENCIL_OP_INVERT;
		case D3DSTENCILOP_INCR:		return D3D12_STENCIL_OP_INCR;
		case D3DSTENCILOP_DECR:		return D3D12_STENCIL_OP_DECR;
	}
	return D3D12_STENCIL_OP_KEEP;
}

D3D12_CULL_MODE CGraphicPipelineKeyDX12::ToCullModeDX12(DWORD dwCullModeD3D9)
{
	switch (dwCullModeD3D9)
	{
		case D3DCULL_NONE:	return D3D12_CULL_MODE_NONE;
		case D3DCULL_CW:	return D3D12_CULL_MODE_FRONT;
		case D3DCULL_CCW:	return D3D12_CULL_MODE_BACK;
	}
	return D3D12_CULL_MODE_NONE;
}

D3D12_FILL_MODE CGraphicPipelineKeyDX12::ToFillModeDX12(DWORD dwFillModeD3D9)
{
	// D3DFILL_POINT has no DX12 equivalent; wireframe is the closest visual.
	if (D3DFILL_SOLID == dwFillModeD3D9)
		return D3D12_FILL_MODE_SOLID;
	return D3D12_FILL_MODE_WIREFRAME;
}

namespace
{

// D3D12 forbids *_COLOR factors in the alpha channel slots.
D3D12_BLEND __ToAlphaChannelBlend(D3D12_BLEND eBlend)
{
	switch (eBlend)
	{
		case D3D12_BLEND_SRC_COLOR:		return D3D12_BLEND_SRC_ALPHA;
		case D3D12_BLEND_INV_SRC_COLOR:	return D3D12_BLEND_INV_SRC_ALPHA;
		case D3D12_BLEND_DEST_COLOR:	return D3D12_BLEND_DEST_ALPHA;
		case D3D12_BLEND_INV_DEST_COLOR:return D3D12_BLEND_INV_DEST_ALPHA;
		default:						return eBlend;
	}
}

}

void CGraphicPipelineKeyDX12::ToBlendDesc(D3D12_BLEND_DESC* pkDesc) const
{
	memset(pkDesc, 0, sizeof(*pkDesc));
	pkDesc->AlphaToCoverageEnable = FALSE;
	pkDesc->IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC& rkTarget = pkDesc->RenderTarget[0];
	rkTarget.BlendEnable = m_bAlphaBlendEnable ? TRUE : FALSE;
	rkTarget.LogicOpEnable = FALSE;
	rkTarget.SrcBlend = ToBlendDX12(m_uSrcBlend);
	rkTarget.DestBlend = ToBlendDX12(m_uDestBlend);
	rkTarget.BlendOp = ToBlendOpDX12(m_uBlendOp);
	rkTarget.SrcBlendAlpha = __ToAlphaChannelBlend(rkTarget.SrcBlend);
	rkTarget.DestBlendAlpha = __ToAlphaChannelBlend(rkTarget.DestBlend);
	rkTarget.BlendOpAlpha = rkTarget.BlendOp;
	rkTarget.LogicOp = D3D12_LOGIC_OP_NOOP;
	rkTarget.RenderTargetWriteMask = static_cast<UINT8>(m_uColorWriteEnable & 0x0F);
}

void CGraphicPipelineKeyDX12::ToDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC* pkDesc) const
{
	memset(pkDesc, 0, sizeof(*pkDesc));
	pkDesc->DepthEnable = m_bZEnable ? TRUE : FALSE;
	pkDesc->DepthWriteMask = m_bZWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	pkDesc->DepthFunc = ToComparisonFuncDX12(m_uZFunc);
	pkDesc->StencilEnable = m_bStencilEnable ? TRUE : FALSE;
	pkDesc->StencilReadMask = static_cast<UINT8>(m_uStencilReadMask);
	pkDesc->StencilWriteMask = static_cast<UINT8>(m_uStencilWriteMask);
	pkDesc->FrontFace.StencilFailOp = ToStencilOpDX12(m_uStencilFail);
	pkDesc->FrontFace.StencilDepthFailOp = ToStencilOpDX12(m_uStencilZFail);
	pkDesc->FrontFace.StencilPassOp = ToStencilOpDX12(m_uStencilPass);
	pkDesc->FrontFace.StencilFunc = ToComparisonFuncDX12(m_uStencilFunc);

	// The engine never enables two-sided stencil.
	pkDesc->BackFace = pkDesc->FrontFace;
}

void CGraphicPipelineKeyDX12::ToRasterizerDesc(D3D12_RASTERIZER_DESC* pkDesc) const
{
	memset(pkDesc, 0, sizeof(*pkDesc));
	pkDesc->FillMode = ToFillModeDX12(m_uFillMode);
	pkDesc->CullMode = ToCullModeDX12(m_uCullMode);
	pkDesc->FrontCounterClockwise = FALSE;

	// D3D9 depth bias is a float in normalized depth units; scale to the
	// 24-bit integer units the D24 depth buffer uses on DX12.
	float fDepthBias;
	memcpy(&fDepthBias, &m_uDepthBias, sizeof(fDepthBias));
	pkDesc->DepthBias = static_cast<INT>(fDepthBias * 16777216.0f);

	float fSlopeScale;
	memcpy(&fSlopeScale, &m_uSlopeScaleDepthBias, sizeof(fSlopeScale));
	pkDesc->SlopeScaledDepthBias = fSlopeScale;

	pkDesc->DepthBiasClamp = 0.0f;
	pkDesc->DepthClipEnable = TRUE;
	pkDesc->MultisampleEnable = FALSE;
	pkDesc->AntialiasedLineEnable = FALSE;
	pkDesc->ForcedSampleCount = 0;
	pkDesc->ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}
