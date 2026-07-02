// TerrainPatch.cpp: implementation of the CTerrainPatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TerrainPatch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CTerrainPatch::Clear()
{
	m_kHT.m_kVB.Destroy();

	m_WaterVertexBuffer.Destroy();
	ClearID();
	SetUse(false);

	m_bWaterExist = false;
	m_bNeedUpdate = true;

	m_dwWaterPriCount = 0;
	m_byType = PATCH_TYPE_PLAIN;
	
	m_fMinX = m_fMaxX = m_fMinY = m_fMaxY = m_fMinZ = m_fMaxZ = 0.0f;

	m_dwVersion=0;
}

void CTerrainPatch::BuildWaterVertexBuffer(SWaterVertex* akSrcVertex, UINT uWaterVertexCount)
{
	CGraphicVertexBuffer& rkVB=m_WaterVertexBuffer;

	if (!rkVB.Create(uWaterVertexCount, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT)) 
		return;
	
	SWaterVertex* akDstWaterVertex;
	if (rkVB.Lock((void **) &akDstWaterVertex))
	{
		UINT uVBSize=sizeof(SWaterVertex)*uWaterVertexCount;
		memcpy(akDstWaterVertex, akSrcVertex, uVBSize);
		m_dwWaterPriCount=uWaterVertexCount/3;

		rkVB.Unlock();		
	}	
}
		
void CTerrainPatch::BuildTerrainVertexBuffer(HardwareTransformPatch_SSourceVertex* akSrcVertex)
{
	__BuildHardwareTerrainVertexBuffer(akSrcVertex);
}

void CTerrainPatch::__BuildHardwareTerrainVertexBuffer(HardwareTransformPatch_SSourceVertex* akSrcVertex)
{
	
	CGraphicVertexBuffer& rkVB=m_kHT.m_kVB;
	if (!rkVB.Create(TERRAIN_VERTEX_COUNT, D3DFVF_XYZ | D3DFVF_NORMAL, D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT)) 
		return;
	
	HardwareTransformPatch_SSourceVertex* akDstVertex;
	if (rkVB.Lock((void **) &akDstVertex))
	{
		UINT uVBSize=sizeof(HardwareTransformPatch_SSourceVertex)*TERRAIN_VERTEX_COUNT;
	
		memcpy(akDstVertex, akSrcVertex, uVBSize);
		rkVB.Unlock();		
	}
}

UINT CTerrainPatch::GetWaterFaceCount()
{
	return m_dwWaterPriCount;
}

CTerrainPatchProxy::CTerrainPatchProxy()
{
	Clear();
}

CTerrainPatchProxy::~CTerrainPatchProxy()
{
	Clear();
}

void CTerrainPatchProxy::SetCenterPosition(const D3DXVECTOR3& c_rv3Center)
{
	m_v3Center=c_rv3Center;
}

bool CTerrainPatchProxy::IsIn(const D3DXVECTOR3& c_rv3Target, float fRadius)
{
	float dx=m_v3Center.x-c_rv3Target.x;
	float dy=m_v3Center.y-c_rv3Target.y;
	float fDist=dx*dx+dy*dy;
	float fCheck=fRadius*fRadius;

	if (fDist<fCheck)
		return true;

	return false;
}

CGraphicVertexBuffer* CTerrainPatchProxy::HardwareTransformPatch_GetVertexBufferPtr()
{
	if (m_pTerrainPatch)
		return m_pTerrainPatch->HardwareTransformPatch_GetVertexBufferPtr();

	return NULL;
}

UINT CTerrainPatchProxy::GetWaterFaceCount()
{
	if (m_pTerrainPatch)
		return m_pTerrainPatch->GetWaterFaceCount();
	
	return 0;
}

void CTerrainPatchProxy::Clear()
{
	m_bUsed = false;
	m_sPatchNum = 0;
	m_byTerrainNum = 0xFF;

	m_pTerrainPatch = NULL;
}
