<?
$item_types = array( "ITEM_NONE", "ITEM_WEAPON", "ITEM_ARMOR", "ITEM_USE", "ITEM_AUTOUSE", "ITEM_MATERIAL", "ITEM_SPECIAL", "ITEM_TOOL",
						"ITEM_LOTTERY", "ITEM_ELK", "ITEM_METIN", "ITEM_CONTAINER", "ITEM_FISH", "ITEM_ROD", "ITEM_RESOURCE", "ITEM_CAMPFIRE",
						"ITEM_UNIQUE", "ITEM_SKILLBOOK", "ITEM_QUEST", "ITEM_POLYMORPH", "ITEM_TREASURE_BOX", "ITEM_TREASURE_KEY",
						"ITEM_SKILLFORGET", "ITEM_GIFTBOX", "ITEM_PICK", "ITEM_HAIR", "ITEM_TOTEM", "ITEM_BLEND", "ITEM_COSTUME") ;

$antiflag_types = array( "ANTI_FEMALE", "ANTI_MALE", "ANTI_MUSA", "ANTI_ASSASSIN", "ANTI_SURA", "ANTI_MUDANG",
							"ANTI_GET", "ANTI_DROP", "ANTI_SELL", "ANTI_EMPIRE_A", "ANTI_EMPIRE_B", "ANTI_EMPIRE_C",
							"ANTI_SAVE", "ANTI_GIVE", "ANTI_PKDROP", "ANTI_STACK", "ANTI_MYSHOP", "ANTI_SAFEBOX") ;

$item_subtype = array(  /*0*/0,
						/*1*/array( "WEAPON_SWORD", "WEAPON_DAGGER", "WEAPON_BOW", "WEAPON_TWO_HANDED",
									"WEAPON_BELL", "WEAPON_FAN", "WEAPON_ARROW", "WEAPON_MOUNT_SPEAR"),
						/*2*/array( "ARMOR_BODY", "ARMOR_HEAD", "ARMOR_SHIELD", "ARMOR_WRIST", "ARMOR_FOOTS",
									"ARMOR_NECK", "ARMOR_EAR", "ARMOR_NUM_TYPES"),
						/*3*/array( "USE_POTION", "USE_TALISMAN", "USE_TUNING", "USE_MOVE", "USE_TREASURE_BOX", "USE_MONEYBAG", "USE_BAIT",
									"USE_ABILITY_UP", "USE_AFFECT", "USE_CREATE_STONE", "USE_SPECIAL", "USE_POTION_NODELAY", "USE_CLEAR",
									"USE_INVISIBILITY", "USE_DETACHMENT", "USE_BUCKET", "USE_POTION_CONTINUE", "USE_CLEAN_SOCKET",
									"USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET",
									"USE_ADD_ATTRIBUTE2", "USE_RECIPE", "USE_CHANGE_ATTRIBUTE2", "USE_BIND", "USE_UNBIND"),
						/*4*/array( "AUTOUSE_POTION", "AUTOUSE_ABILITY_UP", "AUTOUSE_BOMB", "AUTOUSE_GOLD", "AUTOUSE_MONEYBAG", "AUTOUSE_TREASURE_BOX"),
						/*5*/array( "MATERIAL_LEATHER", "MATERIAL_BLOOD", "MATERIAL_ROOT", "MATERIAL_NEEDLE", "MATERIAL_JEWEL"),
						/*6*/array( "SPECIAL_MAP", "SPECIAL_KEY", "SPECIAL_DOC", "SPECIAL_SPIRIT"),
						/*7*/array( "TOOL_FISHING_ROD" ),
						/*8*/array( "LOTTERY_TICKET", "LOTTERY_INSTANT" ),
						/*9*/0,
						/*10*/array( "METIN_NORMAL", "METIN_GOLD" ),
						/*11*/0,
						/*12*/array( "FISH_ALIVE", "FISH_DEAD"),
						/*13*/0,
						/*14*/array( "RESOURCE_FISHBONE", "RESOURCE_WATERSTONEPIECE", "RESOURCE_WATERSTONE", "RESOURCE_BLOOD_PEARL",
										"RESOURCE_BLUE_PEARL", "RESOURCE_WHITE_PEARL", "RESOURCE_BUCKET", "RESOURCE_CRYSTAL", "RESOURCE_GEM",
										"RESOURCE_STONE", "RESOURCE_METIN", "RESOURCE_ORE" ),
						/*15*/0,
						/*16*/array( "UNIQUE_NONE", "UNIQUE_BOOK", "UNIQUE_SPECIAL_RIDE" ),
						/*17*/0,
						/*18*/0,
						/*19*/0,
						/*20*/0,
						/*21*/0,
						/*22*/0,
						/*23*/0,
						/*24*/0,
						/*25*/0,
						/*26*/0,
						/*27*/0,
						/*28*/array( "COSTUME_BODY", "COSTUME_HAIR" )
					) ;

