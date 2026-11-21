#ifndef __INC_METIN2_ITEM_LENGTH_H__
#define __INC_METIN2_ITEM_LENGTH_H__

enum EItemMisc
{
	ITEM_NAME_MAX_LEN			= 24,
	ITEM_VALUES_MAX_NUM			= 6,
	ITEM_SMALL_DESCR_MAX_LEN	= 256,
	ITEM_LIMIT_MAX_NUM			= 2,
	ITEM_APPLY_MAX_NUM			= 3,
	ITEM_SOCKET_MAX_NUM			= 3,
	ITEM_ATTRIBUTE_MAX_NUM		= 7,
	MAX_NORM_ATTR_NUM			= 5,
	MAX_RARE_ATTR_NUM			= 2,
	ITEM_ATTRIBUTE_MAX_LEVEL	= 5,
	ITEM_AWARD_WHY_MAX_LEN		= 50,

	REFINE_MATERIAL_MAX_NUM		= 5,

	ITEM_ELK_VNUM				= 50026,
};

const BYTE ITEM_SOCKET_REMAIN_SEC = 0;
enum EItemValueIdice
{
	ITEM_VALUE_DRAGON_SOUL_POLL_OUT_BONUS_IDX = 0,
	ITEM_VALUE_CHARGING_AMOUNT_IDX = 0,
	ITEM_VALUE_SECONDARY_COIN_UNIT_IDX = 0,
};
enum EItemDragonSoulSockets
{
	ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX = 2,
	ITEM_SOCKET_CHARGING_AMOUNT_IDX = 2,
};
// Seriously, is this for real?
// What is supposed to happen when the sockets get extended later? -_-;;;
enum EItemUniqueSockets
{
	ITEM_SOCKET_UNIQUE_SAVE_TIME = ITEM_SOCKET_MAX_NUM - 2,
	ITEM_SOCKET_UNIQUE_REMAIN_TIME = ITEM_SOCKET_MAX_NUM - 1
};

enum EItemTypes
{
    ITEM_NONE,              //0
    ITEM_WEAPON,            //1//weapon
    ITEM_ARMOR,             //2//armor
    ITEM_USE,               //3//usable item
    ITEM_AUTOUSE,           //4
    ITEM_MATERIAL,          //5
    ITEM_SPECIAL,           //6 //special item
    ITEM_TOOL,              //7
    ITEM_UNUSED_1,          //8//lottery
    ITEM_ELK,               //9//money
    ITEM_METIN,             //10
    ITEM_CONTAINER,         //11
    ITEM_FISH,              //12//fishing
    ITEM_ROD,               //13
    ITEM_RESOURCE,          //14
    ITEM_CAMPFIRE,          //15
    ITEM_UNIQUE,            //16
    ITEM_SKILLBOOK,         //17
    ITEM_QUEST,             //18
    ITEM_POLYMORPH,         //19
    ITEM_TREASURE_BOX,      //20//treasure box
    ITEM_TREASURE_KEY,      //21//treasure box key
    ITEM_SKILLFORGET,       //22
    ITEM_GIFTBOX,           //23
    ITEM_PICK,              //24
    ITEM_HAIR,              //25//hair
    ITEM_TOTEM,             //26//totem
	ITEM_BLEND,				//27//potion that gets random attributes when created
	ITEM_COSTUME,			//28//costume item (for the costume system added in August 2011)
	ITEM_DS,				//29 //dragon soul stone
	ITEM_SPECIAL_DS,		//30 // special dragon soul stone (think of it as a UNIQUE item worn in DS_SLOT)
	ITEM_EXTRACT,			//31 extraction tool.
	ITEM_SECONDARY_COIN,	//32 ?? Myeongdo coin??
	ITEM_RING,				//33 ring
	ITEM_BELT,				//34 belt
};

enum EMetinSubTypes
{
	METIN_NORMAL,
	METIN_GOLD,
};

enum EWeaponSubTypes
{
	WEAPON_SWORD,
	WEAPON_DAGGER,
	WEAPON_BOW,
	WEAPON_TWO_HANDED,
	WEAPON_BELL,
	WEAPON_FAN,
	WEAPON_ARROW,
	WEAPON_MOUNT_SPEAR,
	WEAPON_NUM_TYPES,
};

enum EArmorSubTypes
{
	ARMOR_BODY,
	ARMOR_HEAD,
	ARMOR_SHIELD,
	ARMOR_WRIST,
	ARMOR_FOOTS,
	ARMOR_NECK,
	ARMOR_EAR,
	ARMOR_NUM_TYPES
};

