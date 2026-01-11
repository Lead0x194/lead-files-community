#!/bin/sh
# This script erases all lofiles created by the DBCache server

rm -fr log/*

if [ -r autorun.log ]; then rm autorun.log; fi
if [ -r PTS ]; then rm PTS; fi
if [ -r syslog ]; then rm syslog; fi
if [ -r syserr ]; then rm syserr; fi

if [ -r db.core ]; then rm db.core; fi
if [ -r VERSION.txt ]; then rm VERSION.txt; fi
# if [ -r usage.txt ]; then rm usage.txt; fi

# Remove files which are indicating an improper shutdown
if [ -r pid.db ]; then rm pid.db; fi
if [ -r pid ]; then rm pid; fi