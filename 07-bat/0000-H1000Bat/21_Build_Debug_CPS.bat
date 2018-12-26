set path=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE
set MY_PATH=%CD%
set MY_LOG=%MY_PATH%\21_Build_Debug_CPS.log

REM PwbDataAccess
devenv /rebuild Debug %MY_PATH%\..\PwbDataAccess\PwbDataAccess.sln>%MY_LOG%

REM PwbDataCheck Debug_CPSを使用する
devenv /rebuild Debug_CPS %MY_PATH%\..\PwbDataCheck\PwbDataCheck.sln>>%MY_LOG%

REM DAS Debug_CPSを使用する
devenv /rebuild Debug_CPS %MY_PATH%\..\DAS\Das.sln>>%MY_LOG%

REM DasAndSimulationTool
devenv /rebuild Debug %MY_PATH%\..\DasAndSimulationTool\DasAndSimulationTool.sln>>%MY_LOG%
