#ifndef __INC_METIN_II_LENGTH_H__
#define __INC_METIN_II_LENGTH_H__

#define WORD_MAX 0xffff

#include "service.h"
#include <cstdint>

typedef long long GoldType;
typedef int64_t   TimeT64;   // absolute Unix epoch seconds; canonical 64-bit timestamp type (wire/DB/server)

enum EMisc
{
	// TODO: private code length is 7, refactor this later on
	PRIVATE_CODE_LENGTH		= 8,
	QUEST_INPUT_STRING_MAX_NUM = 64,

	ITEM_SOCKET_SLOT_MAX_NUM = 3,
	MAX_HOST_LENGTH			= 15,
	IP_ADDRESS_LENGTH		= 15,
	LOGIN_MAX_LEN			= 30,
	PASSWD_MAX_LEN			= 16,
	PLAYER_PER_ACCOUNT		= 4,
	ACCOUNT_STATUS_MAX_LEN	= 8,
	CHARACTER_NAME_MAX_LEN	= 24,
	SHOP_SIGN_MAX_LEN		= 32,

	INVENTORY_SLOT_WIDTH			= 5,
	INVENTORY_SLOT_HEIGHT			= 9,
	INVENTORY_PAGE_COUNT			= 5,
	INVENTORY_SLOT_PER_PAGE			= INVENTORY_SLOT_WIDTH * INVENTORY_SLOT_HEIGHT,
	INVENTORY_MAX_NUM				= INVENTORY_PAGE_COUNT * INVENTORY_SLOT_PER_PAGE,

	ABILITY_MAX_NUM			= 50,
	EMPIRE_MAX_NUM			= 4,
	BANWORD_MAX_LEN			= 24,
	SOCIAL_ID_MAX_LEN		= 18,

	GUILD_NAME_MAX_LEN		= 12,

	SHOP_HOST_ITEM_MAX_NUM	= 40,	/* maximum number of items for the host */
	SHOP_GUEST_ITEM_MAX_NUM = 18,	/* maximum number of items for the guest */

	SHOP_PRICELIST_MAX_NUM	= 40,	///< maximum number of price entries kept in a private shop price list

	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 10,
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,


	CHAT_MAX_LEN			= 512,

	QUICKSLOT_MAX_NUM		= 36,

	JOURNAL_MAX_NUM			= 2,

	QUERY_MAX_LEN			= 8192,

	FILE_MAX_LEN			= 128,

	PLAYER_EXP_TABLE_MAX	= 120,
	PLAYER_MAX_LEVEL_CONST	= 120,

	GUILD_MAX_LEVEL			= 20,
	MOB_MAX_LEVEL			= 100,

	ATTRIBUTE_MAX_VALUE		= 20,
	CHARACTER_PATH_MAX_NUM	= 64,
	SKILL_MAX_NUM			= 255,
	SKILLBOOK_DELAY_MIN		= 64800,
	SKILLBOOK_DELAY_MAX		= 108000, 
	SKILL_MAX_LEVEL			= 40,

	APPLY_NAME_MAX_LEN		= 32,
	EVENT_FLAG_NAME_MAX_LEN = 32,

	MOB_SKILL_MAX_NUM		= 5,

    POINT_MAX_NUM = 255,
	DRAGON_SOUL_BOX_SIZE = 32,
	DRAGON_SOUL_BOX_COLUMN_NUM = 8,
	DRAGON_SOUL_BOX_ROW_NUM = DRAGON_SOUL_BOX_SIZE / DRAGON_SOUL_BOX_COLUMN_NUM,
	DRAGON_SOUL_REFINE_GRID_SIZE = 15,
	MAX_AMOUNT_OF_MALL_BONUS	= 20,

	WEAR_MAX_NUM				= 32,

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

	BELT_INVENTORY_SLOT_WIDTH = 4,
	BELT_INVENTORY_SLOT_HEIGHT= 4,

	BELT_INVENTORY_SLOT_COUNT = BELT_INVENTORY_SLOT_WIDTH * BELT_INVENTORY_SLOT_HEIGHT,

	MAX_GROUP_ITEM_NUM = 256,
};

// LIMIT_GOLD
constexpr GoldType GOLD_MAX = 100000000000000000LL;
// END_LIMIT_GOLD

