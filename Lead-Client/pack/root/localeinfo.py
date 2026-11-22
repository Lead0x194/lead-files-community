import app
import constInfo
import dbg
import sys
import types

MAP_TRENT02 = "MAP_TRENT02"
MAP_WL = "MAP_WL"
MAP_NUSLUCK = "MAP_NUSLUCK"
MAP_TREE2 = "MAP_TREE2"

BLEND_POTION_NO_TIME = "BLEND_POTION_NO_TIME"
BLEND_POTION_NO_INFO = "BLEND_POTION_NO_INFO"

APP_TITLE = "METIN2"

GUILD_HEADQUARTER = "Main Building"
GUILD_FACILITY = "Facility"
GUILD_OBJECT = "Object"
GUILD_MEMBER_COUNT_INFINITY = "INFINITY"

LOGIN_FAILURE_WEB_BLOCK = "BLOCK_LOGIN(WEB)"
LOGIN_FAILURE_BLOCK_LOGIN = "BLOCK_LOGIN"
CHANNEL_NOTIFY_FULL = "CHANNEL_NOTIFY_FULL"

GUILD_BUILDING_LIST_TXT = app.GetLocalePath() + "/GuildBuildingList.txt"

GUILD_MARK_MIN_LEVEL = "3"
GUILD_MARK_NOT_ENOUGH_LEVEL = "��巹�� 3�̻� ���� �����մϴ�."

ERROR_MARK_UPLOAD_NEED_RECONNECT = "UploadMark: Reconnect to game"
ERROR_MARK_CHECK_NEED_RECONNECT = "CheckMark: Reconnect to game"

__IS_RTL		= app.IsRTL()
LOGIN_FAILURE_WRONG_SOCIALID = "ASDF"
LOGIN_FAILURE_SHUTDOWN_TIME = "ASDF"

def IsRTL():
	global __IS_RTL
	return __IS_RTL

class LocaleModule(types.ModuleType):
	def __getattr__(self, name):
		if name.startswith("__"):
			raise AttributeError(name)

		dbg.TraceError("%s: missing key %s" % (self.__name__, name))
		self.__dict__[name] = name
		return name

def ReportMissingKeys(moduleName):
	module = LocaleModule(moduleName)
	module.__dict__.update(sys.modules[moduleName].__dict__)
	module.__dict__["__source__"] = sys.modules[moduleName]
	sys.modules[moduleName] = module

def LoadLocaleData():
	app.LoadLocaleData(app.GetLocalePath())

def mapping(**kwargs): return kwargs

def SNA(text):	
	def f(x):
		return text
	return f

def SA(text):
	def f(x):
		return text % x
	return f

def LoadLocaleFile(srcFileName, localeDict):

	funcDict = {"SA":SA, "SNA":SNA}

	lineIndex = 1

	try:
		lines = pack_open(srcFileName, "r").readlines()
	except IOError:
		import dbg
		dbg.LogBox("LoadLocaleError(%(srcFileName)s)" % locals())
		app.Abort()

	for line in lines:
		try:		
			tokens = line[:-1].split("\t")
			if len(tokens) == 2:
				localeDict[tokens[0]] = tokens[1]
			elif len(tokens) >= 3:
				type = tokens[2].strip()
				if type:
					localeDict[tokens[0]] = funcDict[type](tokens[1])
				else:
					localeDict[tokens[0]] = tokens[1]
			else:
				raise RuntimeError, "Unknown TokenSize"

			lineIndex += 1
		except:
			import dbg
			dbg.LogBox("%s: line(%d): %s" % (srcFileName, lineIndex, line), "Error")
			raise


	
all = ["locale","error"]

FN_GM_MARK = "%s/effect/gm.mse"	% app.GetLocalePath()
LOCALE_FILE_NAME = "%s/locale_game.txt" % app.GetLocalePath()
constInfo.IN_GAME_SHOP_ENABLE = 0

LoadLocaleFile(LOCALE_FILE_NAME, locals())

########################################################################################################
locale = mapping(
)


