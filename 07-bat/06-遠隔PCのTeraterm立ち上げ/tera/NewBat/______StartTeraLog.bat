@echo off

SET LogFolder=C:\Log\
SET YM=%date:~0,4%%date:~5,2%


md %LogFolder%%YM%

REM path C:\Program Files (x86)\teraterm

REM start StartLog_Main.lnk

REM start StartLog_Sub.lnk

REM start StartLog_Panel.lnk

path C:\Program Files\teraterm

start ______Log_Main.bat

start ______Log_Sub.bat

start ______Log_Panel.bat

exit 0