enum SwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	//1 = Switching Item
	//2 = Yang
	SWITCHBOT_PRICE_TYPE = 1,
	//Amount Swtichting Items the Yang-Price
	SWITCHBOT_PRICE_AMOUNT = 1,
};

const DWORD c_arSwitchingItems[3] =
{
	39028,
	71084,
	76014,
};

enum EWearPositions
{
	WEAR_BODY,		// 0
	WEAR_HEAD,		// 1
	WEAR_FOOTS,		// 2
	WEAR_WRIST,		// 3
	WEAR_WEAPON,	// 4
	WEAR_NECK,		// 5
	WEAR_EAR,		// 6
	WEAR_UNIQUE1,	// 7
	WEAR_UNIQUE2,	// 8
	WEAR_ARROW,		// 9
	WEAR_SHIELD,	// 10
    WEAR_ABILITY1,  // 11
    WEAR_ABILITY2,  // 12
    WEAR_ABILITY3,  // 13
    WEAR_ABILITY4,  // 14
    WEAR_ABILITY5,  // 15
    WEAR_ABILITY6,  // 16
    WEAR_ABILITY7,  // 17
    WEAR_ABILITY8,  // 18
	WEAR_COSTUME_BODY,	// 19
	WEAR_COSTUME_HAIR,	// 20
	
	WEAR_RING1,			// 21	: new ring slot 1 (left)
	WEAR_RING2,			// 22	: new ring slot 2 (right)

	WEAR_BELT,			// 23	: new belt slot

	WEAR_MAX = 32	// 
};

enum EDragonSoulDeckType
{
	DRAGON_SOUL_DECK_0,
	DRAGON_SOUL_DECK_1,
	DRAGON_SOUL_DECK_MAX_NUM = 2,

	DRAGON_SOUL_DECK_RESERVED_MAX_NUM = 3,	// NOTE: Important! Not in use yet, but 3 pages are reserved. When adding DS decks, subtract the same number from RESERVED!
};

enum ESex
{
	SEX_MALE,
	SEX_FEMALE
};

enum EDirection
{
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_MAX_NUM
};

#define ABILITY_MAX_LEVEL	10  /* maximum skill level */

enum EAbilityDifficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERY_HARD,
	DIFFICULTY_NUM_TYPES
};

enum EAbilityCategory
{
	CATEGORY_PHYSICAL,	/* physical ability */
	CATEGORY_MENTAL,	/* mental ability */
	CATEGORY_ATTRIBUTE,	/* attribute ability */
	CATEGORY_NUM_TYPES
};

enum EJobs
{
	JOB_WARRIOR,
	JOB_ASSASSIN,
	JOB_SURA,
	JOB_SHAMAN,
	JOB_MAX_NUM
};

enum ESkillGroups
{
	SKILL_GROUP_MAX_NUM = 2,
};

enum ERaceFlags
{
	RACE_FLAG_ANIMAL	= (1 << 0),
	RACE_FLAG_UNDEAD	= (1 << 1),
	RACE_FLAG_DEVIL		= (1 << 2),
	RACE_FLAG_HUMAN		= (1 << 3),
	RACE_FLAG_ORC		= (1 << 4),
	RACE_FLAG_MILGYO	= (1 << 5),
	RACE_FLAG_INSECT	= (1 << 6),
	RACE_FLAG_FIRE		= (1 << 7),
	RACE_FLAG_ICE		= (1 << 8),
	RACE_FLAG_DESERT	= (1 << 9),
	RACE_FLAG_TREE		= (1 << 10),
	RACE_FLAG_ATT_ELEC	= (1 << 11),
	RACE_FLAG_ATT_FIRE	= (1 << 12),
	RACE_FLAG_ATT_ICE	= (1 << 13),
	RACE_FLAG_ATT_WIND	= (1 << 14),
	RACE_FLAG_ATT_EARTH	= (1 << 15),
	RACE_FLAG_ATT_DARK	= (1 << 16),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

enum
{
	QUEST_SEND_ISBEGIN		= (1 << 0),
	QUEST_SEND_TITLE		= (1 << 1),  // 30 characters max
	QUEST_SEND_CLOCK_NAME		= (1 << 2),  // 16 characters max
	QUEST_SEND_CLOCK_VALUE		= (1 << 3),
	QUEST_SEND_COUNTER_NAME		= (1 << 4),  // 16 characters max
	QUEST_SEND_COUNTER_VALUE	= (1 << 5),
	QUEST_SEND_ICON_FILE		= (1 << 6),  // 24 characters max
};

enum
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE,
};

