#!/bin/sh
# Shuts each process down
# (Executes child scripts)

# if [ `id -u` = "0" ]; then
# 	echo -e "\033[31mThis script must not be run as root..\033[0m" 1>&2
# 	exit 1
# fi

ROOT=$PWD

# Shutdown auth
echo -e "\033[31mStopping auth..\033[0m"
cd $ROOT/auth && sh shut.sh

# Shutdown channel1
echo -e "\033[31mStopping channel1/first..\033[0m"
cd $ROOT/channel1/first && sh shut.sh
echo -e "\033[31mStopping channel1/game2..\033[0m"
cd $ROOT/channel1/game2 && sh shut.sh
echo -e "\033[31mStopping channel1/game1..\033[0m"
cd $ROOT/channel1/game1 && sh shut.sh

# Shutdown channel2
echo -e "\033[31mStopping channel2/first..\033[0m"
cd $ROOT/channel2/first && sh shut.sh
echo -e "\033[31mStopping channel2/game2..\033[0m"
cd $ROOT/channel2/game2 && sh shut.sh
echo -e "\033[31mStopping channel2/game1..\033[0m"
cd $ROOT/channel2/game1 && sh shut.sh

# Shutdown channel3
echo -e "\033[31mStopping channel3/first..\033[0m"
cd $ROOT/channel3/first && sh shut.sh
echo -e "\033[31mStopping channel3/game2..\033[0m"
cd $ROOT/channel3/game2 && sh shut.sh
echo -e "\033[31mStopping channel3/game1..\033[0m"
cd $ROOT/channel3/game1 && sh shut.sh

# Shutdown channel4
echo -e "\033[31mStopping channel4/first..\033[0m"
cd $ROOT/channel4/first && sh shut.sh
echo -e "\033[31mStopping channel4/game2..\033[0m"
cd $ROOT/channel4/game2 && sh shut.sh
echo -e "\033[31mStopping channel4/game1..\033[0m"
cd $ROOT/channel4/game1 && sh shut.sh

# Shutdown game99
echo -e "\033[31mStopping game99..\033[0m"
cd $ROOT/game99 && sh shut.sh

# Shutdown db
echo -e "\033[31mStopping db..\033[0m"
cd $ROOT/db && sh shut.sh
sleep 2
