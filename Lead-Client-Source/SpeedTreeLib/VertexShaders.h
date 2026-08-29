///////////////////////////////////////////////////////////////////////  
//	SpeedTreeRT DirectX Example
//
//	(c) 2003 IDV, Inc.
//
//	This example demonstrates how to render trees using SpeedTreeRT
//	and DirectX.  Techniques illustrated include ".spt" file parsing,
//	static lighting, dynamic lighting, LOD implementation, cloning,
//	instancing, and dynamic wind effects.
//
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com

///////////////////////////////////////////////////////////////////////  
//	Includes

#pragma once
#include "SpeedTreeConfig.h"
#include "../eterlib/StateManager.h"
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////  
//	Branch & Frond Vertex Formats

static DWORD D3DFVF_SPEEDTREE_BRANCH_VERTEX =
		D3DFVF_XYZ |							// always have the position
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			// precomputed colors or geometric normals
		D3DFVF_NORMAL |
	#else
		D3DFVF_DIFFUSE |
	#endif
	#ifdef WRAPPER_RENDER_SELF_SHADOWS
		D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) // shadow texture coordinates
	#else
		D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)	// always have first texture layer coords
	#endif
	#ifdef WRAPPER_USE_GPU_WIND					
		| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(2)	// GPU Only - wind weight and index passed in second texture layer
	#endif
		;

/////////////////////////////////////////////////////////////////////// 
// FVF Branch Vertex Structure

struct SFVFBranchVertex
{
	D3DXVECTOR3		m_vPosition;			// Always Used							
#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			
	D3DXVECTOR3		m_vNormal;				// Dynamic Lighting Only			
#else										     
	DWORD			m_dwDiffuseColor;		// Static Lighting Only	
#endif	
	FLOAT			m_fTexCoords[2];		// Always Used
#ifdef WRAPPER_RENDER_SELF_SHADOWS
	FLOAT			m_fShadowCoords[2];		// Texture coordinates for the shadows
#endif
#ifdef WRAPPER_USE_GPU_WIND		
	FLOAT			m_fWindIndex;			// GPU Only
	FLOAT			m_fWindWeight;			
#endif
};


///////////////////////////////////////////////////////////////////////  
//	Branch/Frond Vertex Program

static const char g_achSimpleVertexProgram[] = 
{
		"vs.1.1\n"												// identity shader version

		"mov		oT0.xy,		v7\n"							// always pass texcoord0 through

	#ifdef WRAPPER_RENDER_SELF_SHADOWS
		"mov		oT1.xy,		v8\n"							// pass shadow texcoords through if enabled
	#endif

		// retrieve and convert wind matrix index
		"mov		a0.x,	v9.x\n"

		// perform wind interpolation
		"m4x4		r1,			v0,			c[54+a0.x]\n"		// compute full wind effect
		"sub		r2,			r1,			v0\n"				// compute difference between full wind and none
		"mov		r3.x,		v9.y\n"							// mad can't access two v's at once, use r3.x as tmp
		"mad		r1,			r2,			r3.x,		v0\n"	// perform interpolation

		"add		r2,			c[52],		r1\n"				// translate to tree's position
		"m4x4		oPos,		r2,			c[0]\n"				// project to screen

	#ifdef WRAPPER_USE_FOG
		"dp4		r1,			r2,			c[2]\n"				// find distance to vertex
		"sub		r2.x,		c[85].y,	r1.z\n"				// linear fogging
		"mul		oFog,		r2.x,		c[85].z\n"			// write to fog register
	#endif

	#ifdef WRAPPER_USE_STATIC_LIGHTING
		"mov		oD0,		v5\n"							// pass color through
	#else
		"mov		r1,			c[74]\n"						// can only use one const register per instruction
		"mul		r5,			c[73],		r1\n"				// diffuse values
	
		"mov		r1,			c[75]\n"						// can only use one const register per instruction
		"mul		r4,			c[72],		r1\n"				// ambient values
			
		"dp3		r2,		v3,			c[71]\n"				// dot light direction with normal
//		"max		r2.x,		r2.x,		c[70].x\n"			// limit it
		"mad		oD0,		r2.x,		r5,			r4\n"	// compute the final color
	#endif
};


///////////////////////////////////////////////////////////////////////  
//	LoadBranchShader

static LPDIRECT3DVERTEXDECLARATION9 LoadBranchShader()
{
	// branch shader declaration
	D3DVERTEXELEMENT9 pBranchShaderDecl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};

	// assemble shader
	LPDIRECT3DVERTEXDECLARATION9 dwShader = NULL;

	if (STATEMANAGER.CreateVertexDeclaration(pBranchShaderDecl, &dwShader) != D3D_OK)
	{
		char szError[1024];
		sprintf_s(szError, "Failed to create branch vertex shader.");
		MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
	}

	return dwShader;
}

///////////////////////////////////////////////////////////////////////  
//	Leaf Vertex Formats

