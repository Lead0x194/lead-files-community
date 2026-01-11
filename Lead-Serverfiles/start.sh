#!/bin/sh
# This script is only here for convience
# It calls all other autorun shellscripts in the
# various subdirectories.

# if [ `id -u` = "0" ]; then
# 	echo -e "\033[31mThis script must not be run as root..\033[0m" 1>&2
# 	exit 1
# fi

ROOT=$PWD

db_start() # Run db
{
	echo -e "\033[31mStarting db..\033[0m"
	cd $ROOT/db && sh run.sh
	sleep 2
	clear
}

auth_start() # Run auth
{
	echo -e "\033[31mStarting auth..\033[0m"
	cd $ROOT/auth && sh run.sh
	sleep 2
	clear
}

ch1_start() # Run channel1
{
	echo -e "\033[31mStarting channel1/game2..\033[0m"
	cd $ROOT/channel1/game2 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel1/game1..\033[0m"
	cd $ROOT/channel1/game1 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel1/first..\033[0m"
	cd $ROOT/channel1/first && sh run.sh
	sleep 2
	clear
}

ch2_start() # Run channel2
{
	echo -e "\033[31mStarting channel2/game2..\033[0m"
	cd $ROOT/channel2/game2 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel2/game1..\033[0m"
	cd $ROOT/channel2/game1 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel2/first..\033[0m"
	cd $ROOT/channel2/first && sh run.sh
	sleep 2
	clear
}

ch3_start() # Run channel3
{
	echo -e "\033[31mStarting channel3/game2..\033[0m"
	cd $ROOT/channel3/game2 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel3/game1..\033[0m"
	cd $ROOT/channel3/game1 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel3/first..\033[0m"
	cd $ROOT/channel3/first && sh run.sh
	sleep 2
	clear
}

ch4_start() # Run channel4
{
	echo -e "\033[31mStarting channel4/game2..\033[0m"
	cd $ROOT/channel4/game2 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel4/game1..\033[0m"
	cd $ROOT/channel4/game1 && sh run.sh
	sleep 2
	clear
	echo -e "\033[31mStarting channel3/first..\033[0m"
	cd $ROOT/channel4/first && sh run.sh
	sleep 2
	clear
}

game99_start() # Run game99
{
	echo -e "\033[31mStarting game99..\033[0m"
	cd $ROOT/game99 && sh run.sh
	sleep 2
	clear
}

echo -e "\033[31m  
How many channels do you start?? \n 
1 - 1CH\n 
2 - 2CH\n 
3 - 3CH\n
4 - 4CH\033[0m"

read chs

case $chs in 
1*) 

	db_start
	auth_start
	ch1_start
	game99_start
	echo -e "\033[32m \n Server started! CH1\033[0m"

;;
2*)

	db_start
	auth_start
	ch1_start
	ch2_start
	game99_start
	echo -e "\033[32m \n Server started! CH1-2\033[0m"

;;
3*)
	db_start
	auth_start
	ch1_start
	ch2_start
	ch3_start
	game99_start
	echo -e "\033[32m \n Server started! CH1-2-3\033[0m"
;;
4*)
	db_start
	auth_start
	ch1_start
	ch2_start
	ch3_start
	ch4_start
	game99_start
	echo -e "\033[32m \n Server started! CH1-2-3-4\033[0m"
;;
esac
