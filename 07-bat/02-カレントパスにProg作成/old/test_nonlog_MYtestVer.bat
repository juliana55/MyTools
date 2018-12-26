@echo off
REM *************************************************************************************
REM ①作業・格納フォルダを指定する。
REM ②作業・格納フォルダを作成(指定パスまたは、指定パスがない場合はデフォルトパス使用。
REM　　　　　　　　　　　　　　デフォルト：Batファイルの所在パスの下にinstフォルダを作成する)
REM ③インストーラーの変換元ファイルをコピーする
REM ④変換ツールに使用するパスを変更する(指定パスまたはデフォルトパス)
REM ⑤変換TOOLを立ち上げる
REM ⑥USBにコピーする
REM *************************************************************************************

SET LOG= .\___log.txt
SET TMP= .\___tmp.txt

if exist %LOG% (
 echo 削除%LOG%
	del %LOG%
)
if exist %TMP% (
 echo 削除%TMP%
	del %TMP%
)
set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set newfolder=inst
REM *********************↓↓↓↓↓変更可能パラメータ↓↓↓↓↓**************************
set InstFolder=%basedir%%newfolder%
set RVL=01010055
set _imgRVL=010100
set FileName=DP
set inpath=%basedir%HENKATOOL\MANAGER\AMS-F_010100.he2
set outpath=%basedir%HENKATOOL\MANAGER\AMS-F_%RVL%_chgpath_.he2
set USBDriveVol=F:
REM *********************↑↑↑↑↑変更可能パラメータ↑↑↑↑↑**************************

echo //-------------------------------------------------------------------
echo //	
echo //	batファイル所在パス下に作業フォルダ「inst」フォルダを作成する
echo //	batファイル所在プロジェクトの.mot、.binファイルをコピーしrenameする。
echo //	
echo //	変更するパラメータ：
echo // ①．RVLを変えたい場合はこのBATファイルの「RVL=010100」にて修正する。
echo //    ファイル名「FileName=DP」にて修正する。
echo // ②．作業PATHを変えたい場合はこのBATファイルの「InstFolder」にて修正する。
echo // 　　デフォルト：「InstFolder」はBATファイルの所在パスとなります。
echo // ③．使用する「*.he2」ファイルのパスを変更したい場合は「inpath」にて修正する。
echo // 　　デフォルト：「inpath」はBATファイルの相対パスとなります。
echo //	    変換ツール用iniファイル「AMS-F_010101.he2」が存在しないと処理が終了する。
echo //	    変換ツール用iniファイルのパスは「inpath=******」の「*」部分を入力する。
echo // ④．出力用USBのドライバー番号は「USBDriveVol」にて修正する。
echo // 　　デフォルト：「USBDriveVol」は「F:」です。
echo //	
echo //	BATファイルの実行を途中で止めたい場合は”CTL+C”を押す。
echo //	
echo //-------------------------------------------------------------------

pause
REM ①作業・格納パス入力する。

REM ②作業・格納フォルダを作成(指定パスまたは、指定パスがない場合はデフォルトパス使用。
REM　　　　　　　　　　　　　　デフォルト：Batファイルの所在パスの下にinstフォルダを作成する)
echo 【CHECKPATH】: %DATE% %TIME% >>%LOG%
:CHECKPATH
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

REM ③インストーラーの変換元ファイルをコピーする
:COPYFILE
echo 【COPYFILE】: %DATE% %TIME% >>%LOG%
echo //
echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。
echo //-------------------------------------------------------------------
pause
REM echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\GP%RVL%.m01 /Y
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i01 /Y
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i11 /Y
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i51 /Y
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i61 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\%FileName%%RVL%.m01 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i01 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i11 /Y
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i51 /Y
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i61 /Y

REM ④変換ツールに使用するパスを変更する(指定パスまたはデフォルトパス)
:FINDWORD
echo 【FINDWORD】: %DATE% %TIME% >>%LOG%
set searchWD=基準ディレクトリ
set searchVN=システムRVL
set KEY1=
set KEY2=

type nul > %outpath%
setlocal enabledelayedexpansion
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchWD% %inpath%') do (
 set line=%%a
 set KEY1=%%b
)
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchVN% %inpath%') do (
 set line=%%a
 set KEY2=%%b
)

:MAIN
echo 【MAIN】開始: %DATE% %TIME% >>%LOG%
set before=%KEY1%
set after=%InstFolder%
set before1=%KEY2%
set after1=%RVL%
echo before   : %before% >>%LOG%
echo after    : %after% >>%LOG%
echo before1   : %before1% >>%LOG%
echo after1    : %after1% >>%LOG%
if exist %outpath% (
 del %outpath%
)
for /f "tokens=1* delims=: eol=" %%a in ('findstr /n "^" %inpath%') do (
 set line2=%%b
 if not "!line2!" == "" (
  set line2=!line2:%before%=%after%!
  set line2=!line2:%before1%=%after1%!
 )
 echo.!line2!>> %outpath%
)
endlocal

echo //
echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。しない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:TOOLSTARTUP
REM ④変換TOOLを立ち上げる
%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
echo 変換TOOLを立ち上げます使用設定ファイルは「%outpath%」です。 >>%LOG%

echo //
echo //-------------------------------------------------------------------
echo //	USBにPRGファイルをコピーします。
echo //	コピーしない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:USBOUPUT
echo 【USBOUPUT】: %DATE% %TIME% >>%LOG%
set __USB_DRV=%USBDriveVol%

set USBpath=%__USB_DRV%\Prog\AMSEN\BP%RVL%\
if exist %__USB_DRV% (
  REM xcopy %InstFolder%\prog\AMS-F\GP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\GP%RVL%\ /Y
  xcopy %InstFolder%\prog\AMS-F\%FileName%%RVL%\*.* %USBDriveVol%\Prog\AMS-F\%FileName%%RVL%\ /Y
) else (
    echo USBパス「%__USB_DRV%」または「%USBpath%」が存在しない >>%LOG%
)
pause

:EOF
