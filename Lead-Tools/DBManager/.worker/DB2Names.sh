#!/usr/local/bin/bash

clear

echo ""
echo "Select locale for Names export"
echo ""

DBName=""
CharSet="utf8"

select locale in "ymir" "korea" "japan" "brazil" "newcibn" "cibn10" "canada" "singapore" "we_korea" "we_korea_test" "taiwan" "exit"
do
	case $locale in
		we_korea_test)
			CharSet="euckr"	;	break	;;
		we_korea)
			CharSet="euckr"	;	break	;;
		ymir)
			CharSet="euckr"	;	break	;;
		korea)
			CharSet="euckr"	;	break	;;
		japan)
			CharSet="sjis"	;	break	;;
		brazil)
			CharSet="latin1"	;	break	;;
		newcibn)
			CharSet="gb2312"	;	break	;;
		cibn10)
			CharSet="gb2312"	;	break	;;
		taiwan)
			CharSet="big5"	;	break	;;
		canada)
			CharSet="latin1"	;	break	;;
		singapore)
			CharSet="latin1"	;	break	;;

		*) exit 0
	esac
done

clear

echo ""
echo "Which table do you want to export from $DBName"
echo ""

select table in "item_proto" "mob_proto" "exit"
do
	case $table in
		item_proto)
			php .worker/script/table2names.php $locale $table $CharSet > data/$locale/item_names.txt ; exit 0 ;;
		mob_proto)
			php .worker/script/table2names.php $locale $table $CharSet > data/$locale/mob_names.txt ; exit 0 ;;

		*) exit 0
	esac
done