$flag_types = array( "ITEM_TUNABLE", "ITEM_SAVE", "ITEM_STACKABLE", "COUNT_PER_1GOLD", "ITEM_SLOW_QUERY", "ITEM_UNIQUE",
						"ITEM_MAKECOUNT", "ITEM_IRREMOVABLE", "CONFIRM_WHEN_USE", "QUEST_USE", "QUEST_USE_MULTIPLE",
						"QUEST_GIVE", "ITEM_QUEST", "LOG", "STACKABLE", "SLOW_QUERY", "REFINEABLE", "IRREMOVABLE", "ITEM_APPLICABLE") ;

$wearflag_types = array( "WEAR_BODY", "WEAR_HEAD", "WEAR_FOOTS", "WEAR_WRIST", "WEAR_WEAPON", "WEAR_NECK", "WEAR_EAR", "WEAR_SHIELD", "WEAR_UNIQUE",
							"WEAR_ARROW", "WEAR_HAIR", "WEAR_ABILITY") ;

$limit_types = array("LIMIT_NONE", "LEVEL", "STR", "DEX", "INT", "CON", "PC_BANG", "REAL_TIME") ;

$apply_types = array( "APPLY_NONE", "APPLY_MAX_HP", "APPLY_MAX_SP", "APPLY_CON", "APPLY_INT", "APPLY_STR", "APPLY_DEX", "APPLY_ATT_SPEED",
						"APPLY_MOV_SPEED", "APPLY_CAST_SPEED", "APPLY_HP_REGEN", "APPLY_SP_REGEN", "APPLY_POISON_PCT", "APPLY_STUN_PCT",
						"APPLY_SLOW_PCT", "APPLY_CRITICAL_PCT", "APPLY_PENETRATE_PCT", "APPLY_ATTBONUS_HUMAN", "APPLY_ATTBONUS_ANIMAL",
						"APPLY_ATTBONUS_ORC", "APPLY_ATTBONUS_MILGYO", "APPLY_ATTBONUS_UNDEAD", "APPLY_ATTBONUS_DEVIL", "APPLY_STEAL_HP",
						"APPLY_STEAL_SP", "APPLY_MANA_BURN_PCT", "APPLY_DAMAGE_SP_RECOVER", "APPLY_BLOCK", "APPLY_DODGE", "APPLY_RESIST_SWORD",
						"APPLY_RESIST_TWOHAND", "APPLY_RESIST_DAGGER", "APPLY_RESIST_BELL", "APPLY_RESIST_FAN", "APPLY_RESIST_BOW", "APPLY_RESIST_FIRE",
						"APPLY_RESIST_ELEC", "APPLY_RESIST_MAGIC", "APPLY_RESIST_WIND", "APPLY_REFLECT_MELEE", "APPLY_REFLECT_CURSE", "APPLY_POISON_REDUCE",
						"APPLY_KILL_SP_RECOVER", "APPLY_EXP_DOUBLE_BONUS", "APPLY_GOLD_DOUBLE_BONUS", "APPLY_ITEM_DROP_BONUS", "APPLY_POTION_BONUS",
						"APPLY_KILL_HP_RECOVER", "APPLY_IMMUNE_STUN", "APPLY_IMMUNE_SLOW", "APPLY_IMMUNE_FALL", "APPLY_SKILL", "APPLY_BOW_DISTANCE",
						"APPLY_ATT_GRADE_BONUS", "APPLY_DEF_GRADE_BONUS", "APPLY_MAGIC_ATT_GRADE", "APPLY_MAGIC_DEF_GRADE", "APPLY_CURSE_PCT",
						"APPLY_MAX_STAMINA", "APPLY_ATTBONUS_WARRIOR", "APPLY_ATTBONUS_ASSASSIN", "APPLY_ATTBONUS_SURA", "APPLY_ATTBONUS_SHAMAN",
						"APPLY_ATTBONUS_MONSTER", "APPLY_MALL_ATTBONUS", "APPLY_MALL_DEFBONUS", "APPLY_MALL_EXPBONUS", "APPLY_MALL_ITEMBONUS",
						"APPLY_MALL_GOLDBONUS", "APPLY_MAX_HP_PCT", "APPLY_MAX_SP_PCT", "APPLY_SKILL_DAMAGE_BONUS", "APPLY_NORMAL_HIT_DAMAGE_BONUS",
						"APPLY_SKILL_DEFEND_BONUS", "APPLY_NORMAL_HIT_DEFEND_BONUS", "APPLY_PC_BANG_EXP_BONUS", "APPLY_PC_BANG_DROP_BONUS",
						"APPLY_EXTRACT_HP_PCT", "APPLY_RESIST_WARRIOR", "APPLY_RESIST_ASSASSIN", "APPLY_RESIST_SURA", "APPLY_RESIST_SHAMAN") ;

