#pragma once

#include "../eterBase/Singleton.h"

#include "GrpBase.h"
#include "Util.h"
#include "Pool.h"

#include <deque>

typedef DWORD TLightID;

enum ELightType
{
	LIGHT_TYPE_STATIC,			// Continuously turning on light
	LIGHT_TYPE_DYNAMIC,			// Immediately turning off light
};

class CLight : public CGraphicBase
{
	public:
		CLight();
		virtual ~CLight();

		void		Initialize();
		void		Clear();

		void		SetParameter(TLightID id, const D3DLIGHT9 & c_rLight);

		TLightID	GetLightID()	{ return m_LightID;		}

		BOOL		isEdited()		{ return m_isEdited;	}
		void		SetDeviceLight(BOOL bActive);

		void		SetDiffuseColor(float fr, float fg, float fb, float fa = 1.0f);
		void		SetAmbientColor(float fr, float fg, float fb, float fa = 1.0f);
		void		SetRange(float fRange);
		void		SetPosition(float fx, float fy, float fz);

		const D3DVECTOR & GetPosition() const;

	private:
		TLightID		m_LightID;		// Light ID. equal to D3D light index

		D3DLIGHT9		m_d3dLight;
		BOOL			m_isEdited;
};

class CLightManager : public CGraphicBase, public CSingleton<CLightManager>
{
	public:
		typedef std::deque<TLightID>			TLightIDDeque;
		typedef std::map<TLightID, CLight *>	TLightMap;

	public:
		CLightManager();
		virtual ~CLightManager();

		void		Destroy();

		void		Initialize();

		void		RegisterLight(ELightType LightType, TLightID * poutLightID, D3DLIGHT9 & LightData);
		CLight *	GetLight(TLightID LightID);
		void		DeleteLight(TLightID LightID);

		void		SetSkipIndex(DWORD dwSkipIndex);

	protected:
		TLightIDDeque			m_NonUsingLightIDDeque;

		TLightMap				m_LightMap;

		DWORD					m_dwSkipIndex;

	protected:
		TLightID				NewLightID();
		void					ReleaseLightID(TLightID LightID);

		CDynamicPool<CLight>	m_LightPool;
};
