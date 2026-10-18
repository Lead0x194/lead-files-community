#include "StdAfx.h"
#include <d3dcompiler.h>
#include "../eterBase/Stl.h"
#include "GraphicShaderPool.h"
#include "StateManager.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
	// The fixed-function vertex-fog factor, evaluated exactly like D3D9 FFP:
	// LINEAR ramp or EXP density on the (optionally radial) view-space distance,
	// selected by the flags __Bind derives from the cached fog render states.
	// Reads the transposed WORLDVIEW rows in c11-c13 and the params in c28-c29.
	#define FOG_VS_PARAMS \
		"float4 g_vFogParams : register(c28);\n" \
		"float4 g_vFogParams2 : register(c29);\n"
	#define FOG_VS_DECLARATIONS \
		"float4 g_avWorldView[3] : register(c11);\n" \
		FOG_VS_PARAMS
	#define FOG_VS_BODY \
		"    float3 vViewPos;\n" \
		"    vViewPos.x = dot(vPosition, g_avWorldView[0]);\n" \
		"    vViewPos.y = dot(vPosition, g_avWorldView[1]);\n" \
		"    vViewPos.z = dot(vPosition, g_avWorldView[2]);\n" \
		"    float fFogDist = lerp(vViewPos.z, length(vViewPos), g_vFogParams2.y);\n" \
		"    Out.fFog = saturate(fFogDist * g_vFogParams.x + g_vFogParams.y) * g_vFogParams.z\n" \
		"             + exp2(-fFogDist * g_vFogParams2.x) * g_vFogParams.w;\n"

	// One WVP through dp4, matching the SpeedTree leaf shader convention.
	const char c_achPDTVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		FOG_VS_DECLARATIONS
		"struct VS_INPUT { float3 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    Out.vDiffuse = In.vDiffuse;\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";

	// Fixed-function stage 0: COLOROP/ALPHAOP = MODULATE(TEXTURE, DIFFUSE).
	const char c_achModulatePixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 main(float4 vDiffuse : COLOR0, float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    return tex2D(g_kSampler0, vTexCoord) * vDiffuse;\n"
		"}\n";

	// Fixed-function NULL-texture draw: only the interpolated diffuse reaches the output.
	const char c_achDiffusePixelProgram[] =
		"float4 main(float4 vDiffuse : COLOR0) : COLOR0\n"
		"{\n"
		"    return vDiffuse;\n"
		"}\n";

	// XYZ|TEX1 (no diffuse) through the same WVP.
	const char c_achPTVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		FOG_VS_DECLARATIONS
		"struct VS_INPUT { float3 vPosition : POSITION; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float2 vTexCoord : TEXCOORD0; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";

	// Fixed-function stage 0: COLOROP/ALPHAOP = SELECTARG1(TEXTURE).
	const char c_achTexturePixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    return tex2D(g_kSampler0, vTexCoord);\n"
		"}\n";

	// Fixed-function: COLOROP=MODULATE(TEXTURE,DIFFUSE), ALPHAOP=SELECTARG1(TEXTURE).
	const char c_achModulateTexAlphaPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 main(float4 vDiffuse : COLOR0, float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(kTexel.rgb * vDiffuse.rgb, kTexel.a);\n"
		"}\n";

	// PDT vertices with the TEXTURE0 transform applied to the UVs (COUNT2).
	const char c_achPDTTexMatVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avTexMat[2] : register(c4);\n"
		FOG_VS_DECLARATIONS
		"struct VS_INPUT { float3 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    Out.vDiffuse = In.vDiffuse;\n"
		"    float4 vTexCoord = float4(In.vTexCoord, 1.0f, 0.0f);\n"
		"    Out.vTexCoord.x = dot(vTexCoord, g_avTexMat[0]);\n"
		"    Out.vTexCoord.y = dot(vTexCoord, g_avTexMat[1]);\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";


	// XYZ|NORMAL|TEX1 with the fixed-function directional-light formula:
	// color = saturate(cAmbient + cDiffuse * max(0, dot(worldNormal, lightDir))).
	const char c_achPNTLitVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avWorld[3] : register(c4);\n"
		"float4 g_vLightDirection : register(c8);\n"
		"float4 g_kLitDiffuse : register(c9);\n"
		"float4 g_kLitAmbient : register(c10);\n"
		FOG_VS_DECLARATIONS
		"struct VS_INPUT { float3 vPosition : POSITION; float3 vNormal : NORMAL; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    float4 vNormal = float4(In.vNormal, 0.0f);\n"
		"    float3 vWorldNormal;\n"
		"    vWorldNormal.x = dot(vNormal, g_avWorld[0]);\n"
		"    vWorldNormal.y = dot(vNormal, g_avWorld[1]);\n"
		"    vWorldNormal.z = dot(vNormal, g_avWorld[2]);\n"
		"    float fDot = max(0.0f, dot(vWorldNormal, g_vLightDirection.xyz));\n"
		"    Out.vDiffuse = saturate(g_kLitAmbient + g_kLitDiffuse * fDot);\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";

	// XYZ|NORMAL|TEX1 with the fixed-function spot + point vertex lighting used
	// by the character-preview screens (grp.SetOmniLight): per light,
	// atten = 1/(a0 + a1*d + a2*d*d) inside Range, spot factor is the linear
	// theta/phi cone ramp (Falloff = 1), ambient and diffuse both scaled by them.
	const char c_achPNTLitOmniVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avWorld[3] : register(c4);\n"
		FOG_VS_DECLARATIONS
		"float4 g_vSpotPos : register(c18);\n"       // xyz = position, w = range
		"float4 g_vSpotDir : register(c19);\n"       // xyz = unit direction
		"float4 g_vSpotAtten : register(c20);\n"     // xyz = a0/a1/a2, w = cos(theta/2)
		"float4 g_kSpotDiffuse : register(c21);\n"   // rgb = light.Diffuse*mat.Diffuse, w = cos(phi/2)
		"float4 g_kSpotAmbient : register(c22);\n"   // rgb = light.Ambient*mat.Ambient
		"float4 g_vPointPos : register(c23);\n"      // xyz = position, w = range
		"float4 g_vPointAtten : register(c24);\n"    // xyz = a0/a1/a2, w = enabled (0/1)
		"float4 g_kPointDiffuse : register(c25);\n"
		"float4 g_kPointAmbient : register(c26);\n"
		"float4 g_kBaseColor : register(c27);\n"     // rgb = mat.Ambient*RS_AMBIENT + mat.Emissive, w = mat.Diffuse.a
		"struct VS_INPUT { float3 vPosition : POSITION; float3 vNormal : NORMAL; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    float3 vWorldPos;\n"
		"    vWorldPos.x = dot(vPosition, g_avWorld[0]);\n"
		"    vWorldPos.y = dot(vPosition, g_avWorld[1]);\n"
		"    vWorldPos.z = dot(vPosition, g_avWorld[2]);\n"
		"    float4 vNormal = float4(In.vNormal, 0.0f);\n"
		"    float3 vWorldNormal;\n"
		"    vWorldNormal.x = dot(vNormal, g_avWorld[0]);\n"
		"    vWorldNormal.y = dot(vNormal, g_avWorld[1]);\n"
		"    vWorldNormal.z = dot(vNormal, g_avWorld[2]);\n"
		"    float3 vColor = g_kBaseColor.rgb;\n"
		"    {\n"
		"        float3 vToLight = g_vSpotPos.xyz - vWorldPos;\n"
		"        float fDist = length(vToLight);\n"
		"        float3 vL = vToLight / fDist;\n"
		"        float fAtten = 1.0f / dot(g_vSpotAtten.xyz, float3(1.0f, fDist, fDist * fDist));\n"
		"        fAtten *= (fDist <= g_vSpotPos.w) ? 1.0f : 0.0f;\n"
		"        float fRho = dot(g_vSpotDir.xyz, -vL);\n"
		"        float fSpot = saturate((fRho - g_kSpotDiffuse.w) / (g_vSpotAtten.w - g_kSpotDiffuse.w));\n"
		"        float fDot = max(0.0f, dot(vWorldNormal, vL));\n"
		"        vColor += fAtten * fSpot * (g_kSpotAmbient.rgb + g_kSpotDiffuse.rgb * fDot);\n"
		"    }\n"
		"    {\n"
		"        float3 vToLight = g_vPointPos.xyz - vWorldPos;\n"
		"        float fDist = length(vToLight);\n"
		"        float3 vL = vToLight / fDist;\n"
		"        float fAtten = 1.0f / dot(g_vPointAtten.xyz, float3(1.0f, fDist, fDist * fDist));\n"
		"        fAtten *= (fDist <= g_vPointPos.w) ? g_vPointAtten.w : 0.0f;\n"
		"        float fDot = max(0.0f, dot(vWorldNormal, vL));\n"
		"        vColor += fAtten * (g_kPointAmbient.rgb + g_kPointDiffuse.rgb * fDot);\n"
		"    }\n"
		"    Out.vDiffuse = saturate(float4(vColor, g_kBaseColor.w));\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";


	// Lit PNT with a second texcoord: the fixed-function CAMERASPACEREFLECTIONVECTOR
	// texgen (R = 2(E.N)N - E in camera space) through the TEXTURE1 transform.
	const char c_achPNTLitSpecVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avWorld[3] : register(c4);\n"
		"float4 g_vLightDirection : register(c8);\n"
		"float4 g_kLitDiffuse : register(c9);\n"
		"float4 g_kLitAmbient : register(c10);\n"
		"float4 g_avWorldView[3] : register(c11);\n"
		"float4 g_avTexMat1[2] : register(c15);\n"
		FOG_VS_PARAMS
		"struct VS_INPUT { float3 vPosition : POSITION; float3 vNormal : NORMAL; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float2 vSpecCoord : TEXCOORD1; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    float4 vNormal = float4(In.vNormal, 0.0f);\n"
		"    float3 vWorldNormal;\n"
		"    vWorldNormal.x = dot(vNormal, g_avWorld[0]);\n"
		"    vWorldNormal.y = dot(vNormal, g_avWorld[1]);\n"
		"    vWorldNormal.z = dot(vNormal, g_avWorld[2]);\n"
		"    float fDot = max(0.0f, dot(vWorldNormal, g_vLightDirection.xyz));\n"
		"    Out.vDiffuse = saturate(g_kLitAmbient + g_kLitDiffuse * fDot);\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		"    float3 vCamPos;\n"
		"    vCamPos.x = dot(vPosition, g_avWorldView[0]);\n"
		"    vCamPos.y = dot(vPosition, g_avWorldView[1]);\n"
		"    vCamPos.z = dot(vPosition, g_avWorldView[2]);\n"
		"    float3 vCamNormal;\n"
		"    vCamNormal.x = dot(vNormal, g_avWorldView[0]);\n"
		"    vCamNormal.y = dot(vNormal, g_avWorldView[1]);\n"
		"    vCamNormal.z = dot(vNormal, g_avWorldView[2]);\n"
		"    float3 vEye = normalize(vCamPos);\n"
		"    float3 vUnitNormal = normalize(vCamNormal);\n"
		"    float4 vReflect = float4(2.0f * dot(vEye, vUnitNormal) * vUnitNormal - vEye, 1.0f);\n"
		"    Out.vSpecCoord.x = dot(vReflect, g_avTexMat1[0]);\n"
		"    Out.vSpecCoord.y = dot(vReflect, g_avTexMat1[1]);\n"
		"    float fFogDist = lerp(vCamPos.z, length(vCamPos), g_vFogParams2.y);\n"
		"    Out.fFog = saturate(fFogDist * g_vFogParams.x + g_vFogParams.y) * g_vFogParams.z\n"
		"             + exp2(-fFogDist * g_vFogParams2.x) * g_vFogParams.w;\n"
		"    return Out;\n"
		"}\n";

	// Granny specular: stage0 MODULATE(tex, lit) with alpha = tex.a * TFACTOR.a,
	// stage1 MODULATEALPHA_ADDCOLOR(current, envmap), alpha = current.
	const char c_achLitSpecPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"sampler2D g_kSampler1 : register(s1);\n"
		"float4 g_kTFactor : register(c0);\n"
		"float4 main(float4 vDiffuse : COLOR0, float2 vTexCoord : TEXCOORD0, float2 vSpecCoord : TEXCOORD1) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    float3 vColor = kTexel.rgb * vDiffuse.rgb;\n"
		"    float fAlpha = kTexel.a * g_kTFactor.a;\n"
		"    float3 vEnv = tex2D(g_kSampler1, vSpecCoord).rgb;\n"
		"    return float4(vColor + fAlpha * vEnv, fAlpha);\n"
		"}\n";


	// XYZ|NORMAL|TEX1|TEX2 through WVP, both UV sets passed through (dungeon).
	const char c_achPNT2VertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		FOG_VS_DECLARATIONS
		"struct VS_INPUT { float3 vPosition : POSITION; float2 vTexCoord : TEXCOORD0; float2 vLightCoord : TEXCOORD1; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float2 vTexCoord : TEXCOORD0; float2 vLightCoord : TEXCOORD1; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		"    Out.vLightCoord = In.vLightCoord;\n"
		FOG_VS_BODY
		"    return Out;\n"
		"}\n";

	// Dungeon cascade: stage0 SELECTARG1(TEXTURE), stage1 MODULATE(TEXTURE, CURRENT).
	const char c_achLightmapPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"sampler2D g_kSampler1 : register(s1);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0, float2 vLightCoord : TEXCOORD1) : COLOR0\n"
		"{\n"
		"    return tex2D(g_kSampler0, vTexCoord) * tex2D(g_kSampler1, vLightCoord);\n"
		"}\n";


	// Lit PNT with the fixed-function CAMERASPACEPOSITION texgen on TEXCOORD1
	// (character-shadow projection through the TEXTURE1 transform).
	const char c_achPNTLitRecvVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avWorld[3] : register(c4);\n"
		"float4 g_vLightDirection : register(c8);\n"
		"float4 g_kLitDiffuse : register(c9);\n"
		"float4 g_kLitAmbient : register(c10);\n"
		"float4 g_avWorldView[3] : register(c11);\n"
		"float4 g_avTexMat1[2] : register(c15);\n"
		FOG_VS_PARAMS
		"struct VS_INPUT { float3 vPosition : POSITION; float3 vNormal : NORMAL; float2 vTexCoord : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float4 vDiffuse : COLOR0; float2 vTexCoord : TEXCOORD0; float2 vShadowCoord : TEXCOORD1; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    float4 vNormal = float4(In.vNormal, 0.0f);\n"
		"    float3 vWorldNormal;\n"
		"    vWorldNormal.x = dot(vNormal, g_avWorld[0]);\n"
		"    vWorldNormal.y = dot(vNormal, g_avWorld[1]);\n"
		"    vWorldNormal.z = dot(vNormal, g_avWorld[2]);\n"
		"    float fDot = max(0.0f, dot(vWorldNormal, g_vLightDirection.xyz));\n"
		"    Out.vDiffuse = saturate(g_kLitAmbient + g_kLitDiffuse * fDot);\n"
		"    Out.vTexCoord = In.vTexCoord;\n"
		"    float4 vCamPos;\n"
		"    vCamPos.x = dot(vPosition, g_avWorldView[0]);\n"
		"    vCamPos.y = dot(vPosition, g_avWorldView[1]);\n"
		"    vCamPos.z = dot(vPosition, g_avWorldView[2]);\n"
		"    vCamPos.w = 1.0f;\n"
		"    Out.vShadowCoord.x = dot(vCamPos, g_avTexMat1[0]);\n"
		"    Out.vShadowCoord.y = dot(vCamPos, g_avTexMat1[1]);\n"
		"    float fFogDist = lerp(vCamPos.z, length(vCamPos.xyz), g_vFogParams2.y);\n"
		"    Out.fFog = saturate(fFogDist * g_vFogParams.x + g_vFogParams.y) * g_vFogParams.z\n"
		"             + exp2(-fFogDist * g_vFogParams2.x) * g_vFogParams.w;\n"
		"    return Out;\n"
		"}\n";

	// Shadow receiver: stage0 MODULATE(tex, lit), stage1 MODULATE(shadow, current).
	const char c_achLitShadowPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"sampler2D g_kSampler1 : register(s1);\n"
		"float4 main(float4 vDiffuse : COLOR0, float2 vTexCoord : TEXCOORD0, float2 vShadowCoord : TEXCOORD1) : COLOR0\n"
		"{\n"
		"    float3 vColor = tex2D(g_kSampler0, vTexCoord).rgb * vDiffuse.rgb;\n"
		"    vColor *= tex2D(g_kSampler1, vShadowCoord).rgb;\n"
		"    return float4(vColor, 1.0f);\n"
		"}\n";

	// XYZ|NORMAL|TEX1|TEX2 with the character-shadow projection on TEXCOORD1.
	// Position goes through the same WVP dot sequence as PNT2VertexProgram so
	// both dungeon-block passes rasterize bit-identical depths.
	const char c_achPNT2RecvVertexProgram[] =
		"float4 g_avWVP[4] : register(c0);\n"
		"float4 g_avWorldView[3] : register(c11);\n"
		"float4 g_avTexMat1[2] : register(c15);\n"
		FOG_VS_PARAMS
		"struct VS_INPUT { float3 vPosition : POSITION; };\n"
		"struct VS_OUTPUT { float4 vPosition : POSITION; float2 vShadowCoord : TEXCOORD1; float fFog : FOG; };\n"
		"VS_OUTPUT main(VS_INPUT In)\n"
		"{\n"
		"    VS_OUTPUT Out;\n"
		"    float4 vPosition = float4(In.vPosition, 1.0f);\n"
		"    Out.vPosition.x = dot(vPosition, g_avWVP[0]);\n"
		"    Out.vPosition.y = dot(vPosition, g_avWVP[1]);\n"
		"    Out.vPosition.z = dot(vPosition, g_avWVP[2]);\n"
		"    Out.vPosition.w = dot(vPosition, g_avWVP[3]);\n"
		"    float4 vCamPos;\n"
		"    vCamPos.x = dot(vPosition, g_avWorldView[0]);\n"
		"    vCamPos.y = dot(vPosition, g_avWorldView[1]);\n"
		"    vCamPos.z = dot(vPosition, g_avWorldView[2]);\n"
		"    vCamPos.w = 1.0f;\n"
		"    Out.vShadowCoord.x = dot(vCamPos, g_avTexMat1[0]);\n"
		"    Out.vShadowCoord.y = dot(vCamPos, g_avTexMat1[1]);\n"
		"    float fFogDist = lerp(vCamPos.z, length(vCamPos.xyz), g_vFogParams2.y);\n"
		"    Out.fFog = saturate(fFogDist * g_vFogParams.x + g_vFogParams.y) * g_vFogParams.z\n"
		"             + exp2(-fFogDist * g_vFogParams2.x) * g_vFogParams.w;\n"
		"    return Out;\n"
		"}\n";

	// Dungeon-block shadow receiver: stage0 SELECTARG1(TFACTOR), stage1
	// MODULATE(shadow, current); the ZERO/SRCCOLOR blend applies it to the scene.
	const char c_achTFactorShadowPixelProgram[] =
		"sampler2D g_kSampler1 : register(s1);\n"
		"float4 g_vTFactor : register(c0);\n"
		"float4 main(float2 vShadowCoord : TEXCOORD1) : COLOR0\n"
		"{\n"
		"    return float4(g_vTFactor.rgb * tex2D(g_kSampler1, vShadowCoord).rgb, 1.0f);\n"
		"}\n";

	// Fixed-function D3DTOP_MODULATEINVALPHA_ADDCOLOR(TEXTURE, DIFFUSE), alpha = texture.
	const char c_achInvAlphaAddPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 main(float4 vDiffuse : COLOR0, float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(kTexel.rgb + vDiffuse.rgb * (1.0f - kTexel.a), kTexel.a);\n"
		"}\n";

	// Effect combiners: COLOROP(ARG1=TFACTOR, ARG2=TEXTURE), ALPHAOP = MODULATE.
	const char c_achTFactorModulatePixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 g_kTFactor : register(c0);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(kTexel.rgb * g_kTFactor.rgb, kTexel.a * g_kTFactor.a);\n"
		"}\n";
	const char c_achTFactorAddPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 g_kTFactor : register(c0);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(kTexel.rgb + g_kTFactor.rgb, kTexel.a * g_kTFactor.a);\n"
		"}\n";
	const char c_achTFactorOnlyPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 g_kTFactor : register(c0);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(g_kTFactor.rgb, kTexel.a * g_kTFactor.a);\n"
		"}\n";
	const char c_achTexTFactorAlphaPixelProgram[] =
		"sampler2D g_kSampler0 : register(s0);\n"
		"float4 g_kTFactor : register(c0);\n"
		"float4 main(float2 vTexCoord : TEXCOORD0) : COLOR0\n"
		"{\n"
		"    float4 kTexel = tex2D(g_kSampler0, vTexCoord);\n"
		"    return float4(kTexel.rgb, kTexel.a * g_kTFactor.a);\n"
		"}\n";
}

CGraphicShaderPool::CGraphicShaderPool()
	: m_bCreateFailed(false)
	, m_lpPDTVertexShader(NULL)
	, m_lpPTVertexShader(NULL)
	, m_lpPDTTexMatVertexShader(NULL)
	, m_lpPNTLitVertexShader(NULL)
	, m_lpPNTLitSpecVertexShader(NULL)
	, m_lpPNTLitOmniVertexShader(NULL)
	, m_lpPNT2VertexShader(NULL)
	, m_lpPNT2RecvVertexShader(NULL)
	, m_lpPNTLitRecvVertexShader(NULL)
	, m_lpModulatePixelShader(NULL)
	, m_lpDiffusePixelShader(NULL)
	, m_lpTexturePixelShader(NULL)
	, m_lpModulateTexAlphaPixelShader(NULL)
	, m_lpInvAlphaAddPixelShader(NULL)
	, m_lpTFactorModulatePixelShader(NULL)
	, m_lpTFactorAddPixelShader(NULL)
	, m_lpTFactorOnlyPixelShader(NULL)
	, m_lpTexTFactorAlphaPixelShader(NULL)
	, m_lpLitSpecPixelShader(NULL)
	, m_lpLightmapPixelShader(NULL)
	, m_lpLitShadowPixelShader(NULL)
	, m_lpTFactorShadowPixelShader(NULL)
	, m_lpPDTDeclaration(NULL)
	, m_lpPTDeclaration(NULL)
	, m_lpPNTDeclaration(NULL)
	, m_lpPNT2Declaration(NULL)
{
}

CGraphicShaderPool::~CGraphicShaderPool()
{
	Destroy();
}

void CGraphicShaderPool::Destroy()
{
	safe_release(m_lpPDTVertexShader);
	safe_release(m_lpPTVertexShader);
	safe_release(m_lpPDTTexMatVertexShader);
	safe_release(m_lpPNTLitVertexShader);
	safe_release(m_lpPNTLitSpecVertexShader);
	safe_release(m_lpPNTLitOmniVertexShader);
	safe_release(m_lpPNT2VertexShader);
	safe_release(m_lpPNT2RecvVertexShader);
	safe_release(m_lpPNTLitRecvVertexShader);
	safe_release(m_lpModulatePixelShader);
	safe_release(m_lpDiffusePixelShader);
	safe_release(m_lpTexturePixelShader);
	safe_release(m_lpModulateTexAlphaPixelShader);
	safe_release(m_lpInvAlphaAddPixelShader);
	safe_release(m_lpTFactorModulatePixelShader);
	safe_release(m_lpTFactorAddPixelShader);
	safe_release(m_lpTFactorOnlyPixelShader);
	safe_release(m_lpTexTFactorAlphaPixelShader);
	safe_release(m_lpLitSpecPixelShader);
	safe_release(m_lpLightmapPixelShader);
	safe_release(m_lpLitShadowPixelShader);
	safe_release(m_lpTFactorShadowPixelShader);
	safe_release(m_lpPDTDeclaration);
	safe_release(m_lpPTDeclaration);
	safe_release(m_lpPNTDeclaration);
	safe_release(m_lpPNT2Declaration);
	m_bCreateFailed = false;
}

bool CGraphicShaderPool::__Create()
{
	if (m_bCreateFailed)
		return false;

	const D3DVERTEXELEMENT9 akPDTElements[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	ID3DBlob* pCode = NULL;
	ID3DBlob* pError = NULL;

	if (FAILED(D3DCompile(c_achPDTVertexProgram, sizeof(c_achPDTVertexProgram) - 1, "PDTVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPDTVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PDT vertex shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achModulatePixelProgram, sizeof(c_achModulatePixelProgram) - 1, "ModulatePixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpModulatePixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build modulate pixel shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achDiffusePixelProgram, sizeof(c_achDiffusePixelProgram) - 1, "DiffusePixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpDiffusePixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build diffuse pixel shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(STATEMANAGER.CreateVertexDeclaration(akPDTElements, &m_lpPDTDeclaration)))
	{
		TraceError("CGraphicShaderPool: failed to create PDT vertex declaration.");
		Destroy();
		m_bCreateFailed = true;
		return false;
	}


	const D3DVERTEXELEMENT9 akPTElements[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(D3DCompile(c_achPTVertexProgram, sizeof(c_achPTVertexProgram) - 1, "PTVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPTVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PT vertex shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTexturePixelProgram, sizeof(c_achTexturePixelProgram) - 1, "TexturePixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTexturePixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build texture pixel shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achModulateTexAlphaPixelProgram, sizeof(c_achModulateTexAlphaPixelProgram) - 1, "ModulateTexAlphaPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpModulateTexAlphaPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build modulate-tex-alpha pixel shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPDTTexMatVertexProgram, sizeof(c_achPDTTexMatVertexProgram) - 1, "PDTTexMatVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPDTTexMatVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PDTTexMatVertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achInvAlphaAddPixelProgram, sizeof(c_achInvAlphaAddPixelProgram) - 1, "InvAlphaAddPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpInvAlphaAddPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build InvAlphaAddPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTFactorModulatePixelProgram, sizeof(c_achTFactorModulatePixelProgram) - 1, "TFactorModulatePixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTFactorModulatePixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build TFactorModulatePixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTFactorAddPixelProgram, sizeof(c_achTFactorAddPixelProgram) - 1, "TFactorAddPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTFactorAddPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build TFactorAddPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTFactorOnlyPixelProgram, sizeof(c_achTFactorOnlyPixelProgram) - 1, "TFactorOnlyPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTFactorOnlyPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build TFactorOnlyPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTexTFactorAlphaPixelProgram, sizeof(c_achTexTFactorAlphaPixelProgram) - 1, "TexTFactorAlphaPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTexTFactorAlphaPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build TexTFactorAlphaPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(STATEMANAGER.CreateVertexDeclaration(akPTElements, &m_lpPTDeclaration)))
	{
		TraceError("CGraphicShaderPool: failed to create PT vertex declaration.");
		Destroy();
		m_bCreateFailed = true;
		return false;
	}


	const D3DVERTEXELEMENT9 akPNTElements[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(D3DCompile(c_achPNTLitVertexProgram, sizeof(c_achPNTLitVertexProgram) - 1, "PNTLitVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNTLitVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNT lit vertex shader [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPNTLitSpecVertexProgram, sizeof(c_achPNTLitSpecVertexProgram) - 1, "PNTLitSpecVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNTLitSpecVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNTLitSpecVertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPNTLitOmniVertexProgram, sizeof(c_achPNTLitOmniVertexProgram) - 1, "PNTLitOmniVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNTLitOmniVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNTLitOmniVertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achLitSpecPixelProgram, sizeof(c_achLitSpecPixelProgram) - 1, "LitSpecPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpLitSpecPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build LitSpecPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPNT2VertexProgram, sizeof(c_achPNT2VertexProgram) - 1, "PNT2VertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNT2VertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNT2VertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achLightmapPixelProgram, sizeof(c_achLightmapPixelProgram) - 1, "LightmapPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpLightmapPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build LightmapPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPNTLitRecvVertexProgram, sizeof(c_achPNTLitRecvVertexProgram) - 1, "PNTLitRecvVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNTLitRecvVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNTLitRecvVertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achLitShadowPixelProgram, sizeof(c_achLitShadowPixelProgram) - 1, "LitShadowPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpLitShadowPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build LitShadowPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achPNT2RecvVertexProgram, sizeof(c_achPNT2RecvVertexProgram) - 1, "PNT2RecvVertexProgram",
						  NULL, NULL, "main", "vs_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreateVertexShader((const DWORD*)pCode->GetBufferPointer(), &m_lpPNT2RecvVertexShader)))
	{
		TraceError("CGraphicShaderPool: failed to build PNT2RecvVertexProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	if (FAILED(D3DCompile(c_achTFactorShadowPixelProgram, sizeof(c_achTFactorShadowPixelProgram) - 1, "TFactorShadowPixelProgram",
						  NULL, NULL, "main", "ps_2_0", 0, 0, &pCode, &pError)) ||
		FAILED(STATEMANAGER.CreatePixelShader((const DWORD*)pCode->GetBufferPointer(), &m_lpTFactorShadowPixelShader)))
	{
		TraceError("CGraphicShaderPool: failed to build TFactorShadowPixelProgram [ %s ].",
				   pError ? (const char*)pError->GetBufferPointer() : "unknown");
		safe_release(pCode);
		safe_release(pError);
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	safe_release(pCode);
	safe_release(pError);

	const D3DVERTEXELEMENT9 akPNT2Elements[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};

	if (FAILED(STATEMANAGER.CreateVertexDeclaration(akPNT2Elements, &m_lpPNT2Declaration)))
	{
		TraceError("CGraphicShaderPool: failed to create PNT2 vertex declaration.");
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	if (FAILED(STATEMANAGER.CreateVertexDeclaration(akPNTElements, &m_lpPNTDeclaration)))
	{
		TraceError("CGraphicShaderPool: failed to create PNT vertex declaration.");
		Destroy();
		m_bCreateFailed = true;
		return false;
	}

	return true;
}

bool CGraphicShaderPool::__Bind(LPDIRECT3DVERTEXDECLARATION9 lpDeclaration, LPDIRECT3DVERTEXSHADER9 lpVertexShader, LPDIRECT3DPIXELSHADER9 lpPixelShader)
{
	if (!m_lpPDTDeclaration && !__Create())
		return false;

	// The caller reads its member arguments BEFORE the create above runs, so on
	// the very first bind they are still the pre-creation NULLs; binding those
	// would draw with no declaration/shader at all. Fall back to fixed-function
	// for this draw - the next bind picks up the freshly created objects.
	if (!lpDeclaration || !lpVertexShader || !lpPixelShader)
		return false;

	D3DXMATRIX matWorld, matView, matProj, matWorldView, matWVP;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);
	STATEMANAGER.GetTransform(D3DTS_VIEW, &matView);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWVP, &matWorldView, &matProj);
	D3DXMatrixTranspose(&matWVP, &matWVP);
	STATEMANAGER.SetVertexShaderConstant(0, &matWVP, 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	STATEMANAGER.SetVertexShaderConstant(11, &matWorldView, 3);

	// Mirror the fixed-function vertex-fog states into c28/c29: a LINEAR ramp
	// (d * x + y, also covering the fog-off case as a constant 1) or an EXP
	// curve, each gated by its select flag, over the plain or radial distance.
	D3DXVECTOR4 avFogParams[2];
	avFogParams[0] = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.0f);
	avFogParams[1] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	if (STATEMANAGER.GetRenderState(D3DRS_FOGENABLE))
	{
		const DWORD dwFogMode = STATEMANAGER.GetRenderState(D3DRS_FOGVERTEXMODE);
		if (D3DFOG_LINEAR == dwFogMode)
		{
			const DWORD dwStart = STATEMANAGER.GetRenderState(D3DRS_FOGSTART);
			const DWORD dwEnd = STATEMANAGER.GetRenderState(D3DRS_FOGEND);
			const float fStart = *reinterpret_cast<const float*>(&dwStart);
			const float fEnd = *reinterpret_cast<const float*>(&dwEnd);
			// A degenerate range degrades to a hard cutoff at the end distance.
			const float fRange = (fEnd - fStart > 0.0001f) ? (fEnd - fStart) : 0.0001f;
			avFogParams[0].x = -1.0f / fRange;
			avFogParams[0].y = fEnd / fRange;
		}
		else if (D3DFOG_EXP == dwFogMode)
		{
			const DWORD dwDensity = STATEMANAGER.GetRenderState(D3DRS_FOGDENSITY);
			avFogParams[0].z = 0.0f;
			avFogParams[0].w = 1.0f;
			// exp(-d * density) evaluated as exp2: fold in log2(e).
			avFogParams[1].x = *reinterpret_cast<const float*>(&dwDensity) * 1.442695f;
		}
		avFogParams[1].y = STATEMANAGER.GetRenderState(D3DRS_RANGEFOGENABLE) ? 1.0f : 0.0f;
	}
	STATEMANAGER.SetVertexShaderConstant(28, avFogParams, 2);

	STATEMANAGER.SetVertexDeclaration(lpDeclaration);
	STATEMANAGER.SetVertexShader(lpVertexShader);
	STATEMANAGER.SetPixelShader(lpPixelShader);
	return true;
}

bool CGraphicShaderPool::BindPDTModulate()
{
	return __Bind(m_lpPDTDeclaration, m_lpPDTVertexShader, m_lpModulatePixelShader);
}

bool CGraphicShaderPool::BindPDTDiffuse()
{
	return __Bind(m_lpPDTDeclaration, m_lpPDTVertexShader, m_lpDiffusePixelShader);
}

bool CGraphicShaderPool::BindPTTexture()
{
	return __Bind(m_lpPTDeclaration, m_lpPTVertexShader, m_lpTexturePixelShader);
}

bool CGraphicShaderPool::BindPDTTexture()
{
	return __Bind(m_lpPDTDeclaration, m_lpPDTVertexShader, m_lpTexturePixelShader);
}

bool CGraphicShaderPool::BindPDTModulateTexAlpha()
{
	return __Bind(m_lpPDTDeclaration, m_lpPDTVertexShader, m_lpModulateTexAlphaPixelShader);
}

bool CGraphicShaderPool::BindPDTTexMatInvAlphaAdd()
{
	if (!__Bind(m_lpPDTDeclaration, m_lpPDTTexMatVertexShader, m_lpInvAlphaAddPixelShader))
		return false;

	D3DXMATRIX matTexture;
	STATEMANAGER.GetTransform(D3DTS_TEXTURE0, &matTexture);
	D3DXMatrixTranspose(&matTexture, &matTexture);
	STATEMANAGER.SetVertexShaderConstant(4, &matTexture, 2);
	return true;
}

bool CGraphicShaderPool::BindPTTFactorModulate()
{
	return __Bind(m_lpPTDeclaration, m_lpPTVertexShader, m_lpTFactorModulatePixelShader);
}

bool CGraphicShaderPool::BindPTTFactorAdd()
{
	return __Bind(m_lpPTDeclaration, m_lpPTVertexShader, m_lpTFactorAddPixelShader);
}

bool CGraphicShaderPool::BindPTTFactorOnly()
{
	return __Bind(m_lpPTDeclaration, m_lpPTVertexShader, m_lpTFactorOnlyPixelShader);
}

bool CGraphicShaderPool::BindPTTexTFactorAlpha()
{
	return __Bind(m_lpPTDeclaration, m_lpPTVertexShader, m_lpTexTFactorAlphaPixelShader);
}

bool CGraphicShaderPool::BindPNTLit()
{
	if (!__Bind(m_lpPNTDeclaration, m_lpPNTLitVertexShader, m_lpModulatePixelShader))
		return false;

	D3DXMATRIX matWorld;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranspose(&matWorld, &matWorld);
	STATEMANAGER.SetVertexShaderConstant(4, &matWorld, 3);
	return true;
}

bool CGraphicShaderPool::BindPNTLitSpecular()
{
	if (!__Bind(m_lpPNTDeclaration, m_lpPNTLitSpecVertexShader, m_lpLitSpecPixelShader))
		return false;

	D3DXMATRIX matWorld, matTexture;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranspose(&matWorld, &matWorld);
	STATEMANAGER.SetVertexShaderConstant(4, &matWorld, 3);
	STATEMANAGER.GetTransform(D3DTS_TEXTURE1, &matTexture);
	D3DXMatrixTranspose(&matTexture, &matTexture);
	STATEMANAGER.SetVertexShaderConstant(15, &matTexture, 2);
	return true;
}

bool CGraphicShaderPool::BindPNTLitOmni()
{
	if (!__Bind(m_lpPNTDeclaration, m_lpPNTLitOmniVertexShader, m_lpModulatePixelShader))
		return false;

	D3DXMATRIX matWorld;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranspose(&matWorld, &matWorld);
	STATEMANAGER.SetVertexShaderConstant(4, &matWorld, 3);
	return true;
}

bool CGraphicShaderPool::BindPNT2Lightmap()
{
	return __Bind(m_lpPNT2Declaration, m_lpPNT2VertexShader, m_lpLightmapPixelShader);
}

bool CGraphicShaderPool::BindPNTLitShadowReceiver()
{
	if (!__Bind(m_lpPNTDeclaration, m_lpPNTLitRecvVertexShader, m_lpLitShadowPixelShader))
		return false;

	D3DXMATRIX matWorld, matTexture;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranspose(&matWorld, &matWorld);
	STATEMANAGER.SetVertexShaderConstant(4, &matWorld, 3);
	STATEMANAGER.GetTransform(D3DTS_TEXTURE1, &matTexture);
	D3DXMatrixTranspose(&matTexture, &matTexture);
	STATEMANAGER.SetVertexShaderConstant(15, &matTexture, 2);
	return true;
}

bool CGraphicShaderPool::BindPNT2ShadowReceiver()
{
	if (!__Bind(m_lpPNT2Declaration, m_lpPNT2RecvVertexShader, m_lpTFactorShadowPixelShader))
		return false;

	D3DXMATRIX matTexture;
	STATEMANAGER.GetTransform(D3DTS_TEXTURE1, &matTexture);
	D3DXMatrixTranspose(&matTexture, &matTexture);
	STATEMANAGER.SetVertexShaderConstant(15, &matTexture, 2);
	return true;
}


void CGraphicShaderPool::Unbind()
{
	STATEMANAGER.SetVertexShader(NULL);
	STATEMANAGER.SetPixelShader(NULL);
}
