@echo off
REM *************************************************************************************
REM ①ログ収集窓口を閉じる。
REM ②現有ログを当日の日付のフォルダを作成し、ログファイルをフォルダに移動する。
REM *************************************************************************************


set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set slnpath=%cd%\master
set TTermExe=ttermpro.exe



echo TTermExe  : %TTermExe%

REM pause
REM powershell start-process cmd -verb runas

color 0A

:KILLTASK
echo TeraTerm終了:
REM pause
echo TeraTerm終了: %DATE% %TIME%
taskkill /F /IM %TTermExe% /T

taskkill /F /IM RuntimeBroker.exe
taskkill /F /IM WindowsCamera.exe

echo 処理終了:
REM pause
echo 処理終了: %DATE% %TIME%

exit 0