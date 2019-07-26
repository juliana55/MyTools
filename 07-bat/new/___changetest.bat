rem 空ファイル作成
rem type nul> output.txt
@echo off

set filepath=D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\tortoisesvninfo_src.txt

rem 置換前文字列・置換後文字列を変数で定義
set before=user
set after=%USERNAME%

rem before・afterはfor文実行前に展開する（%で変数定義）
rem inputファイルの行（line）はfor文実行中に展開する（!で変数定義）
setlocal enabledelayedexpansion
for /f "delims=" %%I in (%filepath%) do (
 set line=%%I
 !line!>> output.txt
 
 )
rem for /f "delims=" %%I in (%filepath%) do (
rem     set line=%%I
rem     
rem    rem !line!>>out1.txt
rem     echo !!line:%before%=%after%!!>> output.txt
rem )
endlocal

pause