enum
{
	GUILD_AUTH_ADD_MEMBER	= (1 << 0),
	GUILD_AUTH_REMOVE_MEMBER	= (1 << 1),
	GUILD_AUTH_NOTICE		= (1 << 2),
	GUILD_AUTH_USE_SKILL	= (1 << 3),
};

enum
{
	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_GRADE_COUNT = 15,
	GUILD_COMMENT_MAX_COUNT = 12,
	GUILD_COMMENT_MAX_LEN = 50,
	GUILD_LEADER_GRADE = 1,
	GUILD_BASE_POWER = 400,
	GUILD_POWER_PER_SKILL_LEVEL = 200,
	GUILD_POWER_PER_LEVEL = 100,
	GUILD_MINIMUM_LEADERSHIP = 40,
	GUILD_WAR_MIN_MEMBER_COUNT = 8,
	GUILD_LADDER_POINT_PER_LEVEL = 1000,
	GUILD_CREATE_ITEM_VNUM = 70101,
};

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
};

enum ELoads
{
	LOAD_NONE,
	LOAD_LIGHT,
	LOAD_NORMAL,
	LOAD_HEAVY,
	LOAD_MASSIVE
};

enum
{
	QUICKSLOT_TYPE_NONE,
	QUICKSLOT_TYPE_ITEM,
	QUICKSLOT_TYPE_SKILL,
	QUICKSLOT_TYPE_COMMAND,
	QUICKSLOT_TYPE_MAX_NUM,
};

enum EParts
{
	PART_MAIN,
	PART_WEAPON,
	PART_HEAD,
	PART_HAIR,

	PART_MAX_NUM,
	PART_WEAPON_SUB,
};

enum EChatType
{
	CHAT_TYPE_TALKING,	/* normal chat */
	CHAT_TYPE_INFO,	/* info (picked up an item, gained experience, etc.) */
	CHAT_TYPE_NOTICE,	/* notice */
	CHAT_TYPE_PARTY,	/* party chat */
	CHAT_TYPE_GUILD,	/* guild chat */
	CHAT_TYPE_COMMAND,	/* normal command */
	CHAT_TYPE_SHOUT,	/* shout */
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MAX_NUM
};

enum EWhisperType
{
	WHISPER_TYPE_NORMAL		= 0,
	WHISPER_TYPE_NOT_EXIST		= 1,
	WHISPER_TYPE_TARGET_BLOCKED	= 2,
	WHISPER_TYPE_SENDER_BLOCKED	= 3,
	WHISPER_TYPE_ERROR		= 4,
	WHISPER_TYPE_GM			= 5,
	WHISPER_TYPE_SYSTEM		= 0xFF
};

enum ECharacterPosition
{
	POSITION_GENERAL,
	POSITION_BATTLE,
	POSITION_DYING,
	POSITION_SITTING_CHAIR,
	POSITION_SITTING_GROUND,
	POSITION_INTRO,
	POSITION_MAX_NUM
};

enum EGMLevels
{
	GM_PLAYER,
	GM_LOW_WIZARD,
	GM_WIZARD,
	GM_HIGH_WIZARD,
	GM_GOD,
	GM_IMPLEMENTOR,
	GM_MAX_NUM
};

enum EMobRank
{
	MOB_RANK_PAWN,
	MOB_RANK_S_PAWN,
	MOB_RANK_KNIGHT,
	MOB_RANK_S_KNIGHT,
	MOB_RANK_BOSS,
	MOB_RANK_KING,
	MOB_RANK_MAX_NUM
};

enum ECharType
{
	CHAR_TYPE_MONSTER,
	CHAR_TYPE_NPC,
	CHAR_TYPE_STONE,
	CHAR_TYPE_WARP,
	CHAR_TYPE_DOOR,
	CHAR_TYPE_BUILDING,
	CHAR_TYPE_PC,
	CHAR_TYPE_POLYMORPH_PC,
	CHAR_TYPE_HORSE,
	CHAR_TYPE_GOTO
};

