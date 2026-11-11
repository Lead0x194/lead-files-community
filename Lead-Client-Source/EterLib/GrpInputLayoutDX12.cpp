#include "StdAfx.h"
#include "GrpInputLayoutDX12.h"

bool CGraphicInputLayoutDX12::Build(const D3DVERTEXELEMENT9* pkElements9,
									D3D12_INPUT_ELEMENT_DESC* akElementsOut,
									UINT uMaxElementCount,
									UINT* puElementCountOut)
{
	*puElementCountOut = 0;

	if (!pkElements9)
		return false;

	UINT uCount = 0;
	for (const D3DVERTEXELEMENT9* pkElement = pkElements9; 0xFF != pkElement->Stream; ++pkElement)
	{
		if (uCount >= uMaxElementCount)
		{
			TraceError("CGraphicInputLayoutDX12: declaration exceeds %u elements.", uMaxElementCount);
			return false;
		}

		const DXGI_FORMAT eFormat = ToFormatDX12(pkElement->Type);
		if (DXGI_FORMAT_UNKNOWN == eFormat)
		{
			TraceError("CGraphicInputLayoutDX12: no DX12 format for decl type %d.",
					   static_cast<int>(pkElement->Type));
			return false;
		}

		const char* c_szSemantic = ToSemanticNameDX12(pkElement->Usage);
		if (!c_szSemantic)
		{
			TraceError("CGraphicInputLayoutDX12: no DX12 semantic for decl usage %d.",
					   static_cast<int>(pkElement->Usage));
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC& rkElement = akElementsOut[uCount];
		rkElement.SemanticName = c_szSemantic;
		rkElement.SemanticIndex = pkElement->UsageIndex;
		rkElement.Format = eFormat;
		rkElement.InputSlot = pkElement->Stream;
		rkElement.AlignedByteOffset = pkElement->Offset;
		rkElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		rkElement.InstanceDataStepRate = 0;
		++uCount;
	}

	*puElementCountOut = uCount;
	return uCount > 0;
}

DXGI_FORMAT CGraphicInputLayoutDX12::ToFormatDX12(BYTE byDeclTypeD3D9)
{
	switch (byDeclTypeD3D9)
	{
		case D3DDECLTYPE_FLOAT1:	return DXGI_FORMAT_R32_FLOAT;
		case D3DDECLTYPE_FLOAT2:	return DXGI_FORMAT_R32G32_FLOAT;
		case D3DDECLTYPE_FLOAT3:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case D3DDECLTYPE_FLOAT4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case D3DDECLTYPE_D3DCOLOR:	return DXGI_FORMAT_B8G8R8A8_UNORM;
		case D3DDECLTYPE_UBYTE4:	return DXGI_FORMAT_R8G8B8A8_UINT;
		case D3DDECLTYPE_SHORT2:	return DXGI_FORMAT_R16G16_SINT;
		case D3DDECLTYPE_SHORT4:	return DXGI_FORMAT_R16G16B16A16_SINT;
		case D3DDECLTYPE_UBYTE4N:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case D3DDECLTYPE_SHORT2N:	return DXGI_FORMAT_R16G16_SNORM;
		case D3DDECLTYPE_SHORT4N:	return DXGI_FORMAT_R16G16B16A16_SNORM;
		case D3DDECLTYPE_USHORT2N:	return DXGI_FORMAT_R16G16_UNORM;
		case D3DDECLTYPE_USHORT4N:	return DXGI_FORMAT_R16G16B16A16_UNORM;
		case D3DDECLTYPE_FLOAT16_2:	return DXGI_FORMAT_R16G16_FLOAT;
		case D3DDECLTYPE_FLOAT16_4:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

const char* CGraphicInputLayoutDX12::ToSemanticNameDX12(BYTE byDeclUsageD3D9)
{
	switch (byDeclUsageD3D9)
	{
		case D3DDECLUSAGE_POSITION:		return "POSITION";
		case D3DDECLUSAGE_BLENDWEIGHT:	return "BLENDWEIGHT";
		case D3DDECLUSAGE_BLENDINDICES:	return "BLENDINDICES";
		case D3DDECLUSAGE_NORMAL:		return "NORMAL";
		case D3DDECLUSAGE_PSIZE:		return "PSIZE";
		case D3DDECLUSAGE_TEXCOORD:		return "TEXCOORD";
		case D3DDECLUSAGE_TANGENT:		return "TANGENT";
		case D3DDECLUSAGE_BINORMAL:		return "BINORMAL";
		case D3DDECLUSAGE_COLOR:		return "COLOR";
	}
	return NULL;
}