def SecondToDHM(time):
	if time < 60:
		if IsRTL():
			return "%.2f %s" % (time, SECOND)
		else:
			return "0" + MINUTE
		
	second = int(time % 60)
	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60) % 24
	day = int(int((time / 60) / 60) / 24)

	text = ""

	if day > 0:
		text += str(day) + DAY
		text += " "

	if hour > 0:
		text += str(hour) + HOUR
		text += " "

	if minute > 0:
		text += str(minute) + MINUTE

	return text

def SecondToHM(time):

	if time < 60:
		if IsRTL():
			return "%.2f %s" % (time, SECOND)
		else:
			return "0" + MINUTE

	second = int(time % 60)
	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60)

	text = ""

	if hour > 0:
		text += str(hour) + HOUR
		if hour > 0:
			text += " "

	if minute > 0:
		text += str(minute) + MINUTE

	return text


def GetAlignmentTitleName(alignment):
	if alignment >= 12000:
		return TITLE_NAME_LIST[0]
	elif alignment >= 8000:
		return TITLE_NAME_LIST[1]
	elif alignment >= 4000:
		return TITLE_NAME_LIST[2]
	elif alignment >= 1000:
		return TITLE_NAME_LIST[3]
	elif alignment >= 0:
		return TITLE_NAME_LIST[4]
	elif alignment > -4000:
		return TITLE_NAME_LIST[5]
	elif alignment > -8000:
		return TITLE_NAME_LIST[6]
	elif alignment > -12000:
		return TITLE_NAME_LIST[7]

	return TITLE_NAME_LIST[8]


OPTION_PVPMODE_MESSAGE_DICT = {
	0 : PVP_MODE_NORMAL,
	1 : PVP_MODE_REVENGE,
	2 : PVP_MODE_KILL,
	3 : PVP_MODE_PROTECT,
	4 : PVP_MODE_GUILD,
}

error = mapping(
	CREATE_WINDOW = GAME_INIT_ERROR_MAIN_WINDOW,
	CREATE_CURSOR = GAME_INIT_ERROR_CURSOR,
	CREATE_NETWORK = GAME_INIT_ERROR_NETWORK,
	CREATE_ITEM_PROTO = GAME_INIT_ERROR_ITEM_PROTO,
	CREATE_MOB_PROTO = GAME_INIT_ERROR_MOB_PROTO,
	CREATE_NO_DIRECTX = GAME_INIT_ERROR_DIRECTX,
	CREATE_DEVICE = GAME_INIT_ERROR_GRAPHICS_NOT_EXIST,
	CREATE_NO_APPROPRIATE_DEVICE = GAME_INIT_ERROR_GRAPHICS_BAD_PERFORMANCE,
	CREATE_FORMAT = GAME_INIT_ERROR_GRAPHICS_NOT_SUPPORT_32BIT,
	NO_ERROR = ""
)


GUILDWAR_NORMAL_DESCLIST = [GUILD_WAR_USE_NORMAL_MAP, GUILD_WAR_LIMIT_30MIN, GUILD_WAR_WIN_CHECK_SCORE]
GUILDWAR_WARP_DESCLIST = [GUILD_WAR_USE_BATTLE_MAP, GUILD_WAR_WIN_WIPE_OUT_GUILD, GUILD_WAR_REWARD_POTION]
GUILDWAR_CTF_DESCLIST = [GUILD_WAR_USE_BATTLE_MAP, GUILD_WAR_WIN_TAKE_AWAY_FLAG1, GUILD_WAR_WIN_TAKE_AWAY_FLAG2, GUILD_WAR_REWARD_POTION]

