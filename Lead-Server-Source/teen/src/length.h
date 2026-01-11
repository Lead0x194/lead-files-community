#ifndef __INC_METIN_LENGTH_H__
#define __INC_METIN_LENGTH_H__

#define MAX_SQL_LOGIN_LENGTH		8
#define MAX_SQL_PWD_LENGTH         	12
#define MAX_SQL_REAL_NAME_LENGTH        20
#define MAX_SQL_SOCIAL_ID_LENGTH        18
#define MAX_SQL_EMAIL_LENGTH            40
#define MAX_SQL_PHONE_LENGTH            14
#define MAX_SQL_ADDRESS_LENGTH          80
#define MAX_SQL_GAME_ID_LENGTH          20

#define MAX_SQL_IP_ADDR_LENGTH          15
#define MAX_SQL_SERIAL_LENGTH           25
#define MAX_SQL_BANK_PWD_LENGTH         8

#define MAX_NAME_LENGTH			16
#define MAX_ITEM_NAME_LENGTH            32
#define MAX_SOCIAL_LENGTH		16
#define MAX_TITLE_LENGTH		16
#define MAX_PWD_LENGTH			16

#define MAX_GAME_STORES			2

#define HOST_LENGTH			30
#define MAX_HOST_LENGTH			HOST_LENGTH

enum EDescTypes
{
	CLIENT_TYPE_GAME,
	CLIENT_TYPE_ACCOUNT,
	CLIENT_TYPE_UTILS
};

#define HANDSHAKE_XOR			0x6AB3D224

#define MAX_DRAWLIST                    256

#define MAX_ITEMS_ON_CHARACTER          80      // 소지할 수 있는 아이템 갯수
#define MAX_EXCHANGE_ITEMS_ON_CHARACTER 4       // 교환할 수 있는 아이템 갯수
#define MAX_QITEMS_ON_CHARACTER         10      // 퀵창에 가질 수 있는 아이템 갯수
#define MAX_ITEMS_ON_BANK               80      // 은행에 가질 수 있는 아이템 갯수

#define MAX_AFFECTS                     116     // 가질 수 있는 효과

#define MAX_QUESTS                      256     // 최대 퀘스트 갯수
#define MAX_QUEST_QUESTIONS             16      // 최대 퀘스트 질문수
#define MAX_GROUP_MEMBERS               16      // 최대 그룹 멤버 수

#define MAX_SKILL_PER_TYPE              240
#define MAX_QSKILLS                     10

#define SKILL_TYPE_SKILL                0       // 기술
#define SKILL_TYPE_MAGIC                1       // 마법
#define SKILL_TYPE_KNOWLEDGE            2       // 지식
#define SKILL_TYPE_SPECIAL              2       // 기타
#define MAX_SKILL_TYPES                 3       // 기술 종류

#define GUILD_TYPE_COMBAT               0       // 전투 목적 길드
#define GUILD_TYPE_ETC                  1       // 기타 (종교) 목적 길드
#define GUILD_TYPE_ECONOMY              2       // 영리 목적 길드
#define MAX_GUILD_TYPES                 3       // 길드 종류

#define MAX_ATTACK_LIST                 32      // 공격 리스트

#define MAX_VOICE_MEMBERS               4

//
// For Characters
//
/* 성별 */
enum ESex
{
	SEX_NEUTRAL,
	SEX_FEMALE,
	SEX_MALE,
	NUM_SEX
};

/* 종족 */
#define RACE_SENT			 0 // 센트 종족
#define RACE_ELF			 1 // 엘프 종족
#define RACE_DWARF			 2 // 드워프
#define NUM_RACES			 3 // 종족 갯수

/* 입을 수 있는 부위 */
#define WEAR_HEAD			 1  // 머리
#define WEAR_NECK			 2  // 목
#define WEAR_BODY			 3  // 몸
#define WEAR_HAND			 4  // 양손 (장갑)
#define WEAR_FEET			 5  // 양발
#define WEAR_R_FINGER			 6  // 오른쪽 손가락
#define WEAR_L_FINGER			 7  // 왼쪽 손가락
#define WEAR_WIELD			 8  // 무기
#define WEAR_HOLD			 9  // 잡기
#define NUM_WEARS			10 // 입을 부위의 총 갯수

//
// For items
//
#define MAX_ITEM_AFFECT                  4       // 0 1 2 3
#define MAX_ITEM_VALUES                  5       // 0 1 2 3 4
#define MAX_ITEM_RESOURCES               4
#define MAX_ITEM_IMPROVE		24	// 나기라의 눈물로 개량할 수 있는 한계


// by mhh
#define	MAX_COMBINE_COUNT		200


#endif


