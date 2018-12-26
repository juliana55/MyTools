set path=%PATH%;C:\Program Files\Microsoft Visual Studio\2017\Professional\Common7\IDE\
set MY_PATH=%CD%
set MY_LOG=%MY_PATH%\01_Build_Debug_AMS_F.log

devenv /rebuild Debug %MY_PATH%\..\PwbDataAccess\PwbDataAccess.sln>%MY_LOG%

REM PwbDataAccess
REM devenv /rebuild Debug %MY_PATH%\..\PwbDataAccess\PwbDataAccess.sln>%MY_LOG%

REM PwbDataCheck Debug_CPSを使用する
REM devenv /rebuild Debug_CPS %MY_PATH%\..\PwbDataCheck\PwbDataCheck.sln>>%MY_LOG%

REM DAS Debug_HIを使用する(ドングルチェックが不要のため)
REM devenv /rebuild Debug_HI %MY_PATH%\..\DAS\Das.sln>>%MY_LOG%

REM DasAndSimulationTool
REM devenv /rebuild Debug_JAS %MY_PATH%\..\DasAndSimulationTool\DasAndSimulationTool.sln>>%MY_LOG%
