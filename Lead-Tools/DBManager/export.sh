#!/usr/local/bin/bash

clear

echo ""
echo "Which data do you want to export ?"
echo ""

select type in "Excel" "SQL Data" "Proto for Client" "Names" "SQL Schema" "SQL Patch" "exit"
do
	case $type in
		Excel)
			.worker/DB2Excel.sh ; break ;;

		"SQL Data")
			.worker/DB2SQLData.sh ; break ;;

		"Proto for Client")
			.worker/DB2Proto.sh ; break ;;

		"Names")
			.worker/DB2Names.sh ; break ;;

		"SQL Schema")
			.worker/DB2SQLSchema.sh ; break ;;

		"SQL Patch")
			.worker/MakePatch.sh ; break ;;

		*) exit 0
	esac
done

