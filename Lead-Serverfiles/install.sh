#!/bin/sh
# Installs the needed parts
# (Executes child scripts)

# if [ `id -u` = "0" ]; then
# 	echo "This script must not be run as root" 1>&2
# 	exit 1
# fi

ROOT=$PWD

if [ ! -f "$ROOT/conf/GENERAL_CONFIG" ]; then
	echo "Missing shared configuration: $ROOT/conf/GENERAL_CONFIG" >&2
	exit 1
fi

chmod u=rwx,g=r,o=r $ROOT/share/bin/game
chmod u=rwx,g=r,o=r $ROOT/share/bin/db

find . -type d -exec chmod u=rwx,g=rx,o=rx {} \;
find . -name "*.sh" -exec chmod u=rwx,g=r,o=r {} \;

# Install db
cd $ROOT/db && sh install.sh

# Install auth
cd $ROOT/auth && sh install.sh

# Install channel1
cd $ROOT/channel1/game1 && sh install.sh
cd $ROOT/channel1/game2 && sh install.sh

# Install channel2
cd $ROOT/channel2/game1 && sh install.sh
cd $ROOT/channel2/game2 && sh install.sh

# Install channel3
cd $ROOT/channel3/game1 && sh install.sh
cd $ROOT/channel3/game2 && sh install.sh

# Install channel4
cd $ROOT/channel4/game1 && sh install.sh
cd $ROOT/channel4/game2 && sh install.sh

# Install channel99
cd $ROOT/channel99 && sh install.sh
