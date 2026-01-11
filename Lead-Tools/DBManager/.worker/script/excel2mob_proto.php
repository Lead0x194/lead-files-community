<?
include '.worker/lib/flags.php';
include '.worker/lib/sql.php';

$conf = file(".worker/config/$argv[1]/mysql.conf", FILE_IGNORE_NEW_LINES);
$conf = split(" ", $conf[0]);

$db = new mysql( trim($conf[0]), trim($conf[1]), trim($conf[2]), trim($conf[3]), $argv[2]);

$db->query("DELETE FROM mob_proto");

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

	$vnum = $token[0] ;
	$name = $token[1] ;
	$rank = get_mob_rank_val( $token[2] ) ;
	$type = get_mob_types_val( $token[3] ) ;
	$battle_type = get_mob_battle_type_val( $token[4] ) ;

	$level = $token[5];
	$size = $token[6] ? "'$token[6]'" : "''";

	$ai_flag = $token[7];
	$mount_capacity = $token[8];
	$race_flag = $token[9];
	$immune_flag = $token[10];
	$empire = $token[11];
	$folder = $token[12];

	$query = "INSERT INTO `mob_proto` VALUES ($vnum, '$name', 'Noname', $rank, $type, $battle_type, $level, $size, '$ai_flag', ".
		"$mount_capacity, '$race_flag', '$immune_flag', $empire, '$folder'";

	for ($i = 13 ; $i < 71 ; $i++)
	{
		if ($token[$i] == '')
			$token[$i] = 0;

		$query .= ", ".$token[$i];
	}

	$query .= ");";

	if (FALSE === $db->query( $query ))
	{
		echo "ERROR!!! on query\n";
		exit;
	}
}

?>

