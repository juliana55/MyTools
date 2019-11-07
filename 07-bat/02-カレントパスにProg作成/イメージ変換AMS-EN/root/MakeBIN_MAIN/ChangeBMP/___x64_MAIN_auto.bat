@echo off
set basedir=%~dp0
set RVL=_MAIN

setlocal
rem 作業ディレクトリクリア（全削除）
rmdir /S /Q %basedir%change\

rem 変換元ファイルを作業ディレクトリにコピー
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\表示データ\DATA\CSV %basedir%change\IF /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	変換TOOLよりファイル*.csv→*.infに変換
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.csv) do (
	DINFO200 %%i
)

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	他のBMPを作業フォルダ内にコピー中(少々時間かかる…)
echo //-------------------------------------------------------------------
pause 
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\表示データ\BMP\切り出しデータ %basedir%change /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	変換TOOLよりファイルBMP→IMGに変換
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.bmp) do (
	bmpimgx64 %%i /I
)

del /S /A *.bmp *.csv *.jbf *.db

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。IMG→BIN
echo //	・Format B:ボタンでdrv.binファイルをクリアする(size=0)
echo //	・D＆Dで「change」フォルダをwFtoolに入れる
echo //-------------------------------------------------------------------
pause 
%basedir%wFtool.exe

:pause 
echo //-------------------------------------------------------------------
echo //	生成したBINファイルを指定パスにコピーしフォルダを開く
echo //-------------------------------------------------------------------
set mainbin=%basedir%..\..\..\AMS\AMS210E\表示データ\
echo F|xcopy %basedir%drv.bin %mainbin%IM%RVL%.b01 /Y
EXPLORER "%mainbin%"

endlocal
