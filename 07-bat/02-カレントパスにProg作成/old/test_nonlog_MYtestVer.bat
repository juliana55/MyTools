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
SET TMP= .\___tmp.txt

if exist %LOG% (
 echo �폜%LOG%
	del %LOG%
)
if exist %TMP% (
 echo �폜%TMP%
	del %TMP%
)
set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set newfolder=inst
REM *********************�����������ύX�\�p�����[�^����������**************************
set InstFolder=%basedir%%newfolder%
set RVL=01010055
set _imgRVL=010100
set FileName=DP
set inpath=%basedir%HENKATOOL\MANAGER\AMS-F_010100.he2
set outpath=%basedir%HENKATOOL\MANAGER\AMS-F_%RVL%_chgpath_.he2
set USBDriveVol=F:
REM *********************�����������ύX�\�p�����[�^����������**************************

echo //-------------------------------------------------------------------
echo //	
echo //	bat�t�@�C�����݃p�X���ɍ�ƃt�H���_�uinst�v�t�H���_���쐬����
echo //	bat�t�@�C�����݃v���W�F�N�g��.mot�A.bin�t�@�C�����R�s�[��rename����B
echo //	
echo //	�ύX����p�����[�^�F
echo // �@�DRVL��ς������ꍇ�͂���BAT�t�@�C���́uRVL=010100�v�ɂďC������B
echo //    �t�@�C�����uFileName=DP�v�ɂďC������B
echo // �A�D���PATH��ς������ꍇ�͂���BAT�t�@�C���́uInstFolder�v�ɂďC������B
echo // �@�@�f�t�H���g�F�uInstFolder�v��BAT�t�@�C���̏��݃p�X�ƂȂ�܂��B
echo // �B�D�g�p����u*.he2�v�t�@�C���̃p�X��ύX�������ꍇ�́uinpath�v�ɂďC������B
echo // �@�@�f�t�H���g�F�uinpath�v��BAT�t�@�C���̑��΃p�X�ƂȂ�܂��B
echo //	    �ϊ��c�[���pini�t�@�C���uAMS-F_010101.he2�v�����݂��Ȃ��Ə������I������B
echo //	    �ϊ��c�[���pini�t�@�C���̃p�X�́uinpath=******�v�́u*�v��������͂���B
echo // �C�D�o�͗pUSB�̃h���C�o�[�ԍ��́uUSBDriveVol�v�ɂďC������B
echo // �@�@�f�t�H���g�F�uUSBDriveVol�v�́uF:�v�ł��B
echo //	
echo //	BAT�t�@�C���̎��s��r���Ŏ~�߂����ꍇ�́hCTL+C�h�������B
echo //	
echo //-------------------------------------------------------------------

pause
REM �@��ƁE�i�[�p�X���͂���B

REM �A��ƁE�i�[�t�H���_���쐬(�w��p�X�܂��́A�w��p�X���Ȃ��ꍇ�̓f�t�H���g�p�X�g�p�B
REM�@�@�@�@�@�@�@�@�@�@�@�@�@�@�f�t�H���g�FBat�t�@�C���̏��݃p�X�̉���inst�t�H���_���쐬����)
echo �yCHECKPATH�z: %DATE% %TIME% >>%LOG%
:CHECKPATH
if not exist %InstFolder% (
�@mkdir %InstFolder%
�@@echo %InstFolder%�t�H���_�͑��݂��Ȃ����ߍ쐬���� >>%LOG%
) else (
�@@echo %InstFolder%�t�H���_�͑��݂��܂��̂ō�Ɖ\ >>%LOG%
)
if not exist %inpath% (
�@@echo �ϊ��c�[���pini�t�@�C���u%inpath%�v�����݂��܂��񏈗��I���B >>%LOG%
  exit /b 0
) else (
�@@echo �ϊ��c�[���pini�t�@�C���́u%inpath%�v >>%LOG%
)

REM �B�C���X�g�[���[�̕ϊ����t�@�C�����R�s�[����
:COPYFILE
echo �yCOPYFILE�z: %DATE% %TIME% >>%LOG%
echo //
echo //-------------------------------------------------------------------
echo //	�t�@�C�����R�s�[���܂��B
echo //-------------------------------------------------------------------
pause
REM echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\GP%RVL%.m01 /Y
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i01 /Y
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i11 /Y
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i51 /Y
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i61 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\%FileName%%RVL%.m01 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i01 /Y
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i11 /Y
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i51 /Y
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\%FileName%%_imgRVL%.i61 /Y

REM �C�ϊ��c�[���Ɏg�p����p�X��ύX����(�w��p�X�܂��̓f�t�H���g�p�X)
:FINDWORD
echo �yFINDWORD�z: %DATE% %TIME% >>%LOG%
set searchWD=��f�B���N�g��
set searchVN=�V�X�e��RVL
set KEY1=
set KEY2=

type nul > %outpath%
setlocal enabledelayedexpansion
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchWD% %inpath%') do (
 set line=%%a
 set KEY1=%%b
)
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchVN% %inpath%') do (
 set line=%%a
 set KEY2=%%b
)

:MAIN
echo �yMAIN�z�J�n: %DATE% %TIME% >>%LOG%
set before=%KEY1%
set after=%InstFolder%
set before1=%KEY2%
set after1=%RVL%
echo before   : %before% >>%LOG%
echo after    : %after% >>%LOG%
echo before1   : %before1% >>%LOG%
echo after1    : %after1% >>%LOG%
if exist %outpath% (
 del %outpath%
)
for /f "tokens=1* delims=: eol=" %%a in ('findstr /n "^" %inpath%') do (
 set line2=%%b
 if not "!line2!" == "" (
  set line2=!line2:%before%=%after%!
  set line2=!line2:%before1%=%after1%!
 )
 echo.!line2!>> %outpath%
)
endlocal

echo //
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��B���Ȃ��ꍇ�́hCTL+C�h�Œ��f���܂��B
echo //-------------------------------------------------------------------
pause
:TOOLSTARTUP
REM �C�ϊ�TOOL�𗧂��グ��
%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
echo �ϊ�TOOL�𗧂��グ�܂��g�p�ݒ�t�@�C���́u%outpath%�v�ł��B >>%LOG%

echo //
echo //-------------------------------------------------------------------
echo //	USB��PRG�t�@�C�����R�s�[���܂��B
echo //	�R�s�[���Ȃ��ꍇ�́hCTL+C�h�Œ��f���܂��B
echo //-------------------------------------------------------------------
pause
:USBOUPUT
echo �yUSBOUPUT�z: %DATE% %TIME% >>%LOG%
set __USB_DRV=%USBDriveVol%

set USBpath=%__USB_DRV%\Prog\AMSEN\BP%RVL%\
if exist %__USB_DRV% (
  REM xcopy %InstFolder%\prog\AMS-F\GP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\GP%RVL%\ /Y
  xcopy %InstFolder%\prog\AMS-F\%FileName%%RVL%\*.* %USBDriveVol%\Prog\AMS-F\%FileName%%RVL%\ /Y
) else (
    echo USB�p�X�u%__USB_DRV%�v�܂��́u%USBpath%�v�����݂��Ȃ� >>%LOG%
)
pause

:EOF
