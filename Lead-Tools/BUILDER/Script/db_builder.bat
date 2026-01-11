set PATH=%DIR_TOOLS%;%PATH%
plink -pw %DEV_PASSWD% %DEV_USER%@%BUILD_SERVER% "./db_builder.sh %REVISION% %LOCALE% %BRANCH_NAME%"
IF ERRORLEVEL 13 (
	echo "서밋 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 12 (
	echo "변경되지 않은 db_%LOCALE%" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 11 (
	echo "db_%LOCALE% 변경 체크 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 10 (
	echo "db_%LOCALE% 덮어쓰기" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 9	(
	echo "db_%LOCALE% check out 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 8	(
	echo "db_%LOCALE% revert 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 7	(
	echo "정확한 리비전 넘버 가져오기 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 6	(
	echo "디비 바이너리 컴파일 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 5	(
	echo "라이브러리 컴파일 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 4	(
	echo "리비전 %REVISION% 소스 파일 업데이트 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 3	(
	echo "서버 소스 revert 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 2	(
	echo "유효하지 않은 경로" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 1	(
	echo "로그인 실패" > ERROR.txt
	goto error_state
)
IF ERRORLEVEL 0 exit 0

:error_state

echo "---------------------------------" >> ERROR.txt
echo "            error log            " >> ERROR.txt
echo "---------------------------------" >> ERROR.txt

del ERROR_LOG.txt
set PATH=%DIR_WINSCP%;%PATH%
%DIR_WinScp%winscp %DEV_USER%:%DEV_PASSWD%@%BUILD_SERVER% /script=get_error_log.txt

type ERROR_LOG.txt >> ERROR.txt

exit 1