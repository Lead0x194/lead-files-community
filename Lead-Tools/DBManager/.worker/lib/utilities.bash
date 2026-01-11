
select_locale()
{
	select locale in "ymir" "korea" "japan" "brazil" "newcibn" "cibn10" "canada" "singapore" "we_korea" "we_korea_test" "taiwan" "exit" 
	do
		case $locale in
			ymir)
				char_set="euckr";
				name_column_name="name";
				break;;
			korea)
				char_set="euckr";
				name_column_name="name";
				break;;
			japan)
				char_set="sjis";
				name_column_name="locale_name";
				break;;
			brazil)
				char_set="latin1";
				name_column_name="locale_name";
				break;;
			newcibn)
				char_set="gb2312";
				name_column_name="gb2312name";
				break;;
			cibn10)
				char_set="gb2312";
				name_column_name="locale_name";
				break;;
			canada)
				char_set="latin1";
				name_column_name="gb2312name";
				break;;
			singapore)
				char_set="latin1";
				name_column_name="locale_name";
				break;;
			we_korea)
				char_set="euckr";
				name_column_name="name";
				break;;
			we_korea_test)
				char_set="euckr";
				name_column_name="name";
				break;;
			taiwan)
				char_set="big5";
				name_column_name="locale_name";
				break;;

			*) exit 0
		esac
	done
}

