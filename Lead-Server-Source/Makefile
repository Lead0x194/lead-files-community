CC=gcc
CXX=g++

PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)

.PHONY: default libthecore libpoly libgame liblua libsql game db all

default: game db
	@echo "--------------------------------------"
	@echo "Build Done"
	@echo "--------------------------------------"

liblua: .
	$(MAKE) -C $@ clean
	$(MAKE) -C $@

libsql: .
	$(MAKE) -C $@ clean
	$(MAKE) -C $@

libgame: .
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

libpoly: .
	$(MAKE) -C $@ clean
	$(MAKE) -C $@ 

libthecore: .
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

game: libthecore libpoly libgame liblua libsql
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

db: libthecore libpoly libgame libsql
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

all: 
	@echo "--------------------------------------"
	@echo "Full Build Start"
	@echo "--------------------------------------"

	$(MAKE) -C liblua clean
	$(MAKE) -C liblua

	$(MAKE) -C libsql clean
	$(MAKE) -C libsql

	$(MAKE) -C libgame/src clean
	$(MAKE) -C libgame/src

	$(MAKE) -C libpoly clean
	$(MAKE) -C libpoly 

	$(MAKE) -C libthecore/src clean
	$(MAKE) -C libthecore/src

	$(MAKE) -C game/src clean
	$(MAKE) -C game/src limit_time

	$(MAKE) -C db/src clean

	@echo "--------------------------------------"
	@echo "Full Build End"
	@echo "--------------------------------------"
