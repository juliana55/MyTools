@echo off
pushd %~dp0
 
if "%~1" == "" (
	echo �t�@�C�� �������� �t�H���_ ���h���b�O���h���b�v���Ă�������
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
rem �t�@�C�������̃`�F�b�N
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
rem �t�H���_�̏ꍇ
rem ================================
:Func_dir
	echo ���t�H���_�F%1%
 
	set dirName=%1
	for /F %%i in ('dir /s /b %dirName%\*.*') do call :Func_file %%i
 
	exit /b	
 
rem ================================
rem �t�@�C���̏ꍇ
rem ================================
:Func_file
 
	set attr=%~a1
	if %attr:~0,1%==- (
		echo �@���t�@�C��:%1%
		call :Func_cmd %1
	)
 
	exit /b
 
rem ================================
rem �t�@�C���ɑ΂��Ď��s����������
rem ================================
:Func_cmd
	set file=%1
 
	echo �@�@�����Ńt�@�C���ɑ΂��Ď��s
 
	exit /b