enum EBattleType
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGE,
	BATTLE_TYPE_MAGIC,
	BATTLE_TYPE_SPECIAL,
	BATTLE_TYPE_POWER,
	BATTLE_TYPE_TANKER,
	BATTLE_TYPE_SUPER_POWER,
	BATTLE_TYPE_SUPER_TANKER,
	BATTLE_TYPE_MAX_NUM
};

enum EApplyTypes
{
	APPLY_NONE,			// 0
	APPLY_MAX_HP,		// 1
	APPLY_MAX_SP,		// 2
	APPLY_CON,			// 3
	APPLY_INT,			// 4
	APPLY_STR,			// 5
	APPLY_DEX,			// 6
	APPLY_ATT_SPEED,	// 7
	APPLY_MOV_SPEED,	// 8
	APPLY_CAST_SPEED,	// 9
	APPLY_HP_REGEN,		// 10
	APPLY_SP_REGEN,		// 11
	APPLY_POISON_PCT,	// 12
	APPLY_STUN_PCT,		// 13
	APPLY_SLOW_PCT,		// 14
	APPLY_CRITICAL_PCT,		// 15
	APPLY_PENETRATE_PCT,	// 16
	APPLY_ATTBONUS_HUMAN,	// 17
	APPLY_ATTBONUS_ANIMAL,	// 18
	APPLY_ATTBONUS_ORC,		// 19
	APPLY_ATTBONUS_MILGYO,	// 20
	APPLY_ATTBONUS_UNDEAD,	// 21
	APPLY_ATTBONUS_DEVIL,	// 22
	APPLY_STEAL_HP,			// 23
	APPLY_STEAL_SP,			// 24
	APPLY_MANA_BURN_PCT,	// 25
	APPLY_DAMAGE_SP_RECOVER,	// 26
	APPLY_BLOCK,			// 27
	APPLY_DODGE,			// 28
	APPLY_RESIST_SWORD,		// 29
	APPLY_RESIST_TWOHAND,	// 30
	APPLY_RESIST_DAGGER,	// 31
	APPLY_RESIST_BELL,		// 32
	APPLY_RESIST_FAN,		// 33
	APPLY_RESIST_BOW,		// 34
	APPLY_RESIST_FIRE,		// 35
	APPLY_RESIST_ELEC,		// 36
	APPLY_RESIST_MAGIC,		// 37
	APPLY_RESIST_WIND,		// 38
	APPLY_REFLECT_MELEE,	// 39
	APPLY_REFLECT_CURSE,	// 40
	APPLY_POISON_REDUCE,	// 41
	APPLY_KILL_SP_RECOVER,	// 42
	APPLY_EXP_DOUBLE_BONUS,	// 43
	APPLY_GOLD_DOUBLE_BONUS,	// 44
	APPLY_ITEM_DROP_BONUS,	// 45
	APPLY_POTION_BONUS,		// 46
	APPLY_KILL_HP_RECOVER,	// 47
	APPLY_IMMUNE_STUN,		// 48
	APPLY_IMMUNE_SLOW,		// 49
	APPLY_IMMUNE_FALL,		// 50
	APPLY_SKILL,			// 51
	APPLY_BOW_DISTANCE,		// 52
	APPLY_ATT_GRADE_BONUS,	// 53
	APPLY_DEF_GRADE_BONUS,	// 54
	APPLY_MAGIC_ATT_GRADE,	// 55
	APPLY_MAGIC_DEF_GRADE,	// 56
	APPLY_CURSE_PCT,		// 57
	APPLY_MAX_STAMINA,		// 58
	APPLY_ATTBONUS_WARRIOR,	// 59
	APPLY_ATTBONUS_ASSASSIN,	// 60
	APPLY_ATTBONUS_SURA,	// 61
	APPLY_ATTBONUS_SHAMAN,	// 62
	APPLY_ATTBONUS_MONSTER,	// 63
	APPLY_MALL_ATTBONUS,			// 64 attack +x%
	APPLY_MALL_DEFBONUS,			// 65 defence +x%
	APPLY_MALL_EXPBONUS,			// 66 experience +x%
	APPLY_MALL_ITEMBONUS,			// 67 item drop rate x/10 times
	APPLY_MALL_GOLDBONUS,			// 68 gold drop rate x/10 times
	APPLY_MAX_HP_PCT,				// 69 max HP +x%
	APPLY_MAX_SP_PCT,				// 70 max SP +x%
	APPLY_SKILL_DAMAGE_BONUS,		// 71 skill damage * (100+x)%
	APPLY_NORMAL_HIT_DAMAGE_BONUS,	// 72 normal hit damage * (100+x)%
	APPLY_SKILL_DEFEND_BONUS,		// 73 skill damage defence * (100-x)%
	APPLY_NORMAL_HIT_DEFEND_BONUS,	// 74 normal hit damage defence * (100-x)%