enum ECostumeSubTypes
{
	COSTUME_BODY = ARMOR_BODY,			// [IMPORTANT!!] ECostumeSubTypes enum values must equal the matching EArmorSubTypes values.
	COSTUME_HAIR = ARMOR_HEAD,			// This reuses the existing logic, because the business team asked for bonus attributes on costume items.
	COSTUME_NUM_TYPES,
};

enum EDragonSoulSubType
{
	DS_SLOT1,
	DS_SLOT2,
	DS_SLOT3,
	DS_SLOT4,
	DS_SLOT5,
	DS_SLOT6,
	DS_SLOT_MAX,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,

};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};
#define DRAGON_SOUL_STRENGTH_MAX 7

enum EDSInventoryMaxNum
{
	DRAGON_SOUL_INVENTORY_MAX_NUM = DS_SLOT_MAX * DRAGON_SOUL_GRADE_MAX * DRAGON_SOUL_BOX_SIZE,
	DRAGON_SOUL_REFINE_WINDOW_MAX_NUM,
};

enum EFishSubTypes
{
	FISH_ALIVE,
	FISH_DEAD,
};

enum EResourceSubTypes
{
	RESOURCE_FISHBONE,
	RESOURCE_WATERSTONEPIECE,
	RESOURCE_WATERSTONE,
	RESOURCE_BLOOD_PEARL,
	RESOURCE_BLUE_PEARL,
	RESOURCE_WHITE_PEARL,
	RESOURCE_BUCKET,
	RESOURCE_CRYSTAL,
	RESOURCE_GEM,
	RESOURCE_STONE,
	RESOURCE_METIN,
	RESOURCE_ORE,
};

enum EUniqueSubTypes
{
	UNIQUE_NONE,
	UNIQUE_BOOK,
	UNIQUE_SPECIAL_RIDE,
	UNIQUE_SPECIAL_MOUNT_RIDE,
};

enum EUseSubTypes
{
	USE_POTION,					// 0
	USE_TALISMAN,
	USE_TUNING,
	USE_MOVE,
	USE_TREASURE_BOX,
	USE_MONEYBAG,
	USE_BAIT,
	USE_ABILITY_UP,
	USE_AFFECT,
	USE_CREATE_STONE,
	USE_SPECIAL,				// 10
	USE_POTION_NODELAY,
	USE_CLEAR,
	USE_INVISIBILITY,
	USE_DETACHMENT,
	USE_BUCKET,
	USE_POTION_CONTINUE,
	USE_CLEAN_SOCKET,
	USE_CHANGE_ATTRIBUTE,
	USE_ADD_ATTRIBUTE,
	USE_ADD_ACCESSORY_SOCKET,	// 20
	USE_PUT_INTO_ACCESSORY_SOCKET,
	USE_ADD_ATTRIBUTE2,
	USE_RECIPE,
	USE_CHANGE_ATTRIBUTE2,
	USE_BIND,
	USE_UNBIND,
	USE_TIME_CHARGE_PER,
	USE_TIME_CHARGE_FIX,				// 28
	USE_PUT_INTO_BELT_SOCKET,			// 29 item usable on a belt socket
	USE_PUT_INTO_RING_SOCKET,			// 30 item usable on a ring socket (the newly added ring slot, not the unique ring)
};

enum EExtractSubTypes
{
	EXTRACT_DRAGON_SOUL,
	EXTRACT_DRAGON_HEART,
};

enum EAutoUseSubTypes
{
	AUTOUSE_POTION,
	AUTOUSE_ABILITY_UP,
	AUTOUSE_BOMB,
	AUTOUSE_GOLD,
	AUTOUSE_MONEYBAG,
	AUTOUSE_TREASURE_BOX
};

enum EMaterialSubTypes
{
	MATERIAL_LEATHER,
	MATERIAL_BLOOD,
	MATERIAL_ROOT,
	MATERIAL_NEEDLE,
	MATERIAL_JEWEL,
	MATERIAL_DS_REFINE_NORMAL, 
	MATERIAL_DS_REFINE_BLESSED, 
	MATERIAL_DS_REFINE_HOLLY,
};

enum ESpecialSubTypes
{
	SPECIAL_MAP,
	SPECIAL_KEY,
	SPECIAL_DOC,
	SPECIAL_SPIRIT,
};

enum EToolSubTypes
{
	TOOL_FISHING_ROD
};

