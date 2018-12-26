@echo off
REM *************************************************************************************
REM ①作業・格納フォルダを指定する。
REM ②作業・格納フォルダを作成(指定パスまたは、指定パスがない場合はデフォルトパス使用。
REM　　　　　　　　　　　　　　デフォルト：Batファイルの所在パスの下にinstフォルダを作成する)
REM ③インストーラーの変換元ファイルをコピーする
REM ④変換ツールに使用するパスを変更する(指定パスまたはデフォルトパス)
REM ⑤変換TOOLを立ち上げる
REM ⑥USBにコピーする
REM *************************************************************************************

SET LOG= .\___log.txt

set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set slnpath=%cd%\master
REM set SubExe=.\AMS_F_Sub_Simulator.exe
REM set MainExe=.\AMS_F_Main_Simulator.exe
REM set MainLogLnk=.\Tera Term.lnk
set MainLogBat=.\______Log_Main.bat
set SubLogBat=.\______Log_Sub.bat
REM set PanelLog=.\PANEL_LOG_VIEWER.exe

set SubExe=%basedir%AMS_F_Sub_Simulator.exe
set MainExe=%basedir%AMS_F_Main_Simulator.exe
REM set PanelExe=D:\03-Src\04-AMS\AMS-Fpre\branch_mast\bin\project6_ams_panel.exe
set MainLogBat=%basedir%______Log_Main.bat
set SubLogBat=%basedir%______Log_Sub.bat
set PanelLog=D:\03-Src\04-AMS\AMS-Fpre\branch_mast\PANEL_LOG_VIEWER\bin\PANEL_LOG_VIEWER.exe

if exist %LOG% (
	del %LOG%
)
echo 定数表示   : %DATE% %TIME% >>%LOG%
echo basedir    : %basedir% >>%LOG%
echo basename   : %basename% >>%LOG%
echo batname    : %batname% >>%LOG%
echo SubExe     : %SubExe% >>%LOG%
echo MainExe    : %MainExe% >>%LOG%
echo PanelLog   : %PanelLog% >>%LOG%
rem echo MainLogLnk : %MainLogLnk% >>%LOG%
echo MainLogBat : %MainLogBat% >>%LOG%
echo SubLogBat  : %SubLogBat% >>%LOG%

REM pause
REM powershell start-process cmd -verb runas
color 0B

:STARTUP
echo MainLog実行:
REM timeout 3 
REM pause
REM echo MainLog実行開始: %DATE% %TIME% >>%LOG%
REM start "MainLog" "%MainLogLnk%"

path C:\Program Files\teraterm

echo MainLog実行開始: %DATE% %TIME% >>%LOG%
start "MainLog" "%MainLogBat%"

echo SubLog実行開始: %DATE% %TIME% >>%LOG%
start "SubLog" "%SubLogBat%"

echo Sub実行:
REM timeout 3 
REM pause
echo Sub実行開始: %DATE% %TIME% >>%LOG%
start "Sub" "%SubExe%"

echo Main実行:
REM timeout 3 
REM pause
echo Main実行開始: %DATE% %TIME% >>%LOG%
start "Main" "%MainExe%"

echo PanelLog実行:
REM timeout 3 
REM pause
echo PanelLog実行: %DATE% %TIME% >>%LOG%
start "PanelLog" "%PanelLog%"

REM echo Panel実行:
REM pause
REM echo Panel実行開始: %DATE% %TIME% >>%LOG%
REM start "" "%PanelExe%"

echo 処理終了:
REM pause
echo 処理終了: %DATE% %TIME% >>%LOG%
