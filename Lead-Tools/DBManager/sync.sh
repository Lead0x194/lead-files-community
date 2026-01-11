#!/usr/local/bin/bash

source .worker/lib/utilities.bash

select_locale

echo "sync item_proto to DB..."
php .worker/script/excel2item_proto.php $locale $char_set data/$locale/item_proto.txt

echo "sync mob_proto to DB..."
php .worker/script/excel2mob_proto.php $locale $char_set data/$locale/mob_proto.txt

if [ "ymir" != $locale -a "korea" != $locale ]; then
	php .worker/script/name2item_proto.php $locale $char_set $name_column_name data/$locale/item_names.txt
	php .worker/script/name2mob_proto.php $locale $char_set $name_column_name data/$locale/mob_names.txt
fi


