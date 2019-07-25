@echo off
rem SVN_revision情報取得
rem ログに残すためのヘッダーファイルを作成
rem SVN環境がない場合は情報無しヘッダーを作成
set basedir=%~dp0
set getrevdir=%~dp0..\..\
set LOG=%basedir%__built__.log
set orgtxt=%basedir%revision_src.txt
set cresrc=%basedir%revision.h

set separate=************

echo %separate%>%LOG%
echo %getrevdir%>>%LOG%

echo %separate%>>%LOG%
setlocal enabledelayedexpansion
SubWCRev %getrevdir% %orgtxt% %cresrc%>>%LOG%

echo %separate%>>%LOG%
echo [!errorlevel!]>>%LOG%
if %errorlevel% neq 0 (
 echo [NG]>>%LOG%

 for /f "delims=" %%a in (%orgtxt%) do (
  set line=%%a
  set line=!line:#if 1=#if 0!
  echo !line! >> %cresrc%
 )
) else (
 echo [OK]>>%LOG%
)
endlocal

echo %separate%>>%LOG%
type %cresrc%>>%LOG%


exit /b
