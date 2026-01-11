#!/usr/local/bin/bash

source .worker/lib/utilities.bash

clear

echo ""
echo "Select locale for SQL Data export"
echo ""

select_locale

clear

echo ""
echo "Which table do you want to export"
echo ""

select table in "item_proto" "mob_proto" "refine_proto" "skill_proto" "exit"
do
	case $table in
		item_proto)
			php .worker/script/table2sql.php $locale $char_set item_proto ; exit 0 ;;
		mob_proto)
			php .worker/script/table2sql.php $locale $char_set mob_proto ; exit 0 ;;
		skill_proto)
			php .worker/script/table2sql.php $locale $char_set skill_proto ; exit 0 ;;
		refine_proto)
			php .worker/script/table2sql.php $locale $char_set refine_proto ; exit 0 ;;
	esac
done

