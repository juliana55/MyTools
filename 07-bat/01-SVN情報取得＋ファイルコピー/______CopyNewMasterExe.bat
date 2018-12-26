REM **********************************************************
REM �@�D���݂�SVN���擾
REM �A�D����Revision�ԍ���T��
REM �B�D����Revision�ԍ����t�@�C�����̌��ɒǋL
REM �C�D���s�t�@�C���R�s�[
REM �D�D�R�s�[�����t�@�C����Rev�ԍ��ǋL���ė����Ɏc��
REM **********************************************************
@echo off
REM **********************************************************
REM �σp�X
set searchWD=�ŏI�ύX���r�W����: 
set changeWD=_Rev
set REVNUM=0
set filename=svninfo.txt
SET CURRPATH=%~dp0
SET WORKPATH=%CURRPATH%..\..\Main\New_master
SET INFOFILE=%CURRPATH%svninfo.txt
SET TMP= %CURRPATH%___tmp.txt
SET LOG= %CURRPATH%___log.txt
REM **********************************************************
REM �Œ�p�X
set MainExePath=%CURRPATH%..\..\Main\New_master\bin\
set SubExePath=%CURRPATH%..\..\Main\New_master\bin\
set PanelLogExe=%CURRPATH%..\branch_mast\PANEL_LOG_VIEWER\bin\
set PanelExe=%CURRPATH%..\\branch_mast\bin\
set MainExeFile=AMS_F_Main_Simulator.exe
set SubExeFile=AMS_F_Sub_Simulator.exe
set PanelLogExeFile=PANEL_LOG_VIEWER.exe
set PanelExeFile=project6_ams_panel.exe


SET HISTPATH=%CURRPATH%Old_NewMaster\
SET HIST=%HISTPATH%_histinfo.txt
set SPLIT=****************************************************************************

color 0A

echo //-------------------------------------------------------------------
echo //	MainSVN���擾���ARev�ԍ��擾�B
echo //-------------------------------------------------------------------
REM pause
echo �����J�n: %DATE% %TIME% >>%LOG%

:GETSVNINFO
type nul > %LOG%
setlocal enabledelayedexpansion
echo �yGETSVNINFO�z�J�n: %DATE% %TIME% >>%LOG%
if exist %WORKPATH% (
	echo �u%WORKPATH%�v�t�H���_�͑��݂��� >> %LOG%
	pushd %WORKPATH%
	path C:\Program Files\Subversion\bin
	call svn info > %INFOFILE%
	popd
	echo �u%CURRPATH%�v���J�����g >> %LOG%
) else (
	echo %WORKPATH%�t�H���_�͑��݂��Ȃ� >> %LOG%
	exit
)
endlocal

setlocal enabledelayedexpansion
type nul > %TMP%
rem �t�@�C�����̕������1�s���ǂݍ���
for /f "tokens=1* delims=: eol=" %%a in ('findstr /N /C:%searchWD% %INFOFILE%') do (
	rem �s�ԍ�
	set lineNum=%%a
	rem ���g
	set RevNo=%%b
	
	echo !lineNum! >> %TMP%
	echo !RevNo! >> %TMP%
    if not "!RevNo!" == "" (
     set RevNo=!RevNo:%searchWD%=%changeWD%!
    )
	rem echo !RevNo! >> %TMP%
	set REVNUM=!RevNo!
	rem echo !REVNUM! >> %TMP%
)
rem endlocal
echo �yGETSVNINFO�z�I��: %DATE% %TIME% >>%LOG%


echo //-------------------------------------------------------------------
echo //	�t�@�C�����R�s�[���܂��B
echo //-------------------------------------------------------------------
REM pause
REM powershell start-process cmd -verb runas
color 0E

rem setlocal enabledelayedexpansion
:COPYFILE
echo �yCOPYFILE�z�J�n: %DATE% %TIME% >>%LOG%

echo F|xcopy %MainExePath%%MainExeFile%  %CURRPATH% /Y >>%LOG%
echo F|xcopy %SubExePath%%SubExeFile%  %CURRPATH% /Y >>%LOG%
echo F|xcopy %PanelLogExe%%PanelLogExeFile%  %CURRPATH% /Y >>%LOG%
REM echo F|xcopy %PanelExe%%PanelExeFile%  %CURRPATH% /Y >>%LOG%

REM ������Rev�ԍ��t�ŃR�s�[����
echo F|xcopy %MainExePath%%MainExeFile%  %HISTPATH% /Y >>%LOG%
echo F|xcopy %SubExePath%%SubExeFile%  %HISTPATH% /Y >>%LOG%
rem echo !REVNUM! >> %TMP%
ren %HISTPATH%%MainExeFile% %MainExeFile%%REVNUM% >>%LOG%
ren %HISTPATH%%SubExeFile% %SubExeFile%%REVNUM% >>%LOG%
endlocal

echo �yCOPYFILE�z�I��: %DATE% %TIME% >>%LOG%

echo %SPLIT% >> %HIST%
copy %HIST% + %INFOFILE% %HIST%
copy %HIST% + %LOG% %HIST%
echo %SPLIT% >> %HIST%
del %TMP%
del %INFOFILE%
REM del %LOG%
echo �����I��: %DATE% %TIME% >>%LOG%