	APPLY_EXTRACT_HP_PCT,			// 77 HP consumed on use

	APPLY_RESIST_WARRIOR,			// 78 resistance against warriors
	APPLY_RESIST_ASSASSIN,			// 79 resistance against assassins
	APPLY_RESIST_SURA,				// 80 resistance against suras
	APPLY_RESIST_SHAMAN,			// 81 resistance against shamans
	APPLY_ENERGY,					// 82 energy
	APPLY_DEF_GRADE,				// 83 defence. DEF_GRADE_BONUS has an intended bug(...) where the client shows it doubled.
	APPLY_COSTUME_ATTR_BONUS,		// 84 bonus on the attribute values of a costume item
	APPLY_MAGIC_ATTBONUS_PER,		// 85 magic attack +x%
	APPLY_MELEE_MAGIC_ATTBONUS_PER,			// 86 magic + melee attack +x%
	
	APPLY_RESIST_ICE,		// 87 ice resistance
	APPLY_RESIST_EARTH,		// 88 earth resistance
	APPLY_RESIST_DARK,		// 89 dark resistance

	APPLY_ANTI_CRITICAL_PCT,	//90 critical hit resistance
	APPLY_ANTI_PENETRATE_PCT,	//91 piercing hit resistance


	MAX_APPLY_NUM,              // 
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9  stamina
	POINT_MAX_STAMINA,          // 10 max stamina

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12 strength
	POINT_HT,                   // 13 vitality
	POINT_DX,                   // 14 dexterity
	POINT_IQ,                   // 15 intelligence
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17 attack speed
	POINT_ATT_GRADE,		// 18 attack power MAX
	POINT_MOV_SPEED,            // 19 movement speed
	POINT_CLIENT_DEF_GRADE,	// 20 defence grade
	POINT_CASTING_SPEED,        // 21 casting speed: (cooldown time * 100) / (100 + this value) = final cooldown time
	POINT_MAGIC_ATT_GRADE,      // 22 magic attack power
	POINT_MAGIC_DEF_GRADE,      // 23 magic defence
	POINT_EMPIRE_POINT,         // 24 empire score
	POINT_LEVEL_STEP,           // 25 step within one level.. (reward at 1 2 3, level up at 4)
	POINT_STAT,                 // 26 number of stat points that can be spent
	POINT_SUB_SKILL,		// 27 sub skill points
	POINT_SKILL,		// 28 active skill points
	POINT_WEAPON_MIN,		// 29 weapon minimum damage
	POINT_WEAPON_MAX,		// 30 weapon maximum damage
	POINT_PLAYTIME,             // 31 play time
	POINT_HP_REGEN,             // 32 HP regeneration rate
	POINT_SP_REGEN,             // 33 SP regeneration rate

	POINT_BOW_DISTANCE,         // 34 bow range increase (meter)

	POINT_HP_RECOVERY,          // 35 HP recovery increase
	POINT_SP_RECOVERY,          // 36 SP recovery increase

	POINT_POISON_PCT,           // 37 poison chance
	POINT_STUN_PCT,             // 38 stun chance
	POINT_SLOW_PCT,             // 39 slow chance
	POINT_CRITICAL_PCT,         // 40 critical hit chance
	POINT_PENETRATE_PCT,        // 41 piercing hit chance
	POINT_CURSE_PCT,            // 42 curse chance

