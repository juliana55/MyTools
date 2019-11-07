@echo off
 
rem set /p string="収集するファイル名に含まれる文字を入力してください："
rem set /p folder="収集対象をなるフォルダを入力してください："
 
rem 指定文字のフォルダを作成
rem if not exist %string% mkdir %string%
 
rem 収集対象フォルダへ移動
rem cd %folder%

set string=K202.bmp
 
rem 指定文字を含むファイルを探し収集する
for /r %%i in (*%string%*.*) do  (
rem   copy /y %%i %~dp0\%string%
 echo %%i>>j.txt
)
pause
