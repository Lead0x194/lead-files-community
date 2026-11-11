#pragma once

#include <d3d9.h>

// Shader replacements for fixed-function draw paths (DX12 migration).
// Compiled lazily on first bind, released on device destroy.
class CGraphicShaderPool
{
	public:
		CGraphicShaderPool();
		~CGraphicShaderPool();

		// XYZ|DIFFUSE|TEX1 vertices through WORLD*VIEW*PROJECTION,
		// pixel = texture * diffuse (the fixed-function default cascade).
		bool BindPDTModulate();
		// Same vertex path, pixel = diffuse only (fixed-function NULL-texture draws).
		bool BindPDTDiffuse();
		// XYZ|TEX1 vertices, pixel = texture only (fixed-function SELECTARG1(TEXTURE)).
		bool BindPTTexture();
		// XYZ|DIFFUSE|TEX1 vertices, pixel = texture only (diffuse present but unused).
		bool BindPDTTexture();
		// XYZ|DIFFUSE|TEX1 vertices, rgb = texture * diffuse, alpha = texture.
		bool BindPDTModulateTexAlpha();
		// XYZ|DIFFUSE|TEX1 with UVs run through the TEXTURE0 transform,
		// pixel = fixed-function MODULATEINVALPHA_ADDCOLOR (sky cloud layer).
		bool BindPDTTexMatInvAlphaAdd();
		// XYZ|TEX1 with TEXTUREFACTOR (c0) as the effect color; the op mirrors
		// the data-driven COLOROP with ARG1=TFACTOR, ARG2=TEXTURE; alpha = tfactor*texture.
		bool BindPTTFactorModulate();
		bool BindPTTFactorAdd();
		bool BindPTTFactorOnly();
		bool BindPTTexTFactorAlpha();
		// XYZ|NORMAL|TEX1 with fixed-function directional lighting evaluated in the
		// vertex shader; lighting constants (c8-c10) are uploaded by the caller.
		bool BindPNTLit();
		// Lit PNT actor fade: alpha = TFACTOR (BlendRender cascade).
		bool BindPNTLitBlend();
		// Lit PNT hit flash: rgb += TFACTOR (AddRender cascade).
		bool BindPNTLitAdd();
		// Lit PNT plus the sphere-map specular layer: TEXCOORD1 = camera-space
		// reflection vector through the TEXTURE1 transform (granny specular).
		bool BindPNTLitSpecular();
		// PNT with the fixed-function spot + point vertex lighting used by the
		// character-preview screens; light constants (c18-c27) uploaded by the caller.
		bool BindPNTLitOmni();
		// XYZ|NORMAL|TEX1|TEX2 dungeon blocks: tex0 * lightmap(tex1), no lighting.
		bool BindPNT2Lightmap();
		// Lit PNT with the character-shadow projection: TEXCOORD1 = camera-space
		// position through the TEXTURE1 transform (shadow-receiver re-render pass).
		bool BindPNTLitShadowReceiver();
		// Dungeon-block shadow receiver: TFACTOR times the projected shadow map,
		// position through the same WVP math as the PNT2 lightmap pass.
		bool BindPNT2ShadowReceiver();
		void Unbind();

		void Destroy();

	private:
		bool __Create();
		bool __Bind(LPDIRECT3DVERTEXDECLARATION9 lpDeclaration, LPDIRECT3DVERTEXSHADER9 lpVertexShader, LPDIRECT3DPIXELSHADER9 lpPixelShader);

		bool m_bCreateFailed;
		LPDIRECT3DVERTEXSHADER9			m_lpPDTVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPTVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPDTTexMatVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNTLitVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNTLitSpecVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNTLitOmniVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNT2VertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNT2RecvVertexShader;
		LPDIRECT3DVERTEXSHADER9			m_lpPNTLitRecvVertexShader;
		LPDIRECT3DPIXELSHADER9			m_lpModulatePixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpLitBlendPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpLitAddPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpDiffusePixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTexturePixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpModulateTexAlphaPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpInvAlphaAddPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTFactorModulatePixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTFactorAddPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTFactorOnlyPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTexTFactorAlphaPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpLitSpecPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpLightmapPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpLitShadowPixelShader;
		LPDIRECT3DPIXELSHADER9			m_lpTFactorShadowPixelShader;
		LPDIRECT3DVERTEXDECLARATION9	m_lpPDTDeclaration;
		LPDIRECT3DVERTEXDECLARATION9	m_lpPTDeclaration;
		LPDIRECT3DVERTEXDECLARATION9	m_lpPNTDeclaration;
		LPDIRECT3DVERTEXDECLARATION9	m_lpPNT2Declaration;
};