	POINT_ATTBONUS_HUMAN,       // 43 strong against humans
	POINT_ATTBONUS_ANIMAL,      // 44 damage % increase against animals
	POINT_ATTBONUS_ORC,         // 45 damage % increase against orcs
	POINT_ATTBONUS_MILGYO,      // 46 damage % increase against mystics
	POINT_ATTBONUS_UNDEAD,      // 47 damage % increase against undead
	POINT_ATTBONUS_DEVIL,       // 48 damage % increase against devils
	POINT_ATTBONUS_INSECT,      // 49 insects
	POINT_ATTBONUS_FIRE,        // 50 fire race
	POINT_ATTBONUS_ICE,         // 51 ice race
	POINT_ATTBONUS_DESERT,      // 52 desert race
	POINT_ATTBONUS_MONSTER,     // 53 strong against all monsters
	POINT_ATTBONUS_WARRIOR,     // 54 strong against warriors
	POINT_ATTBONUS_ASSASSIN,	// 55 strong against assassins
	POINT_ATTBONUS_SURA,		// 56 strong against suras
	POINT_ATTBONUS_SHAMAN,		// 57 strong against shamans
	POINT_ATTBONUS_TREE,     	// 58 strong against trees 20050729.myevan UNUSED5

	POINT_RESIST_WARRIOR,		// 59 resistance against warriors
	POINT_RESIST_ASSASSIN,		// 60 resistance against assassins
	POINT_RESIST_SURA,			// 61 resistance against suras
	POINT_RESIST_SHAMAN,		// 62 resistance against shamans

	POINT_STEAL_HP,             // 63 HP absorption
	POINT_STEAL_SP,             // 64 SP absorption

	POINT_MANA_BURN_PCT,        // 65 mana burn

	/// bonuses when taking damage ///

	POINT_DAMAGE_SP_RECOVER,    // 66 chance to recover SP when attacked

	POINT_BLOCK,                // 67 block rate
	POINT_DODGE,                // 68 dodge rate

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74  arrow resistance: damage reduction
	POINT_RESIST_FIRE,          // 75  fire resistance: reduces damage from fire attacks
	POINT_RESIST_ELEC,          // 76  electric resistance: reduces damage from electric attacks
	POINT_RESIST_MAGIC,         // 77  magic resistance: reduces damage from all magic
	POINT_RESIST_WIND,          // 78  wind resistance: reduces damage from wind attacks

	POINT_REFLECT_MELEE,        // 79 melee attack reflection

	/// on special damage ///
	POINT_REFLECT_CURSE,		// 80 curse reflection
	POINT_POISON_REDUCE,		// 81 poison damage reduction

	/// when an enemy dies ///
	POINT_KILL_SP_RECOVER,		// 82 MP recovery when an enemy dies
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,		// 85

	/// recovery related ///
	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102 strength of the mana shield effect from the Dark Protection skill

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107
	POINT_POLYMORPH,			// 108 vnum of the monster polymorphed into
	POINT_MOUNT,			// 109 vnum of the monster being ridden

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112 stat reset points from using the Blood Pill (1 point reset per 1)

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114 attack +x%
	POINT_MALL_DEFBONUS,		// 115 defence +x%
	POINT_MALL_EXPBONUS,		// 116 experience +x%
	POINT_MALL_ITEMBONUS,		// 117 item drop rate x/10 times
	POINT_MALL_GOLDBONUS,		// 118 gold drop rate x/10 times

	POINT_MAX_HP_PCT,			// 119 max HP +x%
	POINT_MAX_SP_PCT,			// 120 max SP +x%

	POINT_SKILL_DAMAGE_BONUS,		// 121 skill damage *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122 normal hit damage *(100+x)%

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123 skill defence damage
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124 normal hit defence damage
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	POINT_RAMADAN_CANDY_BONUS_EXP,			// for the Ramadan candy experience bonus

	POINT_ENERGY = 128,					// 128 energy

	// for the energy UI.
	// Not used by the server, but the client tracks the energy end time as a POINT, hence this.
	// How embarrassing
	POINT_ENERGY_END_TIME = 129,					// 129 energy end time

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	// additional attribute resistances
	POINT_RESIST_ICE = 133,          //   ice resistance: damage reduction
	POINT_RESIST_EARTH = 134,        //   earth resistance: damage reduction
	POINT_RESIST_DARK = 135,         //   dark resistance: damage reduction

	POINT_RESIST_CRITICAL = 136,		// critical hit resistance: reduces the opponent's critical hit chance
	POINT_RESIST_PENETRATE = 137,		// piercing hit resistance: reduces the opponent's piercing hit chance

