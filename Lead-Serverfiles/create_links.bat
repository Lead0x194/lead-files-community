@echo off
for %%x in (db, auth, channel1/game1, channel1/game2, channel99) do (
	@rmdir /q "%~dp0%%x\data" >nul 2>&1
	@del /Q "%~dp0%%x\data" >nul 2>&1
	@mklink /D "%~dp0%%x\data" "%~dp0share\data"

	@rmdir /q "%~dp0%%x\common" >nul 2>&1
	@del /Q "%~dp0%%x\common" >nul 2>&1
	@mklink /D "%~dp0%%x\common" "%~dp0share\common"

	@rmdir /q "%~dp0%%x\conf" >nul 2>&1
	@del /Q "%~dp0%%x\conf" >nul 2>&1
	@mklink /D "%~dp0%%x\conf" "%~dp0conf"
)