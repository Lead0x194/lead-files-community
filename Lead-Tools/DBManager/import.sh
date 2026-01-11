#!/usr/local/bin/bash

if [ $# -lt 1 ]
then
	echo "$0 <excel file>";
	exit 0;
fi

if [ ${1: -4} != ".txt" ]
then
	echo "Please use Excel file only";
	exit 0;
fi

clear

.worker/Excel2DB.sh $1;

