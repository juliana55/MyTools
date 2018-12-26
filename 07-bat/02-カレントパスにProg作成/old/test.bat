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
 pause
 echo 削除%LOG%
	del %LOG%
)
if exist %TMP% (
 pause
 echo 削除%TMP%
	del %TMP%
)

set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set newfolder=inst
REM *********************↓↓↓↓↓変更可能パラメータ↓↓↓↓↓**************************
set InstFolder=%basedir%%newfolder%
set RVL=010100
set inpath=%basedir%HENKATOOL\MANAGER\AMS-F_010101.he2
set outpath=%basedir%HENKATOOL\MANAGER\AMS-F_%RVL%_chgpath_1.he2
set USBDriveVol=F:
REM *********************↑↑↑↑↑変更可能パラメータ↑↑↑↑↑**************************

echo //-------------------------------------------------------------------
echo //	
echo //batファイル所在パス下に作業フォルダ「inst」フォルダを作成する
echo //batファイル所在プロジェクトの.mot、.binファイルをコピーしrenameする。
echo //	
echo //変更するパラメータ：
echo //①．RVLを変えたい場合はこのBATファイルの「RVL=010100」にて修正する。
echo //②．作業PATHを変えたい場合はこのBATファイルの「InstFolder=」にて修正する。
echo //　　デフォルト：「InstFolder」はBATファイルの所在パスとなります。
echo //③．使用する「*.he2」ファイルのパスを変更したい場合は「inpath=」にて修正する。
echo //　　デフォルト：「inpath」はBATファイルの相対パスとなります。
echo //    変換ツール用iniファイル「AMS-F_010101.he2」が存在しないと処理が終了する。
echo //    変換ツール用iniファイルのパスは「inpath=******」の「*」部分を入力する。
echo //④．出力用USBのドライバー番号は「USBDriveVol=」にて修正する。
echo //　　デフォルト：「USBDriveVol」は「F:」です。
echo //
echo //BATファイルの実行を途中で止めたい場合は”CTL+C”を押す。
echo //
echo //-------------------------------------------------------------------

pause
echo 定数表示  : %DATE% %TIME% >>%LOG%
echo basedir   : %basedir% >>%LOG%
echo basename  : %basename% >>%LOG%
echo batname   : %batname% >>%LOG%
echo InstFolder: %InstFolder%>>%LOG%
echo RVL       : %RVL% >>%LOG%
echo inpath    : %inpath% >>%LOG%
echo outpath   : %outpath% >>%LOG%
echo USBDriveVol: %outpath% >>%LOG%

REM echo { >>%LOG%
REM ①作業・格納パス指定する。
REM :INPUT1
REM if "%1"=="" (
REM  echo 使い方：%batname% 指定作業フォルダ
REM  set /p CHANGEPATH="作業パスを入力してください"
REM  echo　入力パス=%CHANGEPATH%
REM  echo　"%CHANGEPATH%="%CHANGEPATH% >>%LOG%
REM IF EXIST %CHANGEPATH% ( 
REM     set InstFolder=%CHANGEPATH%
REM  	echo "指定パスが存在する"
REM  	@echo "指定パスが存在する"%InstFolder% >>%LOG%
REM  ) else (
REM  	echo "指定パスが存在しません=>Defaultパス"%InstFolder%
REM  	echo "指定パスが存在しません=>Defaultパス"%InstFolder% >>%LOG%
REM  )
REM REM exit /b 0
REM )
REM echo } >>%LOG%

