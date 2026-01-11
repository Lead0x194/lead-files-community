<?
include '.worker/lib/flags.php';
include '.worker/lib/sql.php';

$conf = file(".worker/config/$argv[1]/mysql.conf", FILE_IGNORE_NEW_LINES);
$conf = split(" ", $conf[0]);

$db = new mysql( trim($conf[0]), trim($conf[1]), trim($conf[2]), trim($conf[3]), $argv[2]);

$db->query("DELETE FROM item_proto");

$lines = file($argv[3], FILE_IGNORE_NEW_LINES);

if (FALSE === $lines)
{
	echo "ERROR!!! file loading failed\n";
	exit;
}

foreach ($lines as $line_num => $line)
{
	if (2 == $line_num)
		continue;

	$line = trim($line);

	$token = split("\t", $line);

	if ("" == $token[0])
		continue;

	$type = get_item_type_value($token[2]);
	$subtype = get_item_subtype_value($type, $token[3]);
	$antiflag = get_anti_flag_value(trim($token[5]));
	$flag = get_flag_value($token[6]);
	$wearflag = get_wearflag_value($token[7]);
	$immune = get_immune_value($token[8]);

	$limit_type0 = get_limit_type_value($token[14]);
	$limit_type1 = get_limit_type_value($token[16]);

	$apply_type0 = get_apply_value($token[18]);
	$apply_type1 = get_apply_value($token[20]);
	$apply_type2 = get_apply_value($token[22]);

	$query = "INSERT INTO item_proto( vnum, name, type, subtype, size, antiflag, flag, wearflag, immuneflag,".
				"gold, shop_buy_price, refined_vnum, refine_set, magic_pct, limittype0, limitvalue0,".
				"limittype1, limitvalue1, applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2,".
				"value0, value1, value2, value3, value4, value5, specular, socket_pct, addon_type )".
				"VALUES".
				"(".
					"$token[0], '$token[1]', $type, $subtype, $token[4], $antiflag, $flag, $wearflag, $immune,".
					"$token[9], $token[10], $token[11], $token[12], $token[13], $limit_type0, $token[15],".
					"$limit_type1, $token[17], $apply_type0, $token[19], $apply_type1, $token[21], $apply_type2, $token[23],".
					"$token[24], $token[25], $token[26], $token[27], $token[28], $token[29], $token[30], $token[31], $token[32]);";

	if (FALSE === $db->query( $query ))
	{
		echo "ERROR!!! on query\n";
		exit;
	}
}

?>