MINIMAP_ZONE_NAME_DICT = {
	"metin2_map_a1"  : MAP_A1,
	"map_a2"         : MAP_A2,
	"metin2_map_a3"  : MAP_A3,
	"metin2_map_b1"  : MAP_B1,
	"map_b2"         : MAP_B2,
	"metin2_map_b3"  : MAP_B3,
	"metin2_map_c1"  : MAP_C1,
	"map_c2"         : MAP_C2,
	"metin2_map_c3"  : MAP_C3,
	"map_n_snowm_01" : MAP_SNOW,
	"metin2_map_n_flame_01" : MAP_FLAME,
	"metin2_map_n_desert_01" : MAP_DESERT,
	"metin2_map_milgyo" : MAP_TEMPLE,
	"metin2_map_spiderdungeon" : MAP_SPIDER,
	"metin2_map_deviltower1" : MAP_SKELTOWER,
	"metin2_map_guild_01" : MAP_AG,
	"metin2_map_guild_02" : MAP_BG,
	"metin2_map_guild_03" : MAP_CG,
	"metin2_map_trent" : MAP_TREE,
	"metin2_map_trent02" : MAP_TREE2,
	"season1/metin2_map_WL_01" : MAP_WL,
	"season1/metin2_map_nusluck01" : MAP_NUSLUCK,
    "Metin2_map_CapeDragonHead" : MAP_CAPE,
    "metin2_map_Mt_Thunder" : MAP_THUNDER,
    "metin2_map_dawnmistwood" : MAP_DAWN,
    "metin2_map_BayBlackSand" : MAP_BAY,
}



JOBINFO_TITLE = [
	[JOB_WARRIOR0, JOB_WARRIOR1, JOB_WARRIOR2,],
	[JOB_ASSASSIN0, JOB_ASSASSIN1, JOB_ASSASSIN2,],
	[JOB_SURA0, JOB_SURA1, JOB_SURA2,],
	[JOB_SHAMAN0, JOB_SHAMAN1, JOB_SHAMAN2,],
]

