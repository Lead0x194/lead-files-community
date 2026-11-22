#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "NetworkActorManager.h"
#include "PythonBackground.h"

#include "PythonApplication.h"
#include "AbstractPlayer.h"
#include "../gamelib/ActorInstance.h"





void CPythonNetworkStream::__GlobalPositionToLocalPosition(int32_t& rGlobalX, int32_t& rGlobalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(rGlobalX, rGlobalY);
}

void CPythonNetworkStream::__LocalPositionToGlobalPosition(int32_t& rLocalX, int32_t& rLocalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.LocalPositionToGlobalPosition(rLocalX, rLocalY);
}

bool CPythonNetworkStream::__CanActMainInstance()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanAct();
}

void CPythonNetworkStream::__ClearNetworkActorManager()
{
	m_rokNetActorMgr->Destroy();
}

void __SetWeaponPower(IAbstractPlayer& rkPlayer, DWORD dwWeaponID)
{
	DWORD minPower=0;
	DWORD maxPower=0;
	DWORD minMagicPower=0;
	DWORD maxMagicPower=0;
	DWORD addPower=0;

	CItemData* pkWeapon;
	if (CItemManager::Instance().GetItemDataPointer(dwWeaponID, &pkWeapon))
	{
		if (pkWeapon->GetType()==CItemData::ITEM_TYPE_WEAPON)
		{
			minPower=pkWeapon->GetValue(3);
			maxPower=pkWeapon->GetValue(4);
			minMagicPower=pkWeapon->GetValue(1);
			maxMagicPower=pkWeapon->GetValue(2);
			addPower=pkWeapon->GetValue(5);
		}
	}

	rkPlayer.SetWeaponPower(minPower, maxPower, minMagicPower, maxMagicPower, addPower);
}

// In the table the name is "." things that are
// A function that will disappear if the server no longer sends it in the future. (Be sure to threaten the server!)
bool IsInvisibleRace(WORD raceNum)
{
	switch(raceNum)
	{
	case 20025:
	case 20038:
	case 20039:
		return true;
	default:
		return false;
	}
}

static SNetworkActorData s_kNetActorData;


bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_bType=chrAddPacket.bType;
	kNetActorData.m_dwMovSpd=chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;
	
	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_fRot=chrAddPacket.angle;

	kNetActorData.m_stName="";

	kNetActorData.m_stName="";
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	
	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);

	kNetActorData.m_sAlignment=0;/*chrAddPacket.sAlignment*/;	
	kNetActorData.m_byPKMode=0;/*chrAddPacket.bPKMode*/;	
	kNetActorData.m_dwGuildID=0;/*chrAddPacket.dwGuild*/;
	kNetActorData.m_dwEmpireID=0;/*chrAddPacket.bEmpire*/;
	kNetActorData.m_dwArmor=0;/*chrAddPacket.awPart[CHR_EQUIPPART_ARMOR]*/;
	kNetActorData.m_dwWeapon=0;/*chrAddPacket.awPart[CHR_EQUIPPART_WEAPON]*/;
	kNetActorData.m_dwHair=0;/*chrAddPacket.awPart[CHR_EQUIPPART_HAIR]*/;	
	kNetActorData.m_dwMountVnum=0;/*chrAddPacket.dwMountVnum*/;	

	kNetActorData.m_dwLevel = 0; // No monster level display

	if(kNetActorData.m_bType != CActorInstance::TYPE_PC && 
		kNetActorData.m_bType != CActorInstance::TYPE_NPC)
	{
		const char * c_szName;
		CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
		if (rkNonPlayer.GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;
		//else
		//	kNetActorData.m_stName=chrAddPacket.name;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		s_kNetActorData = kNetActorData;
	}

	return true;
}

