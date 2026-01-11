<?
include '.worker/lib/sql.php';
include '.worker/lib/flags.php';

if ($argc < 3)
{
	echo "Usage: table2names.php <locale> <table> <charset>";
	exit;
}

if ($argv[1] == "ymir" or $argv[1] == "korea")
{
	exit;
}

$fp = fopen(".worker/config/$argv[1]/mysql.conf", 'r');
$line = fgets($fp);
fclose($fp);

$conf = split(" ", $line);

$host = trim($conf[0]);
$id = trim($conf[1]);
$pass = trim($conf[2]);
$db = trim($conf[3]);

$name_table_map = array(
						"brazil" => "locale_name",
						"canada" => "gb2312name",
						"japan" => "locale_name",
						"cibn10" => "locale_name",
						"newcibn" => "gb2312name",
						"singapore" => "locale_name",
						"taiwan" => "locale_name",
					);

$db = new mysql( trim($conf[0]), trim($conf[1]), trim($conf[2]), trim($conf[3]), $argv[3]);

$db->query("SELECT vnum, ".$name_table_map[$argv[1]]." FROM ".$argv[2]." ORDER BY vnum");


echo "VNUM\tLOCALE_NAME\n";

while ($row = $db->fetch_row())
{
	echo $row[0]."\t".$row[1]."\n";
}

?>