static DWORD D3DFVF_SPEEDTREE_LEAF_VERTEX =
		D3DFVF_XYZ |							// always have the position
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			// precomputed colors or geometric normals
		D3DFVF_NORMAL |
	#else
		D3DFVF_DIFFUSE |
	#endif
		D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)	// always have first texture layer coords
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT					
		| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE4(2)	// GPU Only - wind weight and index passed in second texture layer
	#endif
		;


/////////////////////////////////////////////////////////////////////// 
// FVF Leaf Vertex Structure

struct SFVFLeafVertex
{
		D3DXVECTOR3		m_vPosition;			// Always Used							
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			
		D3DXVECTOR3		m_vNormal;				// Dynamic Lighting Only			
	#else										     
		DWORD			m_dwDiffuseColor;		// Static Lighting Only	
	#endif											
		FLOAT			m_fTexCoords[2];		// Always Used
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
		FLOAT			m_fWindIndex;			// Only used when GPU is involved
		FLOAT			m_fWindWeight;					
		FLOAT			m_fLeafPlacementIndex;
		FLOAT			m_fLeafScalarValue;
	#endif
};


///////////////////////////////////////////////////////////////////////  
//	Leaf Vertex Program

static const char g_achLeafVertexProgram[] = 
{
		"vs.1.1\n"											// identity shader version

        "dcl_position v0\n"

#ifdef WRAPPER_USE_STATIC_LIGHTING
        "dcl_color v5\n"
#else
        "dcl_normal v3\n"
#endif
        "dcl_texcoord0 v7\n"
#ifdef WRAPPER_USE_GPU_LEAF_PLACEMENT
        "dcl_texcoord2 v9\n"
#endif


		"mov		oT0.xy,	v7\n"							// always pass texcoord0 through
		
	#ifdef WRAPPER_USE_GPU_WIND
		// retrieve and convert wind matrix index
		"mov		a0.x,	v9.x\n"

		// perform wind interpolation
		"m4x4		r1,		v0,			c[54+a0.x]\n"		// compute full wind effect
		"sub		r2,		r1,			v0\n"				// compute difference between full wind and none
		"mov		r3.x,	v9.y\n"							// mad can't access two v's at once, use r3.x as tmp
		"mad		r0,		r2,			r3.x,		v0\n"	// perform interpolation
	#else
		"mov		r0,		v0\n"							// wind already handled, pass the vertex through
	#endif

	#ifdef WRAPPER_USE_GPU_LEAF_PLACEMENT
		"mov		a0.x,	v9.z\n"							// place the leaves
		"mul		r1,		c[a0.x],	v9.w\n"
		"add		r0,		r1,			r0\n"	
	#endif

		"add		r0,		c[52],		r0\n"				// translate to tree's position
		"m4x4		oPos,	r0,			c[0]\n"				// project to screen

	#ifdef WRAPPER_USE_FOG
		"dp4		r1,			r0,			c[2]\n"			// find distance to vertex
		"sub		r2.x,		c[85].y,	r1.z\n"			// 
		"mul		oFog,		r2.x,		c[85].z\n"
	#endif

	#ifdef WRAPPER_USE_STATIC_LIGHTING
		"mov		oD0,	v5\n"							// pass color through
	#else
		"mov		r1,		c[74]\n"						// can only use one const register per instruction
		"mul		r5,		c[73],		r1\n"				// diffuse values

		"mov		r1,		c[75]\n"						// can only use one const register per instruction
		"mul		r4,		c[72],		r1\n"				// ambient values
		
		"dp3		r2.x,   v3,			c[71]\n"			// dot light direction with normal
		"max		r2.x,	r2.x,		c[70].x\n"			// limit it
		"mad		oD0,	r2.x,		r5,			r4\n"	// compute the final color
	#endif
};


///////////////////////////////////////////////////////////////////////  
//	LoadLeafShader

static void LoadLeafShader(LPDIRECT3DVERTEXDECLARATION9& pVertexDecl, LPDIRECT3DVERTEXSHADER9& pVertexShader)
{
    SAFE_RELEASE(pVertexDecl);
    SAFE_RELEASE(pVertexShader);

	const D3DVERTEXELEMENT9 leafVertexDecl[] = {
			{ 0,  0, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 },
#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,        0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,      0 },
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
			{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,      2 },
	#endif
#else
			{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,       0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,      0 },
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
			{ 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,      2 },
	#endif
#endif
			D3DDECL_END()
	};

    LPD3DXBUFFER pCode = nullptr, pError = nullptr;
    if (D3DXAssembleShader(g_achLeafVertexProgram, sizeof(g_achLeafVertexProgram) - 1, nullptr, nullptr, 0, &pCode, &pError) == D3D_OK) {
        if (STATEMANAGER.CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &pVertexShader) != D3D_OK) {
            TraceError("Failed to create leaf vertex shader.");
        }
    }
    else {
        TraceError("Failed to assemble leaf vertex shader. The error reported is [ %s ].", pError->GetBufferPointer());
    }

    if (FAILED(STATEMANAGER.CreateVertexDeclaration(leafVertexDecl, &pVertexDecl))) {
        TraceError("Failed to create leaf vertex declaration");
    }

    SAFE_RELEASE(pCode);
    SAFE_RELEASE(pError);
}




