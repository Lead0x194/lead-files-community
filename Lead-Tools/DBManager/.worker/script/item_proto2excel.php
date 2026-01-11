<?
include '.worker/lib/sql.php';
include '.worker/lib/flags.php';

if( $argc < 3 )
{
	echo "Usage: item_proto2execl.php <locale> <charset>";
	exit;
}

$fp = fopen(".worker/config/$argv[1]/mysql.conf", 'r');

$line = fgets($fp);

fclose($fp);

$conf = split(" ", $line);

$db = new mysql( trim($conf[0]), trim($conf[1]), trim($conf[2]), trim($conf[3]), $argv[2]);

$db->query("SELECT * FROM item_proto ORDER BY vnum");

echo "ITEM번호\t";
echo "ITEM_NAME(K)\t";
echo "ITEM_TYPE\t";
echo "SUB_TYPE\t";
echo "SIZE\t";
echo "ANTI_FLAG\t";
echo "FLAG\t";
echo "ITEM_WEAR\t";
echo "IMMUNE\t";
echo "상점 판매가\t";
echo "상점에서 살 때\t";
echo "REFINE\t";
echo "REFINESET\t";
echo "MAGIC_PCT\t";
echo "LIMIT_TYPE0\t";
echo "LIMIT_VALUE0\t";
echo "LIMIT_TYPE1\t";
echo "LIMIT_VALUE1\t";
echo "ADDON_TYPE0\t";
echo "ADDON_VALUE0\t";
echo "ADDON_TYPE1\t";
echo "ADDON_VALUE1\t";
echo "ADDON_TYPE2\t";
echo "ADDON_VALUE2\t";
echo "VALUE0\t";
echo "VALUE1\t";
echo "VALUE2\t";
echo "VALUE3\t";
echo "VALUE4\t";
echo "VALUE5\t";
echo "Specular\t";
echo "SOCKET\t";
echo "ATTU_ADDON\n" ;

while( $row=$db->fetch_row() )
{
	echo "$row[0]\t" ;
	echo "$row[1]\t" ;
	echo get_item_type($row[3])."\t" ;
	echo get_item_subtype($row[3], $row[4])."\t" ;
	echo "$row[6]\t" ;
	echo get_anti_flag($row[7])."\t" ;
	echo get_flag($row[8])."\t" ;
	echo get_wear_flag($row[9])."\t" ;
	echo get_immune_flag($row[10])."\t" ;
	echo "$row[11]\t" ;
	echo "$row[12]\t" ;
	echo $row[13]."\t" ;
	echo $row[14]."\t" ;
	echo $row[16]."\t" ;
	echo get_limit_type($row[17])."\t" ;
	echo "$row[18]\t" ;
	echo get_limit_type($row[19])."\t" ;
	echo "$row[20]\t" ;
	echo get_apply_type($row[21])."\t" ;
	echo $row[22]."\t" ;
	echo get_apply_type($row[23])."\t" ;
	echo $row[24]."\t" ;
	echo get_apply_type($row[25])."\t" ;
	echo $row[26]."\t" ;
	echo $row[27]."\t" ;
	echo $row[28]."\t" ;
	echo $row[29]."\t" ;
	echo $row[30]."\t" ;
	echo $row[31]."\t" ;
	echo $row[32]."\t" ;
	echo $row[39]."\t" ;
	echo $row[40]."\t" ;
	echo $row[41] ;

	echo "\n" ;
}

$db->destroy();

?>

