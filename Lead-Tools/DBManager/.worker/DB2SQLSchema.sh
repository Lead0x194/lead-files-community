
#!/usr/local/bin/bash

source .worker/lib/utilities.bash

clear

echo ""
echo "Select locale for SQL Schema export"
echo ""

select_locale

php .worker/script/schema2sql.php $locale $char_set item_proto;
php .worker/script/schema2sql.php $locale $char_set mob_proto;
php .worker/script/schema2sql.php $locale $char_set refine_proto;
php .worker/script/schema2sql.php $locale $char_set skill_proto;


