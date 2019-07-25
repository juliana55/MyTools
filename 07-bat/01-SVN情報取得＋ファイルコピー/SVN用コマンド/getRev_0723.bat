@echo off

set basedir=%~dp0
set getrevdir=%~dp0..\..\
set LOG=%basedir%__built__.log
set RevHeader=%basedir%revision.h
set OpenHew=%getrevdir%\AMS\AMS_F\AMS_F.hws
set separate="******"

echo %separate%>%LOG%
echo %getrevdir%>>%LOG%

rem pause
echo %separate%>>%LOG%
SubWCRev %getrevdir% %basedir%revision_src.txt %RevHeader%>>%LOG%

rem pause
echo %separate%>>%LOG%
type %RevHeader%>>%LOG%


exit /b
