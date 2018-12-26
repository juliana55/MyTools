echo //
echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。しない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:TOOLSTARTUP
REM ⑤変換TOOLを立ち上げる
REM 旧%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
rem %basedir%HENKATOOL\henkantool6\HenkanTool6.exe %outpath%
D:\03-Src\04-AMS\AMS_F_Prj6_master\project6_ams_panel\TOOL\HENKATOOL\henkantool6\HenKanTool6.exe D:\03-Src\04-AMS\AMS_F_Prj6_master\project6_ams_panel\TOOL\HENKATOOL\MANAGER\AMS-F_change_01010456.he6
rem echo 変換TOOLを立ち上げます使用設定ファイルは「%outpath%」です。 >>%LOG%










rem @echo off
rem set a=3
rem set b=
rem set /P b="デバッグ種類を選択:[1:MainDebug 2:PanelDebug] >"
rem echo デバッグ種類は %b% です
rem 
rem if %b%==1 (
rem echo its 1 
rem ) else if %b%==2 (
rem echo its 2 
rem ) else (
rem echo its not 1 nor 2
rem )
rem pause

