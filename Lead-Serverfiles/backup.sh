#!/bin/sh

backup_metin2()
{
	echo -e "\033[31mBackup started...\033[0m"
	cd /usr
	tar -zcf metin2_server+src.tar.gz metin2
	sleep 2
	echo -e "\033[32m/usr/metin2_server+src.tar.gz created \033[0m"
}

backup_database()
{
	echo -e "\033[31mBackup started...\033[0m"
	cd /var/db
	tar -zcf metin2_mysql.tar.gz mysql
	sleep 2
	echo -e "\033[32m/var/db/metin2_mysql.tar.gz created \033[0m"
}

echo -e "\033[31m  
What do you want to back up? \n 
1 - /usr/metin2 \n 
2 - /var/db/mysql \n 
3 - Both \033[0m"

read chs

case $chs in 

1*) 
	backup_metin2
;;

2*) 
	backup_database
;;

3*) 
	backup_metin2
	backup_database
;;

esac
