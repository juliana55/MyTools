@echo off

SET LogFolder=C:\Log\
SET YM=%date:~0,4%%date:~5,2%


md %LogFolder%%YM%

path C:\Program Files (x86)\teraterm

start StartLog_Main.lnk

start StartLog_Sub.lnk

start StartLog_Panel.lnk

exit 0