bool CPythonNetworkStream::RecvCharacterAdditionalInfo()
{
	TPacketGCCharacterAdditionalInfo chrInfoPacket;
	if (!Recv(sizeof(chrInfoPacket), &chrInfoPacket))
		return false;

	
	SNetworkActorData kNetActorData = s_kNetActorData;
	if (IsInvisibleRace(static_cast<WORD>(kNetActorData.m_dwRace)))
		return true;

	if(kNetActorData.m_dwVID == chrInfoPacket.dwVID)
	{
		const char * c_szName;
		if (!chrInfoPacket.name[0] && CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;
		else
			kNetActorData.m_stName = chrInfoPacket.name;
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwLevel = chrInfoPacket.dwLevel;
		kNetActorData.m_sAlignment=chrInfoPacket.sAlignment;	
		kNetActorData.m_byPKMode=chrInfoPacket.bPKMode;	
		kNetActorData.m_dwGuildID=chrInfoPacket.dwGuildID;
		kNetActorData.m_dwEmpireID=chrInfoPacket.bEmpire;
		kNetActorData.m_dwArmor=chrInfoPacket.awPart[CHR_EQUIPPART_ARMOR];
		kNetActorData.m_dwWeapon=chrInfoPacket.awPart[CHR_EQUIPPART_WEAPON];
		kNetActorData.m_dwHair=chrInfoPacket.awPart[CHR_EQUIPPART_HAIR];	
		kNetActorData.m_dwMountVnum=chrInfoPacket.dwMountVnum;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		TraceError("TPacketGCCharacterAdditionalInfo name=%s vid=%d race=%d Error",chrInfoPacket.name,chrInfoPacket.dwVID,kNetActorData.m_dwRace);
	}
	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacket()
{
	TPacketGCCharacterUpdate chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
	kNetUpdateActorData.m_dwGuildID=chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd=chrUpdatePacket.bMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd=chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor=chrUpdatePacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon=chrUpdatePacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair=chrUpdatePacket.awPart[CHR_EQUIPPART_HAIR];
	kNetUpdateActorData.m_dwVID=chrUpdatePacket.dwVID;	
	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);
	kNetUpdateActorData.m_sAlignment=chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode=chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags=chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum=chrUpdatePacket.dwMountVnum;
	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

void CPythonNetworkStream::__RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData)
{
	// NOTE: To resolve the issue of the camera being buried in the ground, the main character was placed on the terrain.
	// You need to update the map before so you can get the height.
	// However, this is done only after the character has already been added, not when entering the game.
	// But when it comes to movement, why do you use Append instead of Move? - [levites]
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetActorData->m_dwVID))
	{
		rkPlayer.SetRace(pkNetActorData->m_dwRace);

		__SetWeaponPower(rkPlayer, pkNetActorData->m_dwWeapon);

		if (rkPlayer.NEW_GetMainActorPtr())
		{
			CPythonBackground::Instance().Update(static_cast<float>(pkNetActorData->m_lCurX), static_cast<float>(pkNetActorData->m_lCurY), 0.0f);
			CPythonCharacterManager::Instance().Update();

			// NOTE: In case of a private tower, the map name is displayed even when moving with GOTO.
			{
				std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
				if (strMapName == "metin2_map_deviltower1")
					__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
			}
		}
		else
		{
			__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
		}
	}

	m_rokNetActorMgr->AppendActor(*pkNetActorData);

	if (GetMainActorVID()==pkNetActorData->m_dwVID)
	{
		rkPlayer.SetTarget(0);
		if (m_bComboSkillFlag)
			rkPlayer.SetComboSkillFlag(m_bComboSkillFlag);

		__SetGuildID(pkNetActorData->m_dwGuildID);
		//CPythonApplication::Instance().SkipRenderBuffering(10000);
	}
}

void CPythonNetworkStream::__RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData)
{
	m_rokNetActorMgr->UpdateActor(*pkNetUpdateActorData);

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetUpdateActorData->m_dwVID))
	{
		__SetGuildID(pkNetUpdateActorData->m_dwGuildID);
		__SetWeaponPower(rkPlayer, pkNetUpdateActorData->m_dwWeapon);

		__RefreshStatus();
		__RefreshAlignmentWindow();
		__RefreshEquipmentWindow();
		__RefreshInventoryWindow();
	}
	else
	{
		rkPlayer.NotifyCharacterUpdate(pkNetUpdateActorData->m_dwVID);
	}
}