JOBINFO_DATA_LIST = [
	[
		["Ÿ���� ��Ͱ� ������ �ʴ� ������",
		"�ⰳ�� ������� ���þ� [����]���",
		"�θ���. ��� ���⿡���� �׵��� ",
		"�ڷ� �������� ������, ��ġ�� ����",
		"�̱� ���� ���Ḧ ���� �ܽ�����",
		"����� ���� �ο�⵵ �Ѵ�. �̵���",
		"�� �ܷõ� ������ ��, ������ ���ݷ�",
		"���� ���� �ּ��ο��� ����������",
		"Ȱ���Ѵ�.                      ",],
		["���� �Ϲ����� ������ �����, ",
		"�������� ���� ���� �������� ����",
		"���� Ȱ���Ѵ�. ���� Ư���� �ٷ���",
		"�������� ���� ����Ʈ�� �����ϵ�, ",
		"�������� ���� ������ / ����",
		"Ȯ���� ���� ü���� �ø���. ����",
		"������ ��Ȯ���� ���̱� ���� ��ø",
		"���� ����Ʈ�� ������ �ʿ䰡 �ִ�.",],
		["��� ������ ���ŷ��� �̿��ϴ�",
		"��/�ٰŸ� ������ �����, �� ���",
		"�ϳ��ϳ��� ���� ���ݷ����� ���忡��",
		"Ȱ���Ѵ�. ���� Ư���� �ٷ��� ����",
		"���� ���� ����Ʈ�� �����ϵ�, ",
		"��/�ٰŸ� ������ ��Ȯ���� ���߷���",
		"���� ��ø�� �ø���. ���� ���� �� ",
		"�� ���ݿ� ���� ������ / ����",
		"Ȯ���� ���� ü�¿��� ����Ʈ��",
		"������ �ʿ䰡 �ִ�.        ",],		
	],
	[
		["�ڰ��� ��� ��Ȳ������ �ڽ���",
		"���� ����� ������ ����� �ӹ���",
		"�����ϸ鼭 ������ ������ �����ϴ�", 
		"�ڵ��̴�. �̵��� ���� ������ �ż�",
		"�ϸ�, ���� �� ���� �����ϰ� ������",
		"�ൿ���� ���� �޼ҿ� ġ��Ÿ�� ����",
		"��, ���忡�� ������ ���� ������",
		"ȭ���� �������� �ڽ��� �����",
		"�����δ�.                   "],
		["�μ� �ܰ��� �ֹ���� �ٷ��, �ż�",
		"�ϰ� ġ�� ������ �ڰ� Ư���� ������",
		"���� ���忡�� Ȱ���Ѵ�. ���� Ư����",
		"��ø�� �������� ���� ����Ʈ�� ����",
		"�ϵ�, �ٷ��� �÷� ���ݷ��� ���δ�.",
		"���� �������� ���� ������/���� ",
		"����� ���� ü�¿��� ����Ʈ��",
		"������ �ʿ䰡 �ִ�.          ",],
		["Ȱ�� �ֹ���� �ٷ��, �� �þ߿�",
		"�����Ÿ��� ���� ���Ÿ� ��������",
		"���忡�� Ȱ���Ѵ�. ���� Ư����",
		"���� �������� ������ ���� ��ø��",
		"�������� �÷��� �ϸ�, ���Ÿ�",
		"������ ������ ������ ���� �ٷ���",
		"�ø� �ʿ䰡 �ִ�. ���� ���鿡��",
		"�����Ǿ��� ��, �� ���ݿ� ��Ƽ��",
		"���� ������/���� ����� ����",
		"ü�¿��� ����Ʈ�� ������ �ʿ䰡",
		"�ִ�.                        ", ],
	],
	[
		["����� [���� ������]�� �Ӽ�����",
		"â���� Ư�� �Ӽ��� �����̴�. ",
		"�׵��� ���忡�� ������ ��⸦ ����",
		"��Ű��, �Ǹ��� ���� ���� ��ź����",
		"���� ��ȥ�� ������ ��������. ����",
		"�̵��� �ڽ��� �˰� ���ʿ� �����",
		"���� �Ǿ�, ���忡�� ���� ���� ����",
		"���ݷ��� �����ϱ⵵ �ϴµ�, ������",
		"�׿���±� ����� ������ ������",
		"������� ���� ���þ� [����]�̶�",
		"�θ��⸦ ���� �ɴ´�."],
		["ȯ������ ����� �Ǹ��� ������",
		"������� ������ ���⳪ ����",
		"�Ǿ� ���� ���� ���� ����������",
		"���忡�� Ȱ���Ѵ�. ���� Ư����",
		"������ ���������� ���� ���", 
		"�Ǹ��� ������ ������ ����ǹǷ�,",
		"���ɰ� �ٷ��� �������� ����",
		"����Ʈ�� �����ϵ�, ������ ����",
		"������/���� Ȯ���� ���� ü����",
		"�ø���. ���� ������ ��Ȯ����",
		"ȸ�Ǹ� ���ؼ� ��ø���� ����Ʈ��",
		"������ �ʿ䰡 �ִ�.           ",],
		["�渶���� ������� ���� �����",
		"�ֹ��� �Ǹ��� �������� ���忡��",
		"Ȱ���Ѵ�. ���� Ư���� ���� ������",
		"���̹Ƿ� ������ �������� ����",
		"����Ʈ�� �����ϵ�, ���Ÿ� ����",
		"������ ��Ȯ���� ���� ��ø�� �ø���.",
		"���� ���� �Ǿ�����, �� ���ݿ� ����",
		"������ / ���� Ȯ���� ���� ü�¿���",
		"����Ʈ�� ������ �ʿ䰡 �ִ�.    ",],
	],
	[
		["������ ��Ű� �ڿ�, �� ������",
		"���� �ٷ� �� �ִ� ������ �����̴�.",
		"�׵��� �Ĺ濡�� �Ʊ��� �����ϰ�",
		"��ģ ������ �λ��� ȸ�� ��Ű��",
		"������ ��⸦ ��½�Ų��. �׵���",
		"�Ʊ��� ����� �޽��� �����ϴ� �ڸ� ",
		"���� �뼭���� ������, �׷� �ڵ�",
		"���Դ� �� �� ���� ���� �ֹ���",
		"��Ʈ�� �� ������� ���� ¡���Ѵ�.",],
		["õ�決�� ������� ���� ��������",
		"�����ֹ��� ���ϸ�, ���� �� / ����",
		"�������κ��� �Ʊ��� ��Ų��. ����",
		"Ư���� ���� �ɷ��� ���̹Ƿ� ������",
		"�������� ���� ����Ʈ�� �����ϵ�,",
		"�����Ǿ��� ��, �� ���ݿ� ����",
		"������ / ���� Ȯ���� ���� ü����",
		"�ø���. ���� ���Ÿ� ���� ������",
		"��Ȯ���� ���� ��ø���� ����Ʈ��",
		"������ �ʿ䰡 �ִ�.           ",],
		["���ڱ��� ������� �ڿ��� ����",
		"���� �Ʊ��� ȸ���ϰ�, ������ ",
		"������ ������ ���鿡�� ū �����",
		"���� �� �ִ� �̵��̴�. ������",
		"Ư���� ���� �ɷ��� ���̹Ƿ� ������",
		"�������� ���� ����Ʈ�� �����ϵ�,",
		"�����Ǿ�����, �� ���ݿ� ����",
		"������ / ���� Ȯ���� ���� ü����",
		"�ø���. ���� ���Ÿ� ���� ������",
		"��Ȯ���� ���� ��ø���� ����Ʈ��",
		"������ �ʿ䰡 �ִ�.             "],
	],
]


