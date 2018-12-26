set MY_PATH=%CD%

mkdir %MY_PATH%\..\..\bin_JAS
mkdir %MY_PATH%\..\..\bin_JAS\DAS

REM PwbDataAccess
copy %MY_PATH%\..\bin.debug\PwbDataAccess.dll %MY_PATH%\..\..\bin_JAS\DAS\

REM PwbDataCheck Debug_CPSを使用する
copy %MY_PATH%\..\..\bin_CPS\PwbDataCheckH.dll %MY_PATH%\..\..\bin_JAS\DAS\

REM DAS Debug_HIを使用する(ドングルチェックが不要のため)
copy %MY_PATH%\..\..\bin_HI\Tools\DAS\DasArrangement.dll %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\..\bin_HI\Tools\DAS\DasCommonAPI.dll %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\..\bin_HI\Tools\DAS\DasDataCheck.dll %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\..\bin_HI\Tools\DAS\DasInterface.dll %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\..\bin_HI\Tools\DAS\DasSimulation.dll %MY_PATH%\..\..\bin_JAS\DAS\

REM Lang
copy %MY_PATH%\..\Lang\CPSmsg.txt %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\Lang\DASmsg.txt %MY_PATH%\..\..\bin_JAS\DAS\
copy %MY_PATH%\..\Lang\PWBmsg.txt %MY_PATH%\..\..\bin_JAS\DAS\

REM DASTemplate
xcopy %MY_PATH%\..\DAS\DASTemplate %MY_PATH%\..\..\bin_JAS\DAS\DASTemplate /I /E /Y

REM McData
xcopy %MY_PATH%\..\DAS\McData %MY_PATH%\..\..\bin_JAS\McData /I /E /Y

REM INI
copy %MY_PATH%\..\Release_JAS\bin\DasAndSimulationTool.ini %MY_PATH%\..\..\bin_JAS\
copy %MY_PATH%\..\Release_JAS\bin\DAS\DasDebug.ini         %MY_PATH%\..\..\bin_JAS\DAS
copy %MY_PATH%\..\Release_JAS\bin\DAS\DasProject.ini       %MY_PATH%\..\..\bin_JAS\DAS
copy %MY_PATH%\..\Release_JAS\bin\DAS\Simulator.ini        %MY_PATH%\..\..\bin_JAS\DAS
copy %MY_PATH%\..\Release_JAS\bin\DAS\NcCheck.ini          %MY_PATH%\..\..\bin_JAS\DAS
