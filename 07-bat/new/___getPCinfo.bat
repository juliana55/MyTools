@echo off
rem SVN_revision情報取得(Panel)
rem ログに残すためのヘッダーファイルを作成(ヘッダーファイルはコミット対象外にします)
rem SVN環境がない場合は情報無しヘッダーを作成
rem set basedir=%~dp0
rem set getrevdir=%~dp0..\..\
rem set LOG=%basedir%__built__.log
rem set orgtxt=%basedir%tortoisesvninfo_src.txt
rem set cresrc=%basedir%svninfo.h

set separate=************
@echo off

@REM //////////////////////////////////////////////////////////////
@REM 情報取得バッチファイル
@REM 連絡先：（送付者の名前とメルアドぐらい入れておく）
@REM //////////////////////////////////////////////////////////////

@echo これからシステムの情報を収集します。
@echo 完了したと出るまでこの画面は閉じないでください。 　　

@set OutputDir=%~dp0
@set FileName=systeminfo.txt

@echo ##Hostname > %OutputDir%%FileName%
hostname >> %OutputDir%%FileName%

@echo. >> %OutputDir%%FileName%
@echo ##Windows Version >> %OutputDir%%FileName%
@ver >> %OutputDir%%FileName%

@echo. >> %OutputDir%%FileName%
@echo ##Date time >> %OutputDir%%FileName%

@date /t >> %OutputDir%%FileName%
@time /t >> %OutputDir%%FileName%

@echo. >> %OutputDir%%FileName%
@echo ##Network Info >> %OutputDir%%FileName%
@ipconfig /all >> %OutputDir%%FileName%

@echo ##Environment Variable >> %OutputDir%%FileName%
set >> %OutputDir%%FileName%

@echo. >> %OutputDir%%FileName%
@echo ##System Info >> %OutputDir%%FileName%
@systeminfo >> %OutputDir%%FileName%

@echo. >> %OutputDir%%FileName%
@echo ##Task List >> %OutputDir%%FileName%
@tasklist >> %OutputDir%%FileName%


@echo. 　　
@echo. 
@echo/////////////////////////////////////////////////////
@echo 情報の取得が完了しました。
@echo %OutputDir%のフォルダに、%FileName%と言う名前のファ
@echo イルで記録されています。
@echo お手数ですがこれを、メールに添付し、(送付者の名前)
@echo (メルアド）
@echo までお送りください。
@echo.
@echo 最後にエクスプローラで%OutputDir%を開きます。
@echo/////////////////////////////////////////////////////

@pause

call explorer.exe /select,%OutputDir%%FileName%

exit /b