WHISPER_ERROR = {
	1 : CANNOT_WHISPER_NOT_LOGON,
	2 : CANNOT_WHISPER_DEST_REFUSE,
	3 : CANNOT_WHISPER_SELF_REFUSE,
}

NOTIFY_MESSAGE = {
	"CANNOT_EQUIP_SHOP" : CANNOT_EQUIP_IN_SHOP,
	"CANNOT_EQUIP_EXCHANGE" : CANNOT_EQUIP_IN_EXCHANGE,
}


ATTACK_ERROR_TAIL_DICT = {
	"IN_SAFE" : CANNOT_ATTACK_SELF_IN_SAFE,
	"DEST_IN_SAFE" : CANNOT_ATTACK_DEST_IN_SAFE,
}

SHOT_ERROR_TAIL_DICT = {
	"EMPTY_ARROW" : CANNOT_SHOOT_EMPTY_ARROW,
	"IN_SAFE" : CANNOT_SHOOT_SELF_IN_SAFE,
	"DEST_IN_SAFE" : CANNOT_SHOOT_DEST_IN_SAFE,
}
	
USE_SKILL_ERROR_TAIL_DICT = {	
	"IN_SAFE" : CANNOT_SKILL_SELF_IN_SAFE,
	"NEED_TARGET" : CANNOT_SKILL_NEED_TARGET,
	"NEED_EMPTY_BOTTLE" : CANNOT_SKILL_NEED_EMPTY_BOTTLE,
	"NEED_POISON_BOTTLE" : CANNOT_SKILL_NEED_POISON_BOTTLE,
	"REMOVE_FISHING_ROD" : CANNOT_SKILL_REMOVE_FISHING_ROD,
	"NOT_YET_LEARN" : CANNOT_SKILL_NOT_YET_LEARN,
	"NOT_MATCHABLE_WEAPON" : CANNOT_SKILL_NOT_MATCHABLE_WEAPON,
	"WAIT_COOLTIME" : CANNOT_SKILL_WAIT_COOLTIME,
	"NOT_ENOUGH_HP" : CANNOT_SKILL_NOT_ENOUGH_HP,
	"NOT_ENOUGH_SP" : CANNOT_SKILL_NOT_ENOUGH_SP,
	"CANNOT_USE_SELF" : CANNOT_SKILL_USE_SELF,
	"ONLY_FOR_ALLIANCE" : CANNOT_SKILL_ONLY_FOR_ALLIANCE,
	"CANNOT_ATTACK_ENEMY_IN_SAFE_AREA" : CANNOT_SKILL_DEST_IN_SAFE,
	"CANNOT_APPROACH" : CANNOT_SKILL_APPROACH,
	"CANNOT_ATTACK" : CANNOT_SKILL_ATTACK,
	"ONLY_FOR_CORPSE" : CANNOT_SKILL_ONLY_FOR_CORPSE,
	"EQUIP_FISHING_ROD" : CANNOT_SKILL_EQUIP_FISHING_ROD, 
	"NOT_HORSE_SKILL" : CANNOT_SKILL_NOT_HORSE_SKILL,
	"HAVE_TO_RIDE" : CANNOT_SKILL_HAVE_TO_RIDE,
}

LEVEL_LIST=["", HORSE_LEVEL1, HORSE_LEVEL2, HORSE_LEVEL3]

HEALTH_LIST=[
	HORSE_HEALTH0,
	HORSE_HEALTH1, 
	HORSE_HEALTH2,
	HORSE_HEALTH3,
]


