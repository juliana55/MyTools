@echo off
REM *************************************************************************************
REM �@���O���W���������B
REM �A���L���O�𓖓��̓��t�̃t�H���_���쐬���A���O�t�@�C�����t�H���_�Ɉړ�����B
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
echo TeraTerm�I��:
REM pause
echo TeraTerm�I��: %DATE% %TIME%
taskkill /F /IM %TTermExe% /T

:MOVEFILE
md %DailyDir%

move /y %WorkDir%Main_*.* %DailyDir%
move /y %WorkDir%Sub_*.* %DailyDir%
move /y %WorkDir%Panel_*.* %DailyDir%

echo �����I��:
REM pause
echo �����I��: %DATE% %TIME%

exit 0