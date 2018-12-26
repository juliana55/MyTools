
@echo off
set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set datestr=%DATE:/=%
set timestrtmp=%TIME: =0%
set timestr=%timestrtmp:~0,2%%timestrtmp:~3,2%%timestrtmp:~6,2%
set timestamp=%datestr%-%timestr%

SET LOG= .\___log.txt
SET workfld=instal
REM ************************************************************************
REM *	①変換ツール用ファイル.he6の指定ワード差し替え
REM *	②ファイルコピー
REM *	③変換ツール立ち上げ
REM *	④USBにコピー

set FileName=DP
set RVL=01010456
set _imgRVL=010104

set InstFolder=%basedir%%workfld%
set inpath=%basedir%..\MANAGER\AMS-F_010104.he6
set outpath=%basedir%..\\MANAGER\AMS-F_change_%RVL%.he6
rem set outpath=D:\03-Src\04-AMS\AMS_F_Prj6_master\project6_ams_panel\TOOL\HENKATOOL\MANAGER\AMS-F_01010456.he6
set beforeoutpath=..\..\..\AMS\prog\AMS-F
set beforeinpath=temp\
set afteroutpath=.\%workfld%\PNL\prog\AMS-F
set afterinpath=%InstFolder%\

set USBDriveVol=F:
REM ************************************************************************

echo //
echo //-------------------------------------------------------------------
echo //	作業用instフォルダ生成。
echo //	コピーしない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:CREATEPATH
echo 【CREATEPATH】: %DATE% %TIME% >>%LOG%
if not exist %InstFolder% (
　mkdir %InstFolder%
　@echo %InstFolder%フォルダは存在しないため作成する >>%LOG%
) else (
　@echo %InstFolder%フォルダは存在しますので作業可能 >>%LOG%
)
if not exist %inpath% (
　@echo 変換ツール用iniファイル「%inpath%」が存在しません処理終了。 >>%LOG%
  exit /b 0
) else (
　@echo 変換ツール用iniファイルは「%inpath%」 >>%LOG%
)
pause

rem :INIT
rem if "%1"=="/?" (
rem   echo 使い方：%batname% 入力ファイル 出力ファイル 置換前文字列 置換後文字列
rem   exit /b 0
rem )
rem if "%4"=="" (
rem   echo 引数の数が不正です。
rem   goto ERROR
rem )
rem set inpath=%1
rem set outpath=%2
rem set before=%3
rem set after=%4
rem pause

:GETNOWRVL
echo 【GETNOWRVL】: %DATE% %TIME% >>%LOG%
set searchVN=システムRVL
set KEY1=
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchVN% %inpath%') do (
 set line=%%a
 set KEY1=%%b
)

echo //
echo //-------------------------------------------------------------------
echo //	*.he6に指定キーワード差し替え。しない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:MAKEINIFILE
echo 【MAKEINIFILE】: %DATE% %TIME% >>%LOG%
rem call :LOG 処理開始します。

type nul > %outpath%
setlocal enabledelayedexpansion
for /f "tokens=1* delims=: eol=" %%a in ('findstr /n "^" %inpath%') do (
  set line=%%b
  if not "!line!" == "" (
    set line=!line:%beforeoutpath%=%afteroutpath%!
	set line=!line:%beforeinpath%=%afterinpath%!
	set line=!line:%KEY1%=%RVL%!
  )
 echo.!line!>> %outpath%
)
rem endlocal

rem :END
rem call :LOG 正常終了です。
rem exit /b 0
rem 
rem :ERROR
rem call :LOG 異常終了です。
rem exit /b 1
rem 
rem :LOG
rem echo %DATE% %TIME% %basename% %1
rem exit /b 0
rem 
rem :EOF


REM ④インストーラーの変換元ファイルをコピーする
:COPYFILE
echo 【COPYFILE】: %DATE% %TIME% >>%LOG%
echo //
echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。しない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\CPanel\Release\CPANEL.mot      %InstFolder%\AMS-F\%FileName%%RVL%.m01 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i01 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\KCSTM.bin    %InstFolder%\AMS-F\%FileName%%_imgRVL%.i02 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i11 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\ENG.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i12 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\CH1.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i13 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\ESP.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i14 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\PRT.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i15 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\ITA.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i16 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\FRA.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i17 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\DEU.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i18 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\TUR.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i19 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\VNM.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i20 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\KHM.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i21 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\IDN.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i22 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\KOR.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i23 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\BUR.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i24 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\RUS.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i25 /Y
echo F|xcopy %basedir%..\..\..\AMS\AMS_F\DISP_DATA\BMP\Bin\ENG.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i26 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin      %InstFolder%\AMS-F\%FileName%%_imgRVL%.i51 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i61 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_ENG.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i62 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_CH1.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i63 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_ESP.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i64 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_PRT.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i65 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_ITA.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i66 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_FRA.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i67 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_DEU.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i68 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_TUR.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i69 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_VNM.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i70 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_KHM.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i71 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_IDN.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i72 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_KOR.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i73 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_BUR.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i74 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_RUS.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i75 /Y
echo F|xcopy %basedir%..\..\..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_ENG.bin  %InstFolder%\AMS-F\%FileName%%_imgRVL%.i76 /Y

echo //
echo //--------------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。しない場合は”CTL+C”で中断します。
echo //--------------------------------------------------------------------------
pause
:TOOLSTARTUP
echo 【TOOLSTARTUP】: %DATE% %TIME% >>%LOG%
REM ⑤変換TOOLを立ち上げる
REM 旧%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
HenKanTool6.exe		%outpath%
echo 変換TOOLを立ち上げます使用設定ファイルは「%outpath%」です。 >>%LOG%

echo //
echo //---------------------------------------------------------------------------
echo //	USBにPRGファイルをコピーします。コピーしない場合は”CTL+C”で中断します。
echo //---------------------------------------------------------------------------
pause

:USBOUPUT
REM ⑥成果物をUSBにコピー
echo 【USBOUPUT】: %DATE% %TIME% >>%LOG%
set __USB_DRV=%USBDriveVol%

set USBpath=%__USB_DRV%\Prog\AMSEN\BP%RVL%\
if exist %__USB_DRV% (
  REM xcopy %InstFolder%\prog\AMS-F\GP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\GP%RVL%\ /Y
  xcopy %InstFolder%\PNL\prog\AMS-F\%FileName%%RVL%\*.* %USBDriveVol%\Prog\AMS-F\%FileName%%RVL%\ /Y
) else (
    echo USBパス「%__USB_DRV%」または「%USBpath%」が存在しない >>%LOG%
)
pause

endlocal
