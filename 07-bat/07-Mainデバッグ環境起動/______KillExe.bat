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


set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set slnpath=%cd%\master

SET LOG= %basedir%___log.txt

set SubExe=AMS_F_Sub_Simulator.exe
set MainExe=AMS_F_Main_Simulator.exe
REM set SubExe=%basedir%master\bin\AMS_F_Sub_Simulator.exe
REM set MainExe=%basedir%master\bin\AMS_F_Main_Simulator.exe
set PanelExe=project6_ams_panel.exe
set PanelLog=PANEL_LOG_VIEWER.exe
set TTermExe=ttermpro.exe

if exist %LOG% (
	del %LOG%
)
echo �萔�\��  : %DATE% %TIME% >>%LOG%
echo basedir   : %basedir% >>%LOG%
echo basename  : %basename% >>%LOG%
echo batname   : %batname% >>%LOG%
echo SubExe    : %SubExe% >>%LOG%
echo MainExe   : %MainExe% >>%LOG%
echo PanelLog  : %PanelLog% >>%LOG%
echo TTermExe  : %TTermExe% >>%LOG%

REM pause
REM powershell start-process cmd -verb runas

color 0A

:KILLTASK
echo Sub�I��:
REM pause
echo Sub�I��: %DATE% %TIME% >>%LOG%
taskkill /F /IM %SubExe% /T

echo Main�I��:
REM pause
echo Main�I��: %DATE% %TIME% >>%LOG%
taskkill /F /IM %MainExe% /T

echo PanelLog�I��:
REM pause
echo PanelLog�I��: %DATE% %TIME% >>%LOG%
taskkill /F /IM %PanelLog% /T

echo PanelExe�I��:
REM pause
echo PanelExe�I��: %DATE% %TIME% >>%LOG%
taskkill /F /IM %PanelExe% /T

echo TeraTerm�I��:
REM pause
echo TeraTerm�I��: %DATE% %TIME% >>%LOG%
taskkill /F /IM %TTermExe% /T

echo �����I��:
REM pause
echo �����I��: %DATE% %TIME% >>%LOG%
