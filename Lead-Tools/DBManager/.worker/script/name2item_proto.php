<?
include '.worker/lib/sql.php';

$conf = file(".worker/config/$argv[1]/mysql.conf", FILE_IGNORE_NEW_LINES);
$conf = split(" ", $conf[0]);

$db = new mysql( trim($conf[0]), trim($conf[1]), trim($conf[2]), trim($conf[3]), $argv[2]);

$lines = file($argv[4], FILE_IGNORE_NEW_LINES);

if (FALSE === $lines)
{
	echo "ERROR!!! file loading failed\n";
	exit;
}

foreach ($lines as $line_num => $line)
{
	$line = trim($line);

	$token = split("\t", $line);

	if ("" == $token[0] or "VNUM" == $token[0])
		continue;

	$vnum = $token[0];
	$locale_name = $token[1];

	if (FALSE === $db->query( "UPDATE item_proto SET ".$argv[3]." = \"".$locale_name."\" WHERE vnum = ".$vnum ))
	{
		echo "ERROR!! on query\n";
		exit;
	}
}

?>