$immune_types = array("PARA","CURSE","STUN","SLEEP","SLOW","POISON","TERROR") ;

$mob_ranks = array( "PAWN", "S_PAWN", "KNIGHT", "S_KNIGHT", "BOSS", "KING" );
$mob_battle_types = array( "MELEE", "RANGE", "MAGIC", "SPECIAL", "POWER", "TANKER", "SUPER_POWER", "SUPER_TANKER" );
$mob_types = array( "MONSTER", "NPC", "STONE", "WARP", "DOOR", "BUILDING", "PC", "POLYMORPH_PC", "HORSE", "GOTO" );
$mob_onclick_events = array( "NONE", "SHOP", "TALK" );

function get_bit_flag($val, $container)
{
	if (0 == $val)
		return "NONE";

	$pos = 0;
	$ret = "";

	while(true)
	{
		if ($val == 0)
			 break;

		if( ($val & 1) == 1 )
		{
			if( $ret == "" )
			{
				$ret .= $container[$pos];
			}
			else
			{
				$ret .= " | ";
				$ret .= $container[$pos];
			}
		}

		$pos++;
		$val >>= 1;
	}

	return $ret;
}

function get_bit_value($val, $container)
{
	$ret = 0 ;

	for( $i=0 ; $i < sizeof($val) ; $i++ )
	{
		if ("NONE" === $val[$i])
			continue;

		$idx = array_search($val[$i], $container) ;

		if( $idx === FALSE )
		{
			echo "ERROR ".$val[$i]." no matching flag\n";
			continue ;
		}

		$ret += ( 1 << $idx ) ;
	}

	return $ret ;
}


function get_limit_type($val)
{
	global $limit_types ;
	return $limit_types[$val] ;
}

function get_limit_type_value($val)
{
	if( $val[0] == "" ) return 0 ;

	global $limit_types ;
	$idx = array_search($val, $limit_types) ;

	if( $idx === FALSE )
	{
		echo "\n\n\n\nERROR ON get_limit_type_value ".$val."\n\n\n\n" ;
		exit ;
		return 0 ;
	}

	return $idx ;
}

function get_anti_flag($val)
{
	global $antiflag_types ;
	return get_bit_flag($val, $antiflag_types) ;
}

function get_anti_flag_value($val)
{
	if( $val[0] == "" ) return 0 ;

	$arg = split(" \| ", $val) ;

	global $antiflag_types ;
	return get_bit_value($arg, $antiflag_types) ;
}


function get_flag($val)
{
	global $flag_types ;

	return get_bit_flag($val, $flag_types) ;
}

