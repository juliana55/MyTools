@echo off

SET basedir=%~dp0
SET basename=%~n0

SET LogFolder=C:\Log\
SET YM=%date:~0,4%%date:~5,2%


md %LogFolder%%YM%

rem path C:\Program Files (x86)\teraterm
rem cd C:\Users\juki\Desktop\tera\NewBat

start %basedir%______Log_Sub.bat.lnk

start %basedir%______Log_Main.bat.lnk

start %basedir%______Log_Panel.bat.lnk

exit 0
