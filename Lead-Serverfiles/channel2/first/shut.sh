#!/bin/sh
if [ -r ./pid.d ]; then
  kill -s TERM `cat ./pid.d`
elif [ -r ./pid ]; then
  kill -1 `cat ./pid`
else
  echo 'server is not running.';
fi
