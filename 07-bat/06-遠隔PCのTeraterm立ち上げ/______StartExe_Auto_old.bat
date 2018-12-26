@echo off
REM *************************************************************************************
REM �@��ƁE�i�[�t�H���_���w�肷��B
REM �A��ƁE�i�[�t�H���_���쐬(�w��p�X�܂��́A�w��p�X���Ȃ��ꍇ�̓f�t�H���g�p�X�g�p�B
REM�@�@�@�@�@�@�@�@�@�@�@�@�@�@�f�t�H���g�FBat�t�@�C���̏��݃p�X�̉���inst�t�H���_���쐬����)
REM �B�C���X�g�[���[�̕ϊ����t�@�C�����R�s�[����
REM �C�ϊ��c�[���Ɏg�p����p�X��ύX����(�w��p�X�܂��̓f�t�H���g�p�X)
REM �D�ϊ�TOOL�𗧂��グ��
REM �EUSB�ɃR�s�[����
REM *************************************************************************************

SET LOG= .\___log.txt

set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set slnpath=%cd%\master
REM set SubExe=.\AMS_F_Sub_Simulator.exe
REM set MainExe=.\AMS_F_Main_Simulator.exe
REM set MainLogLnk=.\Tera Term.lnk
set MainLogBat=.\______Log_Main.bat
set SubLogBat=.\______Log_Sub.bat
REM set PanelLog=.\PANEL_LOG_VIEWER.exe

set SubExe=%basedir%AMS_F_Sub_Simulator.exe
set MainExe=%basedir%AMS_F_Main_Simulator.exe
REM set PanelExe=D:\03-Src\04-AMS\AMS-Fpre\branch_mast\bin\project6_ams_panel.exe
set MainLogBat=%basedir%______Log_Main.bat
set SubLogBat=%basedir%______Log_Sub.bat
set PanelLog=D:\03-Src\04-AMS\AMS-Fpre\branch_mast\PANEL_LOG_VIEWER\bin\PANEL_LOG_VIEWER.exe

if exist %LOG% (
	del %LOG%
)
echo �萔�\��   : %DATE% %TIME% >>%LOG%
echo basedir    : %basedir% >>%LOG%
echo basename   : %basename% >>%LOG%
echo batname    : %batname% >>%LOG%
echo SubExe     : %SubExe% >>%LOG%
echo MainExe    : %MainExe% >>%LOG%
echo PanelLog   : %PanelLog% >>%LOG%
rem echo MainLogLnk : %MainLogLnk% >>%LOG%
echo MainLogBat : %MainLogBat% >>%LOG%
echo SubLogBat  : %SubLogBat% >>%LOG%

REM pause
REM powershell start-process cmd -verb runas
color 0B

:STARTUP
echo MainLog���s:
REM timeout 3 
REM pause
REM echo MainLog���s�J�n: %DATE% %TIME% >>%LOG%
REM start "MainLog" "%MainLogLnk%"

path C:\Program Files\teraterm

echo MainLog���s�J�n: %DATE% %TIME% >>%LOG%
start "MainLog" "%MainLogBat%"

echo SubLog���s�J�n: %DATE% %TIME% >>%LOG%
start "SubLog" "%SubLogBat%"

echo Sub���s:
REM timeout 3 
REM pause
echo Sub���s�J�n: %DATE% %TIME% >>%LOG%
start "Sub" "%SubExe%"

echo Main���s:
REM timeout 3 
REM pause
echo Main���s�J�n: %DATE% %TIME% >>%LOG%
start "Main" "%MainExe%"

echo PanelLog���s:
REM timeout 3 
REM pause
echo PanelLog���s: %DATE% %TIME% >>%LOG%
start "PanelLog" "%PanelLog%"

REM echo Panel���s:
REM pause
REM echo Panel���s�J�n: %DATE% %TIME% >>%LOG%
REM start "" "%PanelExe%"

echo �����I��:
REM pause
echo �����I��: %DATE% %TIME% >>%LOG%