bool CPythonNetworkStream::RecvCharacterDeletePacket()
{
	TPacketGCCharacterDelete chrDeletePacket;

	if (!Recv(sizeof(chrDeletePacket), &chrDeletePacket))
	{
		TraceError("CPythonNetworkStream::RecvCharacterDeletePacket - Recv Error");
		return false;
	}

	m_rokNetActorMgr->RemoveActor(chrDeletePacket.id);

	// When a character disappears, the personal store is also deleted.
	// It doesn't matter if you don't have it because we do a key check.
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
		"BINARY_PrivateShop_Disappear", 
		Py_BuildValue("(i)", chrDeletePacket.id)
	);

	return true;
}


bool CPythonNetworkStream::RecvCharacterMovePacket()
{
	TPacketGCMove kMovePacket;
	if (!Recv(sizeof(TPacketGCMove), &kMovePacket))
	{
		Tracen("CPythonNetworkStream::RecvCharacterMovePacket - PACKET READ ERROR");
		return false;
	}

	__GlobalPositionToLocalPosition(kMovePacket.lX, kMovePacket.lY);

	SNetworkMoveActorData kNetMoveActorData;
	kNetMoveActorData.m_dwArg=kMovePacket.bArg;
	kNetMoveActorData.m_dwFunc=kMovePacket.bFunc;
	kNetMoveActorData.m_dwTime=kMovePacket.dwTime;
	kNetMoveActorData.m_dwVID=kMovePacket.dwVID;
	kNetMoveActorData.m_fRot=kMovePacket.bRot*5.0f;
	kNetMoveActorData.m_lPosX=kMovePacket.lX;
	kNetMoveActorData.m_lPosY=kMovePacket.lY;
	kNetMoveActorData.m_dwDuration=kMovePacket.dwDuration;

	m_rokNetActorMgr->MoveActor(kNetMoveActorData);

	return true;
}

bool CPythonNetworkStream::RecvOwnerShipPacket()
{
	TPacketGCOwnership kPacketOwnership;

	if (!Recv(sizeof(kPacketOwnership), &kPacketOwnership))
		return false;

	m_rokNetActorMgr->SetActorOwner(kPacketOwnership.dwOwnerVID, kPacketOwnership.dwVictimVID);

	return true;
}

bool CPythonNetworkStream::RecvSyncPositionPacket()
{
	TPacketGCSyncPosition kPacketSyncPos;
	if (!Recv(sizeof(kPacketSyncPos), &kPacketSyncPos))
		return false;

	TPacketGCSyncPositionElement kSyncPos;

	UINT uSyncPosCount=(kPacketSyncPos.wSize-sizeof(kPacketSyncPos))/sizeof(kSyncPos);
	for (UINT iSyncPos=0; iSyncPos<uSyncPosCount; ++iSyncPos)
	{		
		if (!Recv(sizeof(TPacketGCSyncPositionElement), &kSyncPos))
			return false;

#ifdef __MOVIE_MODE__
		return true;
#endif __MOVIE_MODE__

		//Tracenf("CPythonNetworkStream::RecvSyncPositionPacket %d (%d, %d)", kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);

		__GlobalPositionToLocalPosition(kSyncPos.lX, kSyncPos.lY);
		m_rokNetActorMgr->SyncActor(kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);

		/*
		CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
		CInstanceBase * pkChrInst = rkChrMgr.GetInstancePtr(kSyncPos.dwVID);

		if (pkChrInst)
		{			
			pkChrInst->NEW_SyncPixelPosition(kSyncPos.lX, kSyncPos.lY);		
		}
		*/
}

	return true;
}


/*
bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;

	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.lX, chrAddPacket.lY);

	SNetworkActorData kNetActorData;
	kNetActorData.m_dwGuildID=chrAddPacket.dwGuild;
	kNetActorData.m_dwEmpireID=chrAddPacket.bEmpire;
	kNetActorData.m_dwMovSpd=chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;
	kNetActorData.m_dwShape=chrAddPacket.parts[CRaceData::PART_MAIN];
	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_dwWeapon=chrAddPacket.parts[CRaceData::PART_WEAPON];
	kNetActorData.m_fRot=chrAddPacket.angle;
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	kNetActorData.SetPosition(chrAddPacket.lX, chrAddPacket.lY);
	kNetActorData.m_stName=chrAddPacket.name;
	__RecvCharacterAppendPacket(&kNetActorData);
	return true;
}
*/

