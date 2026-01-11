#!/usr/local/bin/bash

source .worker/lib/utilities.bash

clear

echo ""
echo "Select locale for Excel export"
echo ""

select_locale

clear

echo ""
echo "Which table do you want to export"
echo ""

select table in "item_proto" "mob_proto" "exit"
do
	case $table in
		item_proto)
			php .worker/script/item_proto2excel.php $locale $char_set > data/$locale/item_proto.txt ; exit 0 ;;
		mob_proto)
			php .worker/script/mob_proto2excel.php $locale $char_set > data/$locale/mob_proto.txt ; exit 0 ;;

		*) exit 0
	esac
done

