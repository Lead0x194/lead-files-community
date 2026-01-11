pushd %~dp0
mkdir auth
mkdir game\log
mkdir auth\log
mkdir db\log
mkdir game\locale
copy libdevil\win32\bin\* game
copy libdevil\win32\bin\* auth
copy game\*.exe auth
cd db
mklink item_proto.txt ..\..\..\..\dist\dev\Test\Server\item_proto.txt
mklink mob_proto.txt ..\..\..\..\dist\dev\Test\Server\mob_proto.txt
mklink item_proto_test.txt ..\..\..\..\dist\dev\Test\Server\item_proto_test.txt
mklink mob_proto_test.txt ..\..\..\..\dist\dev\Test\Server\mob_proto_test.txt
cd ..
cd auth
mklink /D package ..\..\..\..\dist\dev\Test\Server\package
mklink /D panama ..\..\..\..\dist\dev\Test\Server\panama
cd ..
cd game
mklink /D data ..\..\..\..\dist\dev\Test\Server\data
mklink /D package ..\..\..\..\dist\dev\Test\Server\package
mklink /D panama ..\..\..\..\dist\dev\Test\Server\panama
cd locale
mklink /D ymir ..\..\..\..\..\dist\dev\Test\Server\locale\ymir
mklink /D canada ..\..\..\..\..\dist\ca\Test\Server\locale\canada
mklink /D brazil ..\..\..\..\..\dist\br\Test\Server\locale\brazil
mklink /D taiwan ..\..\..\..\..\dist\tw\Test\Server\locale\taiwan
mklink /D NEWCIBN ..\..\..\..\..\dist\NEWCIBN\Test\Server\locale\NEWCIBN
mklink /D we_korea ..\..\..\..\..\dist\we_korea\Test\Server\locale\we_korea
cd ..
cd ..
pause
