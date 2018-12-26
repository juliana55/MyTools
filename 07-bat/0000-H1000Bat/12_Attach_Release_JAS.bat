set MY_PATH=%CD%

REM PwbDataAccess
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\PwbDataAccess.dll %MY_PATH%\..\Release_JAS\bin\DAS\

REM PwbDataCheck Release_CPSを使用する
copy %MY_PATH%\..\Release_CPS\bin\PwbDataCheckH.dll %MY_PATH%\..\Release_JAS\bin\DAS\

REM DAS Release_HIを使用する(ドングルチェックが不要のため)
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\DasArrangement.dll %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\DasCommonAPI.dll %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\DasDataCheck.dll %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\DasInterface.dll %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Release_HI\bin\Tools\DAS\DasSimulation.dll %MY_PATH%\..\Release_JAS\bin\DAS\

REM Lang
copy %MY_PATH%\..\Lang\CPSmsg.txt %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Lang\DASmsg.txt %MY_PATH%\..\Release_JAS\bin\DAS\
copy %MY_PATH%\..\Lang\PWBmsg.txt %MY_PATH%\..\Release_JAS\bin\DAS\

REM DASTemplate
xcopy %MY_PATH%\..\DAS\DASTemplate %MY_PATH%\..\Release_JAS\bin\DAS\DASTemplate /I /E /Y

REM McData
REM xcopy %MY_PATH%\..\DAS\McData %MY_PATH%\..\Release_JAS\bin\McData /I /E /Y
xcopy %MY_PATH%\..\DAS\McData\E1200 %MY_PATH%\..\Release_JAS\bin\McData\E1200 /I /E /Y
xcopy %MY_PATH%\..\DAS\McData\H1000\High16 %MY_PATH%\..\Release_JAS\bin\McData\H1000\High16 /I /E /Y
REM xcopy %MY_PATH%\..\DAS\McData\H1000\Multi8 %MY_PATH%\..\Release_JAS\bin\McData\H1000\Multi8 /I /E /Y
REM xcopy %MY_PATH%\..\DAS\McData\H1000\Odd2 %MY_PATH%\..\Release_JAS\bin\McData\H1000\Odd2 /I /E /Y
copy %MY_PATH%\..\DAS\McData\nozzle_lib.ini %MY_PATH%\..\Release_JAS\bin\McData\nozzle_lib.ini
REM copy %MY_PATH%\..\..\..\..\Intime\asm\Ddrive\asm\McData\NozzleLib.ini %MY_PATH%\..\Release_JAS\bin\McData\nozzle_lib.ini