	// TODO: This exists only in the client, remove?
	POINT_MIN_WEP = 200,
	POINT_MAX_WEP,
	POINT_MIN_MAGIC_WEP,
	POINT_MAX_MAGIC_WEP,
	POINT_HIT_RATE,
};

enum EOnClickEvents
{
	ON_CLICK_NONE,
	ON_CLICK_SHOP,
	ON_CLICK_TALK,
	ON_CLICK_MAX_NUM
};

enum EOnIdleEvents
{
	ON_IDLE_NONE,
	ON_IDLE_GENERAL,
	ON_IDLE_MAX_NUM
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
	SWITCHBOT,
	GROUND,					// Only used by client
	WINDOW_TYPE_MAX,
};

enum EMobSizes
{
	MOBSIZE_RESERVED,
	MOBSIZE_SMALL,
	MOBSIZE_MEDIUM,
	MOBSIZE_BIG
};

enum EAIFlags
{
	AIFLAG_AGGRESSIVE	= (1 << 0),
	AIFLAG_NOMOVE	= (1 << 1),
	AIFLAG_COWARD	= (1 << 2),
	AIFLAG_NOATTACKSHINSU	= (1 << 3),
	AIFLAG_NOATTACKJINNO	= (1 << 4),
	AIFLAG_NOATTACKCHUNJO	= (1 << 5),
	AIFLAG_ATTACKMOB = (1 << 6 ),
	AIFLAG_BERSERK	= (1 << 7),
	AIFLAG_STONESKIN	= (1 << 8),
	AIFLAG_GODSPEED	= (1 << 9),
	AIFLAG_DEATHBLOW	= (1 << 10),
	AIFLAG_REVIVE		= (1 << 11),
};

enum EMobStatType
{
	MOB_STATTYPE_POWER,
	MOB_STATTYPE_TANKER,
	MOB_STATTYPE_SUPER_POWER,
	MOB_STATTYPE_SUPER_TANKER,
	MOB_STATTYPE_RANGE,
	MOB_STATTYPE_MAGIC,
	MOB_STATTYPE_MAX_NUM
};

enum EImmuneFlags
{
	IMMUNE_STUN		= (1 << 0),
	IMMUNE_SLOW		= (1 << 1),
	IMMUNE_FALL		= (1 << 2),
	IMMUNE_CURSE	= (1 << 3),
	IMMUNE_POISON	= (1 << 4),
	IMMUNE_TERROR	= (1 << 5),
	IMMUNE_REFLECT	= (1 << 6),
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
	MOB_RESISTS_MAX_NUM
};

enum
{
	SKILL_ATTR_TYPE_NORMAL = 1,
	SKILL_ATTR_TYPE_MELEE,
	SKILL_ATTR_TYPE_RANGE,
	SKILL_ATTR_TYPE_MAGIC
		/*
		   SKILL_ATTR_TYPE_FIRE,
		   SKILL_ATTR_TYPE_ICE,
		   SKILL_ATTR_TYPE_ELEC,
		   SKILL_ATTR_TYPE_DARK,
		   */
};

enum
{
	SKILL_NORMAL,
	SKILL_MASTER,
	SKILL_GRAND_MASTER,
	SKILL_PERFECT_MASTER,
};

enum EPartyExpDistributionType
{
	PARTY_EXP_DISTRIBUTION_NON_PARITY,
	PARTY_EXP_DISTRIBUTION_PARITY,
	PARTY_EXP_DISTRIBUTION_MAX_NUM
};

enum EGuildWarType
{
	GUILD_WAR_TYPE_FIELD,
	GUILD_WAR_TYPE_BATTLE,
	GUILD_WAR_TYPE_FLAG,
	GUILD_WAR_TYPE_MAX_NUM
};

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,
	GUILD_WAR_OVER,
	GUILD_WAR_RESERVE,

	GUILD_WAR_DURATION = 30*60, // 1 hour
	GUILD_WAR_WIN_POINT = 1000,
	GUILD_WAR_LADDER_HALF_PENALTY_TIME = 12*60*60,
};

enum EAttributeSet 
{            
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY, 
	ATTRIBUTE_SET_WRIST, 
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
	ATTRIBUTE_SET_MAX_NUM
};  

