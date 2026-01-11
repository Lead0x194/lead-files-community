#!/usr/local/bin/bash

source .worker/lib/utilities.bash

clear

echo ""
echo "Select locale for Patch"
echo ""

select_locale

clear

echo ""
echo "Select table for Patch"
echo ""

select table in "item_proto" "mob_proto" "exit"
do
	case $table in
		item_proto)
			php .worker/script/excel2item_proto.php $locale $char_set "data/$locale/item_proto.txt";

			if [ "ymir" != $locale -a "korea" != $locale ]; then
				php .worker/script/name2item_proto.php $locale $char_set $name_column_name "data/$locale/item_names.txt";
			fi

			php .worker/script/table2sql.php $locale $char_set item_proto;
			cat schema/$locale/item_proto.sql > `date "+%Y%m%d"`_$locale"_item_proto.sql";
			cat data/$locale/item_proto.sql >> `date "+%Y%m%d"`_$locale"_item_proto.sql";
			break;;

		mob_proto)
			php .worker/script/excel2mob_proto.php $locale $char_set "data/$locale/mob_proto.txt";

			if [ "ymir" != $locale -a "korea" != $locale -a "we_korea_test" != $locale -a "we_korea" != $locale ]; then
				php .worker/script/name2mob_proto.php $locale $char_set $name_column_name "data/$locale/mob_names.txt";
			fi

			php .worker/script/table2sql.php $locale $char_set mob_proto;
			cat schema/$locale/mob_proto.sql > `date "+%Y%m%d"`_$locale"_mob_proto.sql";
			cat data/$locale/mob_proto.sql >> `date "+%Y%m%d"`_$locale"_mob_proto.sql";
			break;;

		*) exit 0;
	esac
done