function get_flag_value($val)
{
	if( $val[0] == "" ) return 0 ;

	$arg = split(" \| ", $val) ;

	global $flag_types ;
	return get_bit_value($arg, $flag_types) ;
}


function get_wear_flag($val)
{
	global $wearflag_types ;
	return get_bit_flag($val, $wearflag_types) ;
}

function get_wearflag_value($val)
{
	if( $val[0] == "" ) return 0 ;

	$arg = split(" \| ", $val) ;

	global $wearflag_types ;
	return get_bit_value($arg, $wearflag_types) ;
}


function get_item_type($val)
{
	global $item_types ;
	return $item_types[$val] ;
}

function get_item_type_value($val)
{
	global $item_types ;

	$idx = array_search( $val, $item_types) ;

	if( $idx === FALSE )
	{
		echo "\n\n\n\nERROR ON get_item_type_value ".$val."\n\n\n\n" ;
		exit ;
		return 0 ;
	}

	return $idx ;
}


function get_item_subtype($type, $val)
{
	global $item_subtype ;

	if( $item_subtype[$type] === 0 ) return 0 ;

	if( array_key_exists($val, $item_subtype[$type]) == FALSE )
	{
		if( $type == 16 && $val == 10 ) return "USE_SPECIAL" ;

		echo "\n\n\n\nERROR ON get_item_subtype ".$type." ".$val."\n\n\n\n" ;
		exit ;
		return 0 ;
	}

	return $item_subtype[$type][$val] ;
}

function get_item_subtype_value($type, $val)
{
	global $item_subtype ;

	if( $item_subtype[$type] === 0 ) return 0 ;

	$idx = array_search($val, $item_subtype[$type]) ;

	if( $idx === FALSE )
	{
		if( $type == 16 && $val == "USE_SPECIAL" ) return 10 ;

		echo "\n\n\n\nERROR ON get_item_subtype ".$type." ".$val."\n\n\n\n" ;
		exit ;
		return 0 ;
	}

	return $idx ;
}


function get_immune_flag($val)
{
	global $immune_types ;

	return get_bit_flag($val, $immune_types) ;
}

function get_immune_value($val)
{
	if( $val[0] == "" ) return 0 ;

	$arg = split(" \| ", $val) ;

	global $immune_types ;
	return get_bit_value($arg, $immune_types) ;
}


function get_apply_type($val)
{
	global $apply_types ;
	return $apply_types[$val] ;
}

function get_apply_value($val)
{
	global $apply_types ;
	$idx = array_search($val, $apply_types) ;

	if( $idx === FALSE )
	{
		echo "\n\n\n\nERROR ON get_apply_value ".$val."\n\n\n\n" ;
		exit ;
		return 0 ;
	}

	return $idx ;
}

function get_mob_types_val( $val )
{
    global $mob_types ;
    $idx = array_search($val, $mob_types) ;

    if( $idx === FALSE )
    {
        echo "\n\n\n\nERROR ON get_mob_types_val ".$val."\n\n\n\n" ;
        exit ;
    }

    return $idx ;
}

function get_mob_rank_val( $val )
{
    global $mob_ranks ;
    $idx = array_search($val, $mob_ranks) ;

    if( $idx === FALSE )
    {
        echo "\n\n\n\nERROR ON get_mob_rank_val ".$val."\n\n\n\n" ;
        exit ;
    }

    return $idx ;
}

function get_mob_battle_type_val( $val )
{
    global $mob_battle_types ;
    $idx = array_search($val, $mob_battle_types) ;

    if( $idx === FALSE )
    {
        echo "\n\n\n\nERROR ON get_mob_battle_type_val ".$val."\n\n\n\n" ;
        exit ;
    }

    return $idx ;
}

function get_mob_battle_type_str( $val )
{
    global $mob_battle_types ;
    return $mob_battle_types[ $val ] ;
}

function get_mob_type_str( $val )
{
    global $mob_types ;
    return $mob_types[ $val ] ;
}

function get_mob_rank_str( $val )
{
    global $mob_ranks ;
    return $mob_ranks[ $val ] ;
}

?>

