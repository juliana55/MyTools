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


set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set slnpath=%cd%\master

SET LOG= %basedir%___log.txt

set SubExe=AMS_F_Sub_Simulator.exe
set MainExe=AMS_F_Main_Simulator.exe
REM set SubExe=%basedir%master\bin\AMS_F_Sub_Simulator.exe
REM set MainExe=%basedir%master\bin\AMS_F_Main_Simulator.exe
set PanelExe=project6_ams_panel.exe
set PanelLog=PANEL_LOG_VIEWER.exe
set TTermExe=ttermpro.exe

if exist %LOG% (
	del %LOG%
)
echo 定数表示  : %DATE% %TIME% >>%LOG%
echo basedir   : %basedir% >>%LOG%
echo basename  : %basename% >>%LOG%
echo batname   : %batname% >>%LOG%
echo SubExe    : %SubExe% >>%LOG%
echo MainExe   : %MainExe% >>%LOG%
echo PanelLog  : %PanelLog% >>%LOG%
echo TTermExe  : %TTermExe% >>%LOG%

REM pause
REM powershell start-process cmd -verb runas

color 0A

:KILLTASK
echo Sub終了:
REM pause
echo Sub終了: %DATE% %TIME% >>%LOG%
taskkill /F /IM %SubExe% /T

echo Main終了:
REM pause
echo Main終了: %DATE% %TIME% >>%LOG%
taskkill /F /IM %MainExe% /T

echo PanelLog終了:
REM pause
echo PanelLog終了: %DATE% %TIME% >>%LOG%
taskkill /F /IM %PanelLog% /T

echo PanelExe終了:
REM pause
echo PanelExe終了: %DATE% %TIME% >>%LOG%
taskkill /F /IM %PanelExe% /T

echo TeraTerm終了:
REM pause
echo TeraTerm終了: %DATE% %TIME% >>%LOG%
taskkill /F /IM %TTermExe% /T

echo 処理終了:
REM pause
echo 処理終了: %DATE% %TIME% >>%LOG%
