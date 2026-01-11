<?
include '.worker/lib/sql.php';
include '.worker/lib/flags.php';

$fp = fopen(".worker/config/$argv[1]/mysql.conf", 'r');
$line = fgets($fp);
fclose($fp);

$conf = split(" ", $line);

$host = trim($conf[0]);
$id = trim($conf[1]);
$pass = trim($conf[2]);
$db = trim($conf[3]);

$opt = "--no-create-info --order-by-primary --extended-insert=false --dump-date=false --default-character-set=$argv[2]";

exec("mysqldump -h$host -u$id -p$pass $opt $db $argv[3] > data/$argv[1]/$argv[3].sql");

?>