enum EItemFlag
{
	ITEM_FLAG_REFINEABLE		= (1 << 0),
	ITEM_FLAG_SAVE			= (1 << 1),
	ITEM_FLAG_STACKABLE		= (1 << 2),	// can be stacked
	ITEM_FLAG_COUNT_PER_1GOLD	= (1 << 3),
	ITEM_FLAG_SLOW_QUERY		= (1 << 4),
	ITEM_FLAG_UNUSED01		= (1 << 5),	// UNUSED
	ITEM_FLAG_UNIQUE		= (1 << 6),
	ITEM_FLAG_MAKECOUNT		= (1 << 7),
	ITEM_FLAG_IRREMOVABLE		= (1 << 8),
	ITEM_FLAG_CONFIRM_WHEN_USE	= (1 << 9),
	ITEM_FLAG_QUEST_USE		= (1 << 10),
	ITEM_FLAG_QUEST_USE_MULTIPLE	= (1 << 11),
	ITEM_FLAG_QUEST_GIVE		= (1 << 12),
	ITEM_FLAG_LOG			= (1 << 13),
	ITEM_FLAG_APPLICABLE		= (1 << 14),
};

enum EItemAntiFlag
{
	ITEM_ANTIFLAG_FEMALE	= (1 << 0), // not usable by females
	ITEM_ANTIFLAG_MALE		= (1 << 1), // not usable by males
	ITEM_ANTIFLAG_WARRIOR	= (1 << 2), // not usable by warriors
	ITEM_ANTIFLAG_ASSASSIN	= (1 << 3), // not usable by assassins
	ITEM_ANTIFLAG_SURA		= (1 << 4), // not usable by suras
	ITEM_ANTIFLAG_SHAMAN	= (1 << 5), // not usable by shamans
	ITEM_ANTIFLAG_GET		= (1 << 6), // cannot be picked up
	ITEM_ANTIFLAG_DROP		= (1 << 7), // cannot be dropped
	ITEM_ANTIFLAG_SELL		= (1 << 8), // cannot be sold
	ITEM_ANTIFLAG_EMPIRE_A	= (1 << 9), // A empire: not usable
	ITEM_ANTIFLAG_EMPIRE_B	= (1 << 10), // B empire: not usable
	ITEM_ANTIFLAG_EMPIRE_C	= (1 << 11), // C empire: not usable
	ITEM_ANTIFLAG_SAVE		= (1 << 12), // not saved
	ITEM_ANTIFLAG_GIVE		= (1 << 13), // cannot be traded
	ITEM_ANTIFLAG_PKDROP	= (1 << 14), // PK does not drop it
	ITEM_ANTIFLAG_STACK		= (1 << 15), // cannot be stacked
	ITEM_ANTIFLAG_MYSHOP	= (1 << 16), // cannot be put in a private shop
	ITEM_ANTIFLAG_SAFEBOX	= (1 << 17), // cannot be put in the safebox
};

enum EItemWearableFlag
{
	WEARABLE_BODY	= (1 << 0),
	WEARABLE_HEAD	= (1 << 1),
	WEARABLE_FOOTS	= (1 << 2),
	WEARABLE_WRIST	= (1 << 3),
	WEARABLE_WEAPON	= (1 << 4),
	WEARABLE_NECK	= (1 << 5),
	WEARABLE_EAR	= (1 << 6),
	WEARABLE_UNIQUE	= (1 << 7),
	WEARABLE_SHIELD	= (1 << 8),
	WEARABLE_ARROW	= (1 << 9),
	WEARABLE_HAIR	= (1 << 10),
	WEARABLE_ABILITY		= (1 << 11),
	WEARABLE_COSTUME_BODY	= (1 << 12),
};

enum ELimitTypes
{
	LIMIT_NONE,

	LIMIT_LEVEL,
	LIMIT_STR,
	LIMIT_DEX,
	LIMIT_INT,
	LIMIT_CON,

	/// Time runs down in real time whether equipped or not (socket0 holds the expiry time as a unix_timestamp)
	LIMIT_REAL_TIME,						

	/// The real-time timer starts when the item is first used (or equipped).
	/// Before first use, socket0 holds the usable time (in seconds; 0 means use the proto limit value);
	/// on use, socket1 gets the use count and socket0 the expiry time as a unix_timestamp.
	LIMIT_REAL_TIME_START_FIRST_USE,

	/// Usage time only runs down while the item is equipped.
	/// socket0 holds the remaining time in seconds (on first use, if it is 0, the proto limit value is copied to socket0)
	LIMIT_TIMER_BASED_ON_WEAR,

	LIMIT_MAX_NUM
};

enum EAttrAddonTypes
{
	ATTR_ADDON_NONE,
	// positive values are reserved for set
	ATTR_DAMAGE_ADDON = -1,
};

enum ERefineType
{
	REFINE_TYPE_NORMAL,
	REFINE_TYPE_NOT_USED1,
	REFINE_TYPE_SCROLL,
	REFINE_TYPE_HYUNIRON,
	REFINE_TYPE_MONEY_ONLY,
	REFINE_TYPE_MUSIN,
	REFINE_TYPE_BDRAGON,
};

#endif
