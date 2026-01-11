#!/usr/local/bin/bash

source .worker/lib/utilities.bash

clear

echo ""
echo "Select locale for Excel import"
echo ""

select_locale

clear

echo ""
echo "Which table do you want to import"
echo ""

select table in "item_proto" "mob_proto" "exit"
do
	case $table in
		item_proto)
			php .worker/script/excel2item_proto.php $locale $char_set $1; break ;;
		mob_proto)
			php .worker/script/excel2mob_proto.php $locale $char_set $1; break ;;

		*) exit 0;
	esac
done

