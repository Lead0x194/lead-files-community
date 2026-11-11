#include "StdAfx.h"

#include "GrpLightManager.h"
#include "StateManager.h"

CLightManager::CLightManager()
{
}

CLightManager::~CLightManager()
{
}

void CLightManager::Destroy()
{
	m_LightPool.Destroy();
}

void CLightManager::Initialize()
{
	SetSkipIndex(1);

	m_NonUsingLightIDDeque.clear();

	m_LightMap.clear();
	m_LightPool.FreeAll();
}

void CLightManager::RegisterLight(ELightType /*LightType*/, TLightID * poutLightID, D3DLIGHT9 & LightData)
{
	CLight * pLight = m_LightPool.Alloc();
	TLightID ID = NewLightID();
	pLight->SetParameter(ID, LightData);
	m_LightMap[ID] = pLight;
	*poutLightID = ID;
}

void CLightManager::DeleteLight(TLightID LightID)
{
	TLightMap::iterator itor = m_LightMap.find(LightID);

	if (m_LightMap.end() == itor)
	{
		assert(!"CLightManager::DeleteLight - Failed to find light ID!");
		return;
	}

	CLight * pLight = itor->second;

	pLight->Clear();
	m_LightPool.Free(pLight);

	m_LightMap.erase(itor);

	ReleaseLightID(LightID);
}

CLight * CLightManager::GetLight(TLightID LightID)
{
	TLightMap::iterator itor = m_LightMap.find(LightID);

	if (m_LightMap.end() == itor)
	{
		assert(!"CLightManager::SetLightData - Failed to find light ID!");
		return NULL;
	}

	return itor->second;
}

void CLightManager::SetSkipIndex(DWORD dwSkipIndex)
{
	m_dwSkipIndex = dwSkipIndex;
}

TLightID CLightManager::NewLightID()
{
	if (!m_NonUsingLightIDDeque.empty())
	{
		TLightID id = m_NonUsingLightIDDeque.back();
		m_NonUsingLightIDDeque.pop_back();
		return (id);
	}

	return static_cast<TLightID>(m_dwSkipIndex + m_LightMap.size());
}

void CLightManager::ReleaseLightID(TLightID LightID)
{
	m_NonUsingLightIDDeque.push_back(LightID);
}

//////////////////////////////////////////////////////////////////////////
CLight::CLight()
{
	Initialize();
}

CLight::~CLight()
{
	Clear();
}

void CLight::Initialize()
{
	m_LightID	= 0;
	m_isEdited	= TRUE;

	memset(&m_d3dLight, 0, sizeof(m_d3dLight));

	m_d3dLight.Type			= D3DLIGHT_POINT;
	m_d3dLight.Attenuation0	= 0.0f;
	m_d3dLight.Attenuation1	= 1.0f;
	m_d3dLight.Attenuation2	= 0.0f;
}

void CLight::Clear()
{
	if (m_LightID)
		SetDeviceLight(FALSE);
	Initialize();
}

void CLight::SetDeviceLight(BOOL bActive)
{
	if (bActive && m_isEdited)
	{
		if (ms_lpd3dDevice)
			ms_lpd3dDevice->SetLight(m_LightID, &m_d3dLight);
	}
	if (ms_lpd3dDevice)
	{
		ms_lpd3dDevice->LightEnable(m_LightID, bActive);
	}
}

void CLight::SetParameter(TLightID id, const D3DLIGHT9 & c_rLight)
{
	m_LightID	= id;
	m_d3dLight	= c_rLight;
}

void CLight::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	if (m_d3dLight.Diffuse.r == fr
		&& m_d3dLight.Diffuse.g == fg
		&& m_d3dLight.Diffuse.b == fb
		&& m_d3dLight.Diffuse.a == fa
		)
		return;
	m_d3dLight.Diffuse.r = fr;
	m_d3dLight.Diffuse.g = fg;
	m_d3dLight.Diffuse.b = fb;
	m_d3dLight.Diffuse.a = fa;
	m_isEdited = TRUE;
}

void CLight::SetAmbientColor(float fr, float fg, float fb, float fa)
{
	if (m_d3dLight.Ambient.r == fr
		&& m_d3dLight.Ambient.g == fg
		&& m_d3dLight.Ambient.b == fb
		&& m_d3dLight.Ambient.a == fa
		)
		return;
	m_d3dLight.Ambient.r = fr;
	m_d3dLight.Ambient.g = fg;
	m_d3dLight.Ambient.b = fb;
	m_d3dLight.Ambient.a = fa;
	m_isEdited = TRUE;
}

void CLight::SetRange(float fRange)
{
	if (m_d3dLight.Range == fRange)
		return;

	m_d3dLight.Range = fRange;
	m_isEdited = TRUE;
}

const D3DVECTOR & CLight::GetPosition() const
{
	return m_d3dLight.Position;
}

void CLight::SetPosition(float fx, float fy, float fz)
{
	if (m_d3dLight.Position.x == fx && m_d3dLight.Position.y == fy && m_d3dLight.Position.z == fz)
		return;

	m_d3dLight.Position.x = fx;
	m_d3dLight.Position.y = fy;
	m_d3dLight.Position.z = fz;
	m_isEdited = TRUE;
}
