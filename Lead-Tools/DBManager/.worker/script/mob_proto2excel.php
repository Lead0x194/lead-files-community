<?
include '.worker/lib/sql.php';
include '.worker/lib/flags.php';

if ($argc < 3)
{
	echo "Usage: mob_proto2excel.php <locale> <charset>";
	exit;
}

$fp = fopen(".worker/config/$argv[1]/mysql.conf", 'r');

$line = fgets($fp);

fclose($fp);

$conf = split(" ", $line);

$db = new mysql( trim($conf[0]),  trim($conf[1]),  trim($conf[2]),  trim($conf[3]), $argv[2]);

$db->query("SELECT * FROM mob_proto ORDER BY vnum");

echo "VNUM\t" ;
echo "NAME\t" ;
echo "RANK\t" ;
echo "TYPE\t" ;
echo "BATTLE_TYPE\t" ;
echo "LEVEL\t" ;
echo "SIZE\t" ;
echo "AI_FLAG\t" ;
echo "MOUNT_CAPACITY\t" ;
echo "RACE_FLAG\t" ;
echo "IMMUNE_FLAG\t" ;
echo "EMPIRE\t" ;
echo "FOLDER\t" ;
echo "ON_CLICK\t" ;
echo "ST\t" ;
echo "DX\t" ;
echo "HT\t" ;
echo "IQ\t" ;
echo "DAMAGE_MIN\t" ;
echo "DAMAGE_MAX\t" ;
echo "MAX_HP\t" ;
echo "REGEN_CYCLE\t" ;
echo "REGEN_PERCENT\t" ;
echo "GOLD_MIN\t" ;
echo "GOLD_MAX\t" ;
echo "EXP\t" ;
echo "DEF\t" ;
echo "ATTACK_SPEED\t" ;
echo "MOVE_SPEED\t" ;
echo "AGGRESSIVE_HP_PCT\t" ;
echo "AGGRESSIVE_SIGHT\t" ;
echo "ATTACK_RANGE\t" ;
echo "DROP_ITEM\t" ;
echo "RESURRECTION_VNUM\t" ;
echo "ENCHANT_CURSE\t" ;
echo "ENCHANT_SLOW\t" ;
echo "ENCHANT_POISON\t" ;
echo "ENCHANT_STUN\t" ;
echo "ENCHANT_CRITICAL\t" ;
echo "ENCHANT_PENETRATE\t" ;
echo "RESIST_SWORD\t" ;
echo "RESIST_TWOHAND\t" ;
echo "RESIST_DAGGER\t" ;
echo "RESIST_BELL\t" ;
echo "RESIST_FAN\t" ;
echo "RESIST_BOW\t" ;
echo "RESIST_FIRE\t" ;
echo "RESIST_ELECT\t" ;
echo "RESIST_MAGIC\t" ;
echo "RESIST_WIND\t" ;
echo "RESIST_POISON\t" ;
echo "DAM_MULTIPLY\t" ;
echo "SUMMON\t" ;
echo "DRAIN_SP\t" ;
echo "MOB_COLOR\t" ;
echo "POLYMORPH_ITEM\t" ;
echo "SKILL_LEVEL0\t" ;
echo "SKILL_VNUM0\t" ;
echo "SKILL_LEVEL1\t" ;
echo "SKILL_VNUM1\t" ;
echo "SKILL_LEVEL2\t" ;
echo "SKILL_VNUM2\t" ;
echo "SKILL_LEVEL3\t" ;
echo "SKILL_VNUM3\t" ;
echo "SKILL_LEVEL4\t" ;
echo "SKILL_VNUM4\t" ;
echo "SP_BERSERK\t" ;
echo "SP_STONESKIN\t" ;
echo "SP_GODSPEED\t" ;
echo "SP_DEATHBLOW\t" ;
echo "SP_REVIVE" ;
echo "\n";

while ($row = $db->fetch_row())
{
	echo "$row[0]\t";
	echo "$row[1]\t";

	echo get_mob_rank_str($row[3])."\t";
	echo get_mob_type_str($row[4])."\t";
	echo get_mob_battle_type_str($row[5])."\t";

	for ($i = 6; $i < 71; $i++)
		echo "$row[$i]\t";

	echo "$row[71]\n";
}

$db->destroy();

?>

