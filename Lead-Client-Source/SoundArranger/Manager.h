#pragma once

#include <EterGrnLib/Thing.h>
#include <GameLib/ItemData.h>
#include <GameLib/RaceData.h>
#include <MilesLib/Type.h>
#include <EffectLib/EffectInstance.h>
#include "RaceDataAccessor.h"
#include "RaceManagerAccessor.h"
#include "ActorInstanceAccessor.h"

class CManager
{
	public:
		enum
		{
			SOUND_STRING_LENGTH = 64,
			FILE_MAX_LEN = 128,

			MODE_EDIT = 0,
			MODE_VIEW = 1,
		};

		enum EModelType
		{
			MODEL_TYPE_NONE,
			MODEL_TYPE_PLAYER,
			MODEL_TYPE_NPC,
			MODEL_TYPE_ENEMY,
			MODEL_TYPE_EFFECT,
		};

		typedef std::map<DWORD, std::string> TSoundFileNameMap;
		typedef std::vector<CGraphicThing*> TGraphicThingVector;
		typedef std::map<std::string, std::string> TMotionPathMap;
		typedef std::vector<DWORD> TBasisMotionIDVector;
		typedef std::deque<DWORD> TReserveMotionDeque;

		typedef struct SEquipmentData
		{
			int iPartIndex;
			std::string strAttachingBoneName;
			std::string strItemScriptFileName;
			CItemData * pItemData;
		} TEquipmentData;
		typedef std::list<TEquipmentData> TEquipmentDataList;

		typedef struct SMotionData
		{
			// Static Data From Script
			std::string strName;
			std::string strFullFileName;

			TEquipmentDataList EquipmentDataList;

			// Dynamic Data Will Save To Data File
			NSound::TSoundDataVector SoundDataVector;
		} TMotionData;
		typedef std::map<DWORD, TMotionData> TMotionDataMap;

		typedef struct SCharacterData
		{
			// Static Data From Script
			std::string strPathName;

			EModelType eModelType;
			std::string strName;
			std::string strModelScriptFileName;

			/////

			// For Character & Monster Model
			TEquipmentDataList EquipmentDataList;
			TMotionDataMap MotionDataMap;
			TBasisMotionIDVector BasisMotionIDVector;

			// For Effect
			NSound::TSoundDataVector SoundDataVector;
		} TCharacterData;
		typedef std::map<DWORD, TCharacterData> TCharacterDataMap;

		class IChangedDataObserver
		{
			public:
				virtual void OnChangedModel(){}
				virtual void OnChangedMotion(){}
				virtual void OnChangedSound(){}
				virtual void OnChangedReserveMotion(){}
		};

		typedef std::list<IChangedDataObserver *>	TChangedDataObserverList;

		typedef struct STimeNode
		{
			float fStartTime;
			float fLastTime;
		} TTimeNode;
		typedef std::list<TTimeNode> TTimeNodeList;

		// Effect
		typedef std::list<CEffectInstance *>		TEffectInstanceList;

		// Item
		typedef std::map<DWORD, CItemData*>			TItemDataMap;

	public:
		CManager(void);
		~CManager(void);

		static void SetLocalTime(float fTime);
		static void SetLimitTime(float fTime);

		static DWORD GetCurrentModelIndex();
		static DWORD GetCurrentMotionIndex();
		static DWORD GetCurrentSoundIndex();
		static BOOL isModel();
		static BOOL isMotion();
		static BOOL isSound();
		static BOOL isEffect();
		static float GetCurrentTime();
		static float GetLimitTime();
		static float GetLocalTime();

		static void SetModelIndex(DWORD dwIndex);
		static void SetMotionIndex(DWORD dwIndex);
		static void ReserveMotionIndex(DWORD dwIndex);
		static void SetSoundIndex(DWORD dwIndex);
		static void ClearSoundIndex();
 		static void InsertSoundData(DWORD dwModelIndex, DWORD dwMotionIndex, float fTime, const char * c_szFileName);
		static void InsertSoundData(NSound::TSoundDataVector * pSoundDataVector, const NSound::TSoundData & c_rSoundData);
		static void Edit();
		static void UnEdit();
		static bool isEdit();
		static bool canEdit();

		static void PlayMotion(BOOL bLoopFlag = FALSE);

		static void TimeProcess();

		static void Refresh();
		static void RefreshEquipment(const TEquipmentDataList & c_rEquipmentDataList);

		static void RegisterSoundFileName(DWORD dwID, const char * c_szFileName);
		static void ClearAllSoundFileName();

	protected:
		static BOOL GetCharacterDataPointer(DWORD dwIndex, TCharacterData ** ppCharacterData);
		static BOOL GetCharacterMotionDataPointer(DWORD dwModelIndex, DWORD dwMotionIndex, TMotionData ** ppMotionData);
		static BOOL GetCharacterSoundDataVector(DWORD dwModelIndex, DWORD dwMotionIndex, NSound::TSoundDataVector ** ppSoundDataVector);
		static BOOL GetSoundFileName(DWORD dwSoundIndex, const char ** c_pszFileName);
		static DWORD GetRandomMotionIndex(DWORD dwModelIndex);

		static void RegisterChangedDataObserver(IChangedDataObserver * pObserver);

		static void UpdateSound();

		static void BlendMotion(DWORD dwIndex, float fBlendTime = 0.1f);

		static void DestroyEffect();

	protected:
		static int ms_iMode;
		static int ms_icurType;
		static bool ms_isEdit;
		static bool ms_PlayFlag;
		static bool ms_LoopFlag;
		static DWORD ms_dwLocalFrame;
		static float ms_fLocalTime;
		static float ms_fLimitTime;
		static int ms_iFrameCount;
		static int ms_iLoopCount;
		static int ms_icurLoopCount;
		static float ms_fMotionStartTime;
		static float ms_fElapsedTime;
		static TTimeNodeList ms_TimeNodeList;

		static DWORD ms_dwModelIndex;
		static DWORD ms_dwShapeIndex;
		static DWORD ms_dwMotionIndex;
		static DWORD ms_dwSoundIndex;

		static CRaceDataAccessor ms_RaceData;
		static CRaceManagerAccessor ms_RaceManager;
		static CActorInstanceAccessor ms_ActorInstance;
		static CEffectData * ms_pEffectData;

		// Character
		static TCharacterDataMap ms_CharacterDataMap;
		static TChangedDataObserverList ms_ChangedDataObserverList;
		// Effect
		static CDynamicPool<CEffectInstance> ms_EffectInstancePool;
		static TEffectInstanceList ms_EffectInstanceList;
		// Sound
		static TSoundFileNameMap ms_SoundFileNameMap;
		static TReserveMotionDeque ms_ReserveMotionDeque;
		// Item
		static TItemDataMap ms_ItemDataMap;
};