enum EPrivType
{
	PRIV_NONE,
	PRIV_ITEM_DROP,
	PRIV_GOLD_DROP,
	PRIV_GOLD10_DROP,
	PRIV_EXP_PCT,
	MAX_PRIV_NUM,
};

enum EMoneyLogType
{
	MONEY_LOG_RESERVED,
	MONEY_LOG_MONSTER,
	MONEY_LOG_SHOP,
	MONEY_LOG_REFINE,
	MONEY_LOG_QUEST,
	MONEY_LOG_GUILD,
	MONEY_LOG_MISC,
	MONEY_LOG_MONSTER_KILL,
	MONEY_LOG_DROP,
	MONEY_LOG_TYPE_MAX_NUM,
};

enum EPremiumTypes
{
	PREMIUM_EXP,
	PREMIUM_ITEM,
	PREMIUM_SAFEBOX,
	PREMIUM_AUTOLOOT,
	PREMIUM_FISH_MIND,
	PREMIUM_MARRIAGE_FAST,
	PREMIUM_GOLD,
	PREMIUM_MAX_NUM = 9
};

enum SPECIAL_EFFECT
{
	SE_NONE,

	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY,
	SE_LEVELUP_UNDER_15_FOR_GERMANY,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,
} ;

#include "item_length.h"

enum EDragonSoulRefineWindowSize
{
	DRAGON_SOUL_REFINE_GRID_MAX = 15,
};

enum EMisc2
{
	DRAGON_SOUL_EQUIP_SLOT_START = INVENTORY_MAX_NUM + WEAR_MAX_NUM,
	DRAGON_SOUL_EQUIP_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_START + (DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM),
	DRAGON_SOUL_EQUIP_RESERVED_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_END + (DS_SLOT_MAX * DRAGON_SOUL_DECK_RESERVED_MAX_NUM),

	BELT_INVENTORY_SLOT_START = DRAGON_SOUL_EQUIP_RESERVED_SLOT_END,
	BELT_INVENTORY_SLOT_END = BELT_INVENTORY_SLOT_START + BELT_INVENTORY_SLOT_COUNT,

	INVENTORY_AND_EQUIP_SLOT_MAX = BELT_INVENTORY_SLOT_END,
};

#pragma pack(push, 1)

typedef struct SItemPos
{
	BYTE window_type;
	ItemCellType cell;
    SItemPos ()
    {
        window_type = INVENTORY;
		cell = WORD_MAX;
    }

	SItemPos (BYTE _window_type, ItemCellType _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	bool IsValidItemPosition() const
	{
		switch (window_type)
		{
		case RESERVED_WINDOW:
			return false;
		case INVENTORY:
		case EQUIPMENT:
		case BELT_INVENTORY:
			return cell < INVENTORY_AND_EQUIP_SLOT_MAX;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);
		case SAFEBOX:
		case MALL:
			return false;
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
		default:
			return false;
		}
		return false;
	}

	bool IsSamePosition(const SItemPos& other) const
	{
		return *this == other
			|| ((INVENTORY == window_type || EQUIPMENT == window_type)
				&& (INVENTORY == other.window_type || EQUIPMENT == other.window_type)
				&& cell == other.cell);
	}
	
	bool IsEquipPosition() const
	{
		return ((INVENTORY == window_type || EQUIPMENT == window_type) && cell >= INVENTORY_MAX_NUM && cell < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
			|| IsDragonSoulEquipPosition();
	}

	bool IsDragonSoulEquipPosition() const
	{
		return (DRAGON_SOUL_EQUIP_SLOT_START <= cell) && (DRAGON_SOUL_EQUIP_SLOT_END > cell);
	}

	bool IsBeltInventoryPosition() const
	{
		return (BELT_INVENTORY_SLOT_START <= cell) && (BELT_INVENTORY_SLOT_END > cell);
	}

	bool IsDefaultInventoryPosition() const
	{
		return INVENTORY == window_type && cell < INVENTORY_MAX_NUM;
	}

	bool IsSwitchbotPosition() const
    {
    	return SWITCHBOT == window_type && cell < SWITCHBOT_SLOT_COUNT;
    }

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}
	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

const TItemPos NPOS (RESERVED_WINDOW, WORD_MAX);

typedef enum
{
	SHOP_COIN_TYPE_GOLD,
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#pragma pack(pop)

#endif
