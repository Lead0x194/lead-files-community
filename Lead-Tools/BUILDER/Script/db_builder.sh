#!
REVISION=$1
LOCALE=$2
BRANCH_NAME=$3

cd ~
M2ROOT=~/metin2
if [ "$LOCALE" == "dev" ]; then
	SERVER_PATH=$M2ROOT/dev/Srcs/Server
else
	SERVER_PATH=$M2ROOT/mainline/Srcs/Server
fi

P4PORT="10.1.89.15:1901"; export P4PORT
P4USER=BUILDER;		export P4USER
P4CLIENT=BUILDER-BSD;	export P4CLIENT
P4CHARSET=utf8;			export P4CHARSET
P4ROOT=/home/metin2;	export P4ROOT
P4PASSWD=dlalfmqlfej!;		export P4PASSWD

echo $P4PASSWD | p4 login 2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	exit 1
fi
cd $SERVER_PATH 
if [ ! $? -eq 0 ]; then
	exit 2
fi

p4 revert ... 2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	exit 3
fi

if [ "$REVISION" != "head" ]; then
	p4 sync ...@$REVISION > sync_list.txt 2> ~/ERROR_LOG.txt
else
	p4 sync ...#head > sync_list.txt 2> ~/ERROR_LOG.txt
fi

if [ ! $? -eq 0 ]; then
	exit 4
fi

gmake -j20 > ~/LIB_BUILD_LOG.txt 2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	exit 5
fi

cd db/src
gmake clean
gmake -j20 > ~/DB_BUILD_LOG.txt 2> ~/ERROR_LOG.txt

if [ ! $? -eq 0 ]; then
	exit 6
fi
P4_VERSION=`p4 changes -m1 ../... | cut -d' ' -f2 2> ~/ERROR_LOG.txt`
if [ ! $? -eq 0 ] | [ -z "$P4_VERSION" ]; then
	exit 7
fi

case "$LOCALE" in
ca)
	LOCALE_FULLNAME="canada"
	;;
tw)
	LOCALE_FULLNAME="taiwan"
	;;
br)
	LOCALE_FULLNAME="brazil"
	;;
*)
	LOCALE_FULLNAME=$LOCALE
	;;
esac


p4 sync $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME  2> ~/ERROR_LOG.txt
#p4 revert $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME  2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	exit 8
fi
p4 edit $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME  2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	exit 9
fi
mv ../db_r"$P4_VERSION" $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME  2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	p4 revert $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME	
	exit 10
fi
REV_RES=`p4 revert -a $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME 2> ~/ERROR_LOG.txt`
if [ ! $? -eq 0 ]; then
	p4 revert $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME	
	exit 11
fi
if [ ! -z "$REV_RES" ]; then
	p4 revert $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME	
	exit 12
fi
echo $P4_VERSION > ~/DESCRIPTION
cat ~/DESCRIPTION.txt >> ~/DESCRIPTION
cat ~/DESCRIPTION > ~/DESCRIPTION.txt
DESCRIPTION=`cat ~/DESCRIPTION.txt`
p4 submit -d $DESCRIPTION $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME  2> ~/ERROR_LOG.txt
if [ ! $? -eq 0 ]; then
	p4 revert $M2ROOT/dist/$LOCALE/$BRANCH_NAME/Server/db_$LOCALE_FULLNAME	
	exit 13
fi
exit 0
