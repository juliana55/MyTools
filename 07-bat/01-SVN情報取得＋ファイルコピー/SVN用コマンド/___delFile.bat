@echo off
rem ビルド実施後ヘッダーファイルを削除する
set basedir=%~dp0
set LOG=%basedir%__built__.log
set cresrc=%basedir%revision.h
set errdumy=%basedir%\revision_dumy.txt
set DATETIME=%date:~0,4%/%date:~5,2%/%date:~8,2%-%time:~0,2%:%time:~3,2%:%time:~6,2%

set separate=************

echo %separate%>>%LOG%
echo STARTdel=%DATETIME%>>%LOG%

del %cresrc%>>%LOG%
echo delete[%cresrc%]>>%LOG%

echo ENDdel=%DATETIME%>>%LOG%
@echo off

exit
