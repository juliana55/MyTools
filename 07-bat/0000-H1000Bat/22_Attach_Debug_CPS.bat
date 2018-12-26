set MY_PATH=%CD%

REM PwbDataAccess
copy %MY_PATH%\..\bin.debug\PwbDataAccess.dll %MY_PATH%\..\..\bin_CPS\

REM PwbDataCheck Debug_CPSを使用する
REM copy %MY_PATH%\..\..\bin_CPS\PwbDataCheckH.dll %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Release_CPS\bin\PwbData.dll         %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Release_CPS\bin\PwbDataCheck.dll    %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Release_CPS\bin\PwbDataCheckOld.dll %MY_PATH%\..\..\bin_CPS\

REM DAS Debug_CPSを使用する
REM copy %MY_PATH%\..\..\bin_CPS\DasArrangement.dll %MY_PATH%\..\..\bin_CPS\
REM copy %MY_PATH%\..\..\bin_CPS\DasCommonAPI.dll   %MY_PATH%\..\..\bin_CPS\
REM copy %MY_PATH%\..\..\bin_CPS\DasDataCheck.dll   %MY_PATH%\..\..\bin_CPS\
REM copy %MY_PATH%\..\..\bin_CPS\DasInterface.dll   %MY_PATH%\..\..\bin_CPS\
REM copy %MY_PATH%\..\..\bin_CPS\DasSimulation.dll  %MY_PATH%\..\..\bin_CPS\

REM Lang
copy %MY_PATH%\..\Lang\CPSmsg.txt %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Lang\DASmsg.txt %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Lang\PWBmsg.txt %MY_PATH%\..\..\bin_CPS\

REM DASTemplate
xcopy %MY_PATH%\..\DAS\DASTemplate %MY_PATH%\..\..\bin_CPS\DASTemplate /I /E /Y

REM McData
xcopy %MY_PATH%\..\DAS\McData %MY_PATH%\..\..\bin_CPS\McData /I /E /Y

REM INI
copy %MY_PATH%\..\Release_CPS\bin\Tools\DasAndSimulationTool\DasAndSimulationTool.ini %MY_PATH%\..\..\bin_CPS\Tools\DasAndSimulationTool\

copy %MY_PATH%\..\Release_CPS\bin\DasDebug.ini         %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Release_CPS\bin\DasProject.ini       %MY_PATH%\..\..\bin_CPS\
copy %MY_PATH%\..\Release_CPS\bin\Simulator.ini        %MY_PATH%\..\..\bin_CPS\

REM ドングル
copy %MY_PATH%\..\Release_CPS\bin\Tools\IPDDlls\sx32w.dll %MY_PATH%\..\..\bin_CPS\
