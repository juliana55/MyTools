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

set LogFolder=C:\Log\
set YM=%date:~0,4%%date:~5,2%
set YMD=%date:~2,2%%date:~5,2%%date:~8,2%
set WorkDir=%LogFolder%%YM%\
set DailyDir=%WorkDir%%YMD%


echo TTermExe  : %TTermExe%

REM pause
REM powershell start-process cmd -verb runas

color 0A

:KILLTASK
echo TeraTerm終了:
REM pause
echo TeraTerm終了: %DATE% %TIME%
taskkill /F /IM %TTermExe% /T

:MOVEFILE
md %DailyDir%

move /y %WorkDir%Main_*.* %DailyDir%
move /y %WorkDir%Sub_*.* %DailyDir%
move /y %WorkDir%Panel_*.* %DailyDir%

echo 処理終了:
REM pause
echo 処理終了: %DATE% %TIME%

exit 0