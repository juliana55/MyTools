REM **********************************************************
REM ①．現在のSVN情報取得
REM ②．現在Revision番号を探す
REM ③．現在Revision番号をファイル名の後ろに追記
REM ④．実行ファイルコピー
REM ⑤．コピーしたファイルをRev番号追記して履歴に残す
REM **********************************************************
@echo off
REM **********************************************************
REM 可変パス
set searchWD=最終変更リビジョン: 
set changeWD=_Rev
set REVNUM=0
set filename=svninfo.txt
SET CURRPATH=%~dp0
SET WORKPATH=%CURRPATH%..\..\Main\New_master
SET INFOFILE=%CURRPATH%svninfo.txt
SET TMP= %CURRPATH%___tmp.txt
SET LOG= %CURRPATH%___log.txt
REM **********************************************************
REM 固定パス
set MainExePath=%CURRPATH%..\..\Main\New_master\bin\
set SubExePath=%CURRPATH%..\..\Main\New_master\bin\
set PanelLogExe=%CURRPATH%..\branch_mast\PANEL_LOG_VIEWER\bin\
set PanelExe=%CURRPATH%..\\branch_mast\bin\
set MainExeFile=AMS_F_Main_Simulator.exe
set SubExeFile=AMS_F_Sub_Simulator.exe
set PanelLogExeFile=PANEL_LOG_VIEWER.exe
set PanelExeFile=project6_ams_panel.exe


SET HISTPATH=%CURRPATH%Old_NewMaster\
SET HIST=%HISTPATH%_histinfo.txt
set SPLIT=****************************************************************************

color 0A

echo //-------------------------------------------------------------------
echo //	MainSVN情報取得し、Rev番号取得。
echo //-------------------------------------------------------------------
REM pause
echo 処理開始: %DATE% %TIME% >>%LOG%

:GETSVNINFO
type nul > %LOG%
setlocal enabledelayedexpansion
echo 【GETSVNINFO】開始: %DATE% %TIME% >>%LOG%
if exist %WORKPATH% (
	echo 「%WORKPATH%」フォルダは存在する >> %LOG%
	pushd %WORKPATH%
	path C:\Program Files\Subversion\bin
	call svn info > %INFOFILE%
	popd
	echo 「%CURRPATH%」をカレント >> %LOG%
) else (
	echo %WORKPATH%フォルダは存在しない >> %LOG%
	exit
)
endlocal

setlocal enabledelayedexpansion
type nul > %TMP%
rem ファイル内の文字列を1行ずつ読み込む
for /f "tokens=1* delims=: eol=" %%a in ('findstr /N /C:%searchWD% %INFOFILE%') do (
	rem 行番号
	set lineNum=%%a
	rem 中身
	set RevNo=%%b
	
	echo !lineNum! >> %TMP%
	echo !RevNo! >> %TMP%
    if not "!RevNo!" == "" (
     set RevNo=!RevNo:%searchWD%=%changeWD%!
    )
	rem echo !RevNo! >> %TMP%
	set REVNUM=!RevNo!
	rem echo !REVNUM! >> %TMP%
)
rem endlocal
echo 【GETSVNINFO】終了: %DATE% %TIME% >>%LOG%


echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。
echo //-------------------------------------------------------------------
REM pause
REM powershell start-process cmd -verb runas
color 0E

rem setlocal enabledelayedexpansion
:COPYFILE
echo 【COPYFILE】開始: %DATE% %TIME% >>%LOG%

echo F|xcopy %MainExePath%%MainExeFile%  %CURRPATH% /Y >>%LOG%
echo F|xcopy %SubExePath%%SubExeFile%  %CURRPATH% /Y >>%LOG%
echo F|xcopy %PanelLogExe%%PanelLogExeFile%  %CURRPATH% /Y >>%LOG%
REM echo F|xcopy %PanelExe%%PanelExeFile%  %CURRPATH% /Y >>%LOG%

REM 履歴にRev番号付でコピーする
echo F|xcopy %MainExePath%%MainExeFile%  %HISTPATH% /Y >>%LOG%
echo F|xcopy %SubExePath%%SubExeFile%  %HISTPATH% /Y >>%LOG%
rem echo !REVNUM! >> %TMP%
ren %HISTPATH%%MainExeFile% %MainExeFile%%REVNUM% >>%LOG%
ren %HISTPATH%%SubExeFile% %SubExeFile%%REVNUM% >>%LOG%
endlocal

echo 【COPYFILE】終了: %DATE% %TIME% >>%LOG%

echo %SPLIT% >> %HIST%
copy %HIST% + %INFOFILE% %HIST%
copy %HIST% + %LOG% %HIST%
echo %SPLIT% >> %HIST%
del %TMP%
del %INFOFILE%
REM del %LOG%
echo 処理終了: %DATE% %TIME% >>%LOG%
