#!/bin/sh
# This simple script installs the needed parts
# and files to run the server properly.
# Do not change anything here unless you know
# what you are doing.

# Delete db if already present
rm -f db item_proto.txt mob_proto.txt item_names.txt mob_names.txt

# Create the symbolic links
ln -s ../share/conf/item_proto.txt item_proto.txt
ln -s ../share/conf/mob_proto.txt mob_proto.txt
ln -s ../share/conf/item_names.txt item_names.txt
ln -s ../share/conf/mob_names.txt mob_names.txt
ln -s ../share/bin/db db

if [ ! -d log ]; then mkdir log; fi
if [ ! -d cores ]; then mkdir cores; fi

chmod u=rwx,g=r,o=r db
chmod u=rw,g=r,o= *.txt
chmod u=rwx,g=rx,o=rx log
chmod u=rwx,g=rx,o=rx cores
chmod u=r,g=r,o=r log/*/syslog.* >/dev/null 2>&1