@echo off
set basedir=%~dp0
set RVL=_MAIN2

setlocal
rem 作業ディレクトリクリア（全削除）
rmdir /S /Q %basedir%change\

rem 変換元ファイルを作業ディレクトリにコピー
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\表示データ\BMP\LAN\MAINL2 %basedir%change\MAINL2 /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	変換TOOLよりファイルBMP→IMGに変換
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.bmp) do (
  	bmpimgx64 %%i /I
)

rem 不要元ファイル削除
del /S /A *.bmp *.csv *.jbf *.db

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。IMG→BIN
echo //	下記手順で行ってください
echo //	・Format B:ボタンでdrv.binファイルをクリアする(size=0)
echo //	・チェック入れる「インポート時に圧縮する」。
echo //	・★チェックボックス忘れずに入れてください。★
echo //	・D＆Dで「change」下の「\MAINL2」フォルダをwFtoolに入れる
echo //-------------------------------------------------------------------
pause 
%basedir%wFtool.exe

echo //-------------------------------------------------------------------
echo //	生成したBINファイルを指定フォルダにコピーしフォルダを開く
echo //-------------------------------------------------------------------
set mainbin=%basedir%..\..\..\AMS\AMS210E\表示データ\
echo F|xcopy %basedir%drv.bin %mainbin%IM%RVL%.b22 /Y
EXPLORER "%mainbin%"

endlocal
