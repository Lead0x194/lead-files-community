#include "StdAfx.h"
#include "GrpDynamicDrawDX12.h"

bool CGraphicDynamicDrawDX12::WriteVertices(CGraphicUploadRingDX12& rkRing,
											const void* pvVertices,
											UINT uStrideBytes,
											UINT uVertexCount,
											D3D12_VERTEX_BUFFER_VIEW* pkViewOut)
{
	if (!pvVertices || !uStrideBytes || !uVertexCount)
		return false;

	const UINT64 uByteSize = static_cast<UINT64>(uStrideBytes) * uVertexCount;

	CGraphicUploadRingDX12::TAllocation kAllocation;
	if (!rkRing.Allocate(uByteSize, 4, &kAllocation))
		return false;

	memcpy(kAllocation.pvCPUAddress, pvVertices, static_cast<size_t>(uByteSize));

	pkViewOut->BufferLocation = kAllocation.uGPUAddress;
	pkViewOut->SizeInBytes = static_cast<UINT>(uByteSize);
	pkViewOut->StrideInBytes = uStrideBytes;
	return true;
}

bool CGraphicDynamicDrawDX12::WriteIndices(CGraphicUploadRingDX12& rkRing,
										   const WORD* awIndices,
										   UINT uIndexCount,
										   D3D12_INDEX_BUFFER_VIEW* pkViewOut)
{
	if (!awIndices || !uIndexCount)
		return false;

	const UINT64 uByteSize = static_cast<UINT64>(uIndexCount) * sizeof(WORD);

	CGraphicUploadRingDX12::TAllocation kAllocation;
	if (!rkRing.Allocate(uByteSize, 4, &kAllocation))
		return false;

	memcpy(kAllocation.pvCPUAddress, awIndices, static_cast<size_t>(uByteSize));

	pkViewOut->BufferLocation = kAllocation.uGPUAddress;
	pkViewOut->SizeInBytes = static_cast<UINT>(uByteSize);
	pkViewOut->Format = DXGI_FORMAT_R16_UINT;
	return true;
}

bool CGraphicDynamicDrawDX12::WriteConstants(CGraphicUploadRingDX12& rkRing,
											 const void* pvConstants,
											 UINT uByteSize,
											 D3D12_GPU_VIRTUAL_ADDRESS* puAddressOut)
{
	if (!pvConstants || !uByteSize)
		return false;

	CGraphicUploadRingDX12::TAllocation kAllocation;
	if (!rkRing.Allocate(uByteSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, &kAllocation))
		return false;

	memcpy(kAllocation.pvCPUAddress, pvConstants, uByteSize);

	*puAddressOut = kAllocation.uGPUAddress;
	return true;
}
