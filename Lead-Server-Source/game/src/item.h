#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		CItem(DWORD dwVnum);
		virtual ~CItem();

		int			GetLevelLimit();

		bool		CheckItemUseLevel(int nLevel);

		long		FindApplyValue(BYTE bApplyType);

		bool		IsStackable()		{ return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false; }

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow(BYTE b)	{ m_bWindow = b; }
		BYTE		GetWindow()		{ return m_bWindow; }

		void		SetID(DWORD id)		{ m_dwID = id;	}
		DWORD		GetID()			{ return m_dwID; }

		void			SetProto(const TItemTable * table);
		TItemTable const *	GetProto()	{ return m_pProto; }

		GoldType	GetGold();
		GoldType	GetShopBuyPrice();
		const char *	GetName()		{ return m_pProto ? m_pProto->szName : NULL; }
		const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
		BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

		void		SetFlag(long flag)	{ m_lFlag = flag;	}
		long		GetFlag()		{ return m_lFlag;	}

		void		AddFlag(long bit);
		void		RemoveFlag(long bit);

		DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
		DWORD		GetAntiFlag()		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
		DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

		void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
		DWORD		GetVID()		{ return m_dwVID;	}

		bool		SetCount(DWORD count);
		DWORD		GetCount();

		DWORD		GetVnum() const		{ return m_dwVnum;	}
		BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
		BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
		BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
		long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}

		long		GetValue(DWORD idx);

		void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
		WORD		GetCell()				{ return m_wCell;	}

		LPITEM		RemoveFromCharacter();
		bool		AddToCharacter(LPCHARACTER ch, TItemPos Cell);
		LPCHARACTER	GetOwner()		{ return m_pOwner; }

		LPITEM		RemoveFromGround();
		bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

		int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
		bool		IsEquipped() const		{ return m_bEquipped;	}
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
		bool		IsEquipable() const;

		bool		CanUsedBy(LPCHARACTER ch);

		bool		DistanceValid(LPCHARACTER ch);

		void		UpdatePacket();
		void		SetExchanging(bool isOn = true);
		bool		IsExchanging()		{ return m_bExchanging;	}

		bool		IsPolymorphItem();

		void		ModifyPoints(bool bAdd);	// Gives the effect of an item to the character . bAdd go false This side is removed.

		bool		CreateSocket(BYTE bSlot, BYTE bGold);
		const TimeT64 *	GetSockets()		{ return &m_alSockets[0];	}
		TimeT64		GetSocket(int i)	{ return m_alSockets[i];	}

		void		SetSockets(const TimeT64 * al);
		void		SetSocket(int i, TimeT64 v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	} 
		const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

		BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
		short		GetAttributeValue(int i){ return m_aAttr[i].sValue;	}

		void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);
		
		int		FindAttribute(BYTE bType);
		bool		RemoveAttributeAt(int index);
		bool		RemoveAttributeType(BYTE bType);

		bool		HasAttr(BYTE bApply);
		bool		HasRareAttr(BYTE bApply);

		void		SetDestroyEvent(LPEVENT pkEvent);
		void		StartDestroyEvent(int iSec=300);

		DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
		bool		GetSkipSave()		{ return m_bSkipSave; }

		bool		IsOwnership(LPCHARACTER ch);
		void		SetOwnership(LPCHARACTER ch, int iSec = 10);
		void		SetOwnershipEvent(LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }
		void		SetLastOwnerPID(DWORD pid) { m_dwLastOwnerPID = pid; }

		int		GetAttributeSetIndex(); // Returns which index of the array is used to attach the attribute. .
		void		AlterToMagicItem();
		void		AlterToSocketItem(int iSocketCount);

		WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent(LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

		void		StartRealTimeExpireEvent();
		bool		IsRealTimeItem();

		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();

		//			first REAL_TIME class TIMER_BASED_ON_WEAR Only works properly for items .
		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void		ChangeAttribute(const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute(BYTE bType, short sValue);

		void		ApplyAddon(int iAddonType);

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup(const LPITEM item) const;

		// ACCESSORY_REFINE
		// Add a socket via mine to accessories
		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade(int iGrade);
		void		SetAccessorySocketMaxGrade(int iMaxGrade);
		void		SetAccessorySocketDownGradeTime(DWORD time);

		void		AccessorySocketDegrade();

		// The timer starts when an accessory is placed on an item. ( copper , etc. )
		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

		bool		CanPutInto(LPITEM item);
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo(LPITEM pItem);
		void		CopySocketTo(LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock(bool f) { m_isLocked = f; }
		bool		isLocked() const { return m_isLocked; }

	private :
		void		SetAttribute(int i, BYTE bType, short sValue);
	public:
		void		SetForceAttribute(int i, BYTE bType, short sValue);

	protected:
		bool		EquipEx(bool is_equip);
		bool		Unequip();

		void		AddAttr(BYTE bApply, BYTE bLevel);
		void		PutAttribute(const int * aiAttrPercentTable);
		void		PutAttributeWithLevel(BYTE bLevel);

	protected:
		friend class CInputDB;
		bool		OnAfterCreatedItem();			// Items are fully created on the server with all information ( load ) A function called after .

	public:
		bool		IsRideItem();
		bool		IsRamadanRing();

		void		ClearMountAttributeAndAffect();
		bool		IsNewMountItem();

		bool		IsDragonSoul();
		int			GiveMoreTime_Per(float fPercent);
		int			GiveMoreTime_Fix(DWORD dwTime);

	private:
		TItemTable const * m_pProto;		// prototype

		DWORD		m_dwVnum;
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;		// The window where the current item is located 
		DWORD		m_dwID;			// Unique number
		bool		m_bEquipped;	// Is it installed? ?
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;		// location
		DWORD		m_dwCount;		// count
		long		m_lFlag;		// addition flag
		DWORD		m_dwLastOwnerPID;	// the last person who had it PID

		bool		m_bExchanging;	///< Currently exchanging status 

		TimeT64		m_alSockets[ITEM_SOCKET_MAX_NUM];	// Item Socket
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;

		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;
		
		DWORD		m_dwSIGVnum;
	public:
		void SetSIGVnum(DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info() 
	: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;

	item_vid_event_info() 
	: item_vid( 0 )
	{
	}
};

#endif