USE_SKILL_ERROR_CHAT_DICT = {	
	"NEED_EMPTY_BOTTLE" : SKILL_NEED_EMPTY_BOTTLE,
	"NEED_POISON_BOTTLE" : SKILL_NEED_POISON_BOTTLE, 
	"ONLY_FOR_GUILD_WAR" : SKILL_ONLY_FOR_GUILD_WAR,
}

SHOP_ERROR_DICT = {
	"NOT_ENOUGH_MONEY" : SHOP_NOT_ENOUGH_MONEY,
	"SOLDOUT" : SHOP_SOLDOUT,
	"INVENTORY_FULL" : SHOP_INVENTORY_FULL,
	"INVALID_POS" : SHOP_INVALID_POS,
	"NOT_ENOUGH_MONEY_EX" : SHOP_NOT_ENOUGH_MONEY_EX,
}

STAT_MINUS_DESCRIPTION = {
	"HTH-" : STAT_MINUS_CON,
	"INT-" : STAT_MINUS_INT,
	"STR-" : STAT_MINUS_STR,
	"DEX-" : STAT_MINUS_DEX,
}

MODE_NAME_LIST = ( PVP_OPTION_NORMAL, PVP_OPTION_REVENGE, PVP_OPTION_KILL, PVP_OPTION_PROTECT, )
TITLE_NAME_LIST = ( PVP_LEVEL0, PVP_LEVEL1, PVP_LEVEL2, PVP_LEVEL3, PVP_LEVEL4, PVP_LEVEL5, PVP_LEVEL6, PVP_LEVEL7, PVP_LEVEL8, )

def GetLetterImageName():
	return "season1/icon/scroll_close.tga"
def GetLetterOpenImageName():
	return "season1/icon/scroll_open.tga"
def GetLetterCloseImageName():
	return "season1/icon/scroll_close.tga"

def DO_YOU_SELL_ITEM(sellItemName, sellItemCount, sellItemPrice):
	if sellItemCount > 1 :
		return DO_YOU_SELL_ITEM2 % (sellItemName, sellItemCount, NumberToMoneyString(sellItemPrice) )
	else:
		return DO_YOU_SELL_ITEM1 % (sellItemName, NumberToMoneyString(sellItemPrice) )

def DO_YOU_BUY_ITEM(buyItemName, buyItemCount, buyItemPrice) :
	if buyItemCount > 1 :
		return DO_YOU_BUY_ITEM2 % ( buyItemName, buyItemCount, buyItemPrice )
	else:
		return DO_YOU_BUY_ITEM1 % ( buyItemName, buyItemPrice )
		
def REFINE_FAILURE_CAN_NOT_ATTACH(attachedItemName) :
	return REFINE_FAILURE_CAN_NOT_ATTACH0 % (attachedItemName)

def REFINE_FAILURE_NO_SOCKET(attachedItemName) :
	return REFINE_FAILURE_NO_SOCKET0 % (attachedItemName)

def REFINE_FAILURE_NO_GOLD_SOCKET(attachedItemName) :
	return REFINE_FAILURE_NO_GOLD_SOCKET0 % (attachedItemName)
	
def HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, dropItemCount) :
	if dropItemCount > 1 :
		return HOW_MANY_ITEM_DO_YOU_DROP2 % (dropItemName, dropItemCount)
	else :	
		return HOW_MANY_ITEM_DO_YOU_DROP1 % (dropItemName)

def FISHING_NOTIFY(isFish, fishName) :
	if isFish :
		return FISHING_NOTIFY1 % ( fishName )
	else :
		return FISHING_NOTIFY2 % ( fishName )

def FISHING_SUCCESS(isFish, fishName) :
	if isFish :
		return FISHING_SUCCESS1 % (fishName)
	else :
		return FISHING_SUCCESS2 % (fishName)
		
def NumberToMoneyString(n) :
	if n <= 0 :
		return "0 %s" % (MONETARY_UNIT0)

	return "%s %s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]), MONETARY_UNIT0) 

def NumberToSecondaryCoinString(n) :
	if n <= 0 :
		return "0 %s" % (MONETARY_UNIT_JUN)

	return "%s %s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]), MONETARY_UNIT_JUN) 

def DottedNumber(n) :
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ])) 

ReportMissingKeys(__name__)
