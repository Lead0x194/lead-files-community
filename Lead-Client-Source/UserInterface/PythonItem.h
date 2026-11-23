#pragma once

// Note: Responsible for updating and rendering items dropped on the screen
// Supervises the data of each item and plays the role of manager of the Icon Image Instance.
// It seems like things have gotten a little messy - 2003. 01. 13. [levites]

#include "../EterGrnLib/ThingInstance.h"

class CItemData;

class CPythonItem : public CSingleton<CPythonItem>
{
	public:
		enum
		{
			INVALID_ID = 0xffffffff,
		};

		enum
		{
			VNUM_MONEY = 1,
		};

		enum
		{
			USESOUND_NONE,
			USESOUND_DEFAULT,
			USESOUND_ARMOR,
			USESOUND_WEAPON,
			USESOUND_BOW,
			USESOUND_ACCESSORY,
			USESOUND_POTION,
			USESOUND_PORTAL,
			USESOUND_NUM,
		};

		enum
		{
			DROPSOUND_DEFAULT,
			DROPSOUND_ARMOR,
			DROPSOUND_WEAPON,
			DROPSOUND_BOW,
			DROPSOUND_ACCESSORY,
			DROPSOUND_NUM
		};

		typedef struct SGroundItemInstance
		{
			DWORD					dwVirtualNumber;
			D3DXVECTOR3				v3EndPosition;

			D3DXVECTOR3				v3RotationAxis;
			D3DXQUATERNION			qEnd;
			D3DXVECTOR3				v3Center;
			CGraphicThingInstance	ThingInstance;
			DWORD					dwStartTime;
			DWORD					dwEndTime;

			DWORD					eDropSoundType;

			bool					bAnimEnded;
			bool Update();
			void Clear();

			DWORD					dwEffectInstanceIndex;
			std::string				stOwnership;

			static void	__PlayDropSound(DWORD eItemType, const D3DXVECTOR3& c_rv3Pos);
			static std::string		ms_astDropSoundFileName[DROPSOUND_NUM];

			SGroundItemInstance() {}
			virtual ~SGroundItemInstance() {}
		} TGroundItemInstance;

		typedef std::map<DWORD, TGroundItemInstance *>	TGroundItemInstanceMap;

	public:
		CPythonItem(void);
		virtual ~CPythonItem(void);

		// Initialize
		void	Destroy();
		void	Create();

		void	PlayUseSound(DWORD dwItemID);
		void	PlayDropSound(DWORD dwItemID);
		void	PlayUsePotionSound();

		void	SetUseSoundFileName(DWORD eItemType, const std::string& c_rstFileName);
		void	SetDropSoundFileName(DWORD eItemType, const std::string& c_rstFileName);

		void	GetInfo(std::string* pstInfo);

		void	DeleteAllItems();

		void	Render();
		void	Update(const POINT& c_rkPtMouse);

		void	CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, bool bDrop=true);
		void	DeleteItem(DWORD dwVirtualID);		
		void	SetOwnership(DWORD dwVID, const char * c_pszName);
		bool	GetOwnership(DWORD dwVID, const char ** c_pszName);

		BOOL	GetGroundItemPosition(DWORD dwVirtualID, TPixelPosition * pPosition);

		bool	GetPickedItemID(DWORD* pdwPickedItemID);

		bool	GetCloseItem(const TPixelPosition & c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance=300);
		bool	GetCloseMoney(const TPixelPosition & c_rPixelPosition, DWORD* dwItemID, DWORD dwDistance=300);

		DWORD	GetVirtualNumberOfGroundItem(DWORD dwVID);

		void	BuildNoGradeNameData(int iType);
		DWORD	GetNoGradeNameDataCount();
		CItemData * GetNoGradeNameDataPtr(DWORD dwIndex);

	protected:
		DWORD	__Pick(const POINT& c_rkPtMouse);

		DWORD	__GetUseSoundType(const CItemData& c_rkItemData);
		DWORD	__GetDropSoundType(const CItemData& c_rkItemData);

	protected:
		TGroundItemInstanceMap				m_GroundItemInstanceMap;

	public:
		void	RefreshNames();

	protected:
		CDynamicPool<TGroundItemInstance>	m_GroundItemInstancePool;

		DWORD m_dwDropItemEffectID;
		DWORD m_dwPickedItemID;

		int m_nMouseX;
		int m_nMouseY;

		std::string m_astUseSoundFileName[USESOUND_NUM];

		std::vector<CItemData *> m_NoGradeNameItemData;
};