#!/usr/local/bin/bash

source .worker/lib/utilities.bash

DUMP_UTIL=.worker/bin/dump_proto.exe

clear

select_locale

$DUMP_UTIL .worker/config/$locale/mysql.conf $name_column_name `date +%Y%m%d`_${locale}_

