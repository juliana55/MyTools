@echo off
pushd %~dp0
 
if "%~1" == "" (
	echo ファイル もしくは フォルダ をドラッグ＆ドロップしてください
	pause
)
 
echo ================
 
:loop
 
	call :Func_dirOrFile %1
 
shift
 
if not "%~1" == "" goto loop
 
echo ================
 
pause
 
popd
exit
 
rem ================================
rem ファイル属性のチェック
rem ================================
:Func_dirOrFile
 
	set attr=%~a1
	if %attr:~0,1%==d (
	 	call :Func_dir %1
	) else if %attr:~0,1%==- (
		call :Func_file %1
	)
 
	exit /b
 
rem ================================
rem フォルダの場合
rem ================================
:Func_dir
	echo ■フォルダ：%1%
 
	set dirName=%1
	for /F %%i in ('dir /s /b %dirName%\*.*') do call :Func_file %%i
 
	exit /b	
 
rem ================================
rem ファイルの場合
rem ================================
:Func_file
 
	set attr=%~a1
	if %attr:~0,1%==- (
		echo 　▼ファイル:%1%
		call :Func_cmd %1
	)
 
	exit /b
 
rem ================================
rem ファイルに対して実行したい処理
rem ================================
:Func_cmd
	set file=%1
 
	echo 　　ここでファイルに対して実行
 
	exit /b