echo { >>%LOG%
REM ②作業・格納フォルダを作成(指定パスまたは、指定パスがない場合はデフォルトパス使用。
REM　　　　　　　　　　　　　　デフォルト：Batファイルの所在パスの下にinstフォルダを作成する)
echo 【CHECKPATH】開始: %DATE% %TIME% >>%LOG%
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
echo 【CHECKPATH】終了: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
REM ③インストーラーの変換元ファイルをコピーする
:COPYFILE
echo 【COPYFILE】開始: %DATE% %TIME% >>%LOG%
echo //
echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。
echo //-------------------------------------------------------------------
pause
REM echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\GP%RVL%.m01 /Y >>%TMP%
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i01 /Y >>%TMP%
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i11 /Y >>%TMP%
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i51 /Y >>%TMP%
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i61 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\DP%RVL%.m01 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i01 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i11 /Y >>%TMP%
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i51 /Y >>%TMP%
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i61 /Y >>%TMP%
type %TMP%
copy %LOG% + %TMP% >>%TMP%
del %TMP%
echo 【COPYFILE】終了: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
REM ④変換ツールに使用するパスを変更する(指定パスまたはデフォルトパス)
:FINDWORD
echo 【FINDWORD】開始: %DATE% %TIME% >>%LOG%
set searchWD=基準ディレクトリ
set searchVN=システムRVL
set KEY1=
set KEY2=

type nul > %outpath%
setlocal enabledelayedexpansion
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchWD% %inpath%') do (
 set line=%%a
 set KEY1=%%b
 echo line=    : "!line!" >>%LOG%
 echo KEY1=    : "!KEY1!" >>%LOG%
REM echo.!KEY1!>> %outpath%
)
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchVN% %inpath%') do (
 set line=%%a
 set KEY2=%%b
 echo line=    : "!line!" >>%LOG%
 echo KEY2=    : "!KEY2!" >>%LOG%
REM echo.!KEY1!>> %outpath%
)
echo 【FINDWORD】終了: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
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
 echo 出力ファイル存在するので削除します。%outpath% >>%LOG%
)
for /f "tokens=1* delims=: eol=" %%a in ('findstr /n "^" %inpath%') do (
 set line2=%%b
REM echo line2=: "!line2!" >>%LOG%
 if not "!line2!" == "" (
  set line2=!line2:%before%=%after%!
  set line2=!line2:%before1%=%after1%!
REM echo line2=: "!line2!" >>%LOG%
 )
 echo.!line2!>> %outpath%
)
endlocal
echo 【MAIN】終了: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
:TOOLSTARTUP
echo 【TOOLSTARTUP】開始: %DATE% %TIME% >>%LOG%
REM ④変換TOOLを立ち上げる
echo 変換TOOLを立ち上げます。 >>%LOG%

echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。しない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
echo 変換TOOLを立ち上げます使用設定ファイルは「%outpath%」です。 >>%LOG%
echo 【TOOLSTARTUP】終了: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo //
echo //-------------------------------------------------------------------
echo //	USBにPRGファイルをコピーします。
echo //	コピーしない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:USBCOPY
echo 【USBCOPY】開始: %DATE% %TIME% >>%LOG%
set __USB_DRV=%USBDriveVol%
echo USBのパスは%__USB_DRV% >>%LOG%

REM set __CHECK_DIR_PUSHD=pushd %__USB_DRV% >nul 2>&1
REM echo USBは%__CHECK_DIR_PUSHD% >>%LOG%
REM if ERRORLEVEL 1 (
REM     echo ## pushd失敗「%~dp0」 >>%LOG%
REM     dir >>%LOG%
REM     set __CHECK_DIR_PUSHD=UNSUCCESS
REM ) else (
REM 　　echo ## pushd成功「%~dp0」 → popdで戻しておく >>%LOG%
REM     dir >>%LOG%
REM 　　set __CHECK_DIR_PUSHD=SUCCESS
REM 　　popd
REM 　　echo ## popd場所「%~dp0」 >>%LOG%
REM )
set USBpath=%__USB_DRV%\Prog\AMSEN\BP%RVL%\
if exist %__USB_DRV% (
  REM xcopy %InstFolder%\prog\AMS-F\GP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\GP%RVL%\ /Y >>%TMP%
  xcopy %InstFolder%\prog\AMS-F\DP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\DP%RVL%\ /Y >>%TMP%
  type %TMP%
  copy %LOG% + %TMP% >>%TMP%
  del %TMP%
) else (
    echo USBパス「%__USB_DRV%」または「%USBpath%」が存在しない >>%LOG%
)
echo 【USBCOPY】終了: %DATE% %TIME% >>%LOG%
pause

:EOF
