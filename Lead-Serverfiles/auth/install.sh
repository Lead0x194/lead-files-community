#!/bin/sh
# This simple script installs the needed parts
# and files to run the server properly.
# Do not change anything here unless you know
# what you are doing.

# Delete the files if they're already present
rm -f auth data locale package

# Create the symbolic links
ln -s ../share/bin/game auth
ln -s ../share/data data
ln -s ../share/locale locale
ln -s ../share/package package

if [ ! -d log ]; then mkdir log; fi
if [ ! -d cores ]; then mkdir cores; fi

chmod u=rwx,g=r,o=r auth
chmod u=rw,g=r,o= CONFIG
chmod u=rwx,g=rx,o=rx log
chmod u=rwx,g=rx,o=rx cores
chmod u=r,g=r,o=r log/*/syslog.* >/dev/null 2>&1