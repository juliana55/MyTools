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
 pause
 echo �폜%LOG%
	del %LOG%
)
if exist %TMP% (
 pause
 echo �폜%TMP%
	del %TMP%
)

set basedir=%~dp0
set basename=%~n0
set batname=%~n0%~x0
set newfolder=inst
REM *********************�����������ύX�\�p�����[�^����������**************************
set InstFolder=%basedir%%newfolder%
set RVL=010100
set inpath=%basedir%HENKATOOL\MANAGER\AMS-F_010101.he2
set outpath=%basedir%HENKATOOL\MANAGER\AMS-F_%RVL%_chgpath_1.he2
set USBDriveVol=F:
REM *********************�����������ύX�\�p�����[�^����������**************************

echo //-------------------------------------------------------------------
echo //	
echo //bat�t�@�C�����݃p�X���ɍ�ƃt�H���_�uinst�v�t�H���_���쐬����
echo //bat�t�@�C�����݃v���W�F�N�g��.mot�A.bin�t�@�C�����R�s�[��rename����B
echo //	
echo //�ύX����p�����[�^�F
echo //�@�DRVL��ς������ꍇ�͂���BAT�t�@�C���́uRVL=010100�v�ɂďC������B
echo //�A�D���PATH��ς������ꍇ�͂���BAT�t�@�C���́uInstFolder=�v�ɂďC������B
echo //�@�@�f�t�H���g�F�uInstFolder�v��BAT�t�@�C���̏��݃p�X�ƂȂ�܂��B
echo //�B�D�g�p����u*.he2�v�t�@�C���̃p�X��ύX�������ꍇ�́uinpath=�v�ɂďC������B
echo //�@�@�f�t�H���g�F�uinpath�v��BAT�t�@�C���̑��΃p�X�ƂȂ�܂��B
echo //    �ϊ��c�[���pini�t�@�C���uAMS-F_010101.he2�v�����݂��Ȃ��Ə������I������B
echo //    �ϊ��c�[���pini�t�@�C���̃p�X�́uinpath=******�v�́u*�v��������͂���B
echo //�C�D�o�͗pUSB�̃h���C�o�[�ԍ��́uUSBDriveVol=�v�ɂďC������B
echo //�@�@�f�t�H���g�F�uUSBDriveVol�v�́uF:�v�ł��B
echo //
echo //BAT�t�@�C���̎��s��r���Ŏ~�߂����ꍇ�́hCTL+C�h�������B
echo //
echo //-------------------------------------------------------------------

pause
echo �萔�\��  : %DATE% %TIME% >>%LOG%
echo basedir   : %basedir% >>%LOG%
echo basename  : %basename% >>%LOG%
echo batname   : %batname% >>%LOG%
echo InstFolder: %InstFolder%>>%LOG%
echo RVL       : %RVL% >>%LOG%
echo inpath    : %inpath% >>%LOG%
echo outpath   : %outpath% >>%LOG%
echo USBDriveVol: %outpath% >>%LOG%

REM echo { >>%LOG%
REM �@��ƁE�i�[�p�X�w�肷��B
REM :INPUT1
REM if "%1"=="" (
REM  echo �g�����F%batname% �w���ƃt�H���_
REM  set /p CHANGEPATH="��ƃp�X����͂��Ă�������"
REM  echo�@���̓p�X=%CHANGEPATH%
REM  echo�@"%CHANGEPATH%="%CHANGEPATH% >>%LOG%
REM IF EXIST %CHANGEPATH% ( 
REM     set InstFolder=%CHANGEPATH%
REM  	echo "�w��p�X�����݂���"
REM  	@echo "�w��p�X�����݂���"%InstFolder% >>%LOG%
REM  ) else (
REM  	echo "�w��p�X�����݂��܂���=>Default�p�X"%InstFolder%
REM  	echo "�w��p�X�����݂��܂���=>Default�p�X"%InstFolder% >>%LOG%
REM  )
REM REM exit /b 0
REM )
REM echo } >>%LOG%

echo { >>%LOG%
REM �A��ƁE�i�[�t�H���_���쐬(�w��p�X�܂��́A�w��p�X���Ȃ��ꍇ�̓f�t�H���g�p�X�g�p�B
REM�@�@�@�@�@�@�@�@�@�@�@�@�@�@�f�t�H���g�FBat�t�@�C���̏��݃p�X�̉���inst�t�H���_���쐬����)
echo �yCHECKPATH�z�J�n: %DATE% %TIME% >>%LOG%
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
echo �yCHECKPATH�z�I��: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
REM �B�C���X�g�[���[�̕ϊ����t�@�C�����R�s�[����
:COPYFILE
echo �yCOPYFILE�z�J�n: %DATE% %TIME% >>%LOG%
echo //
echo //-------------------------------------------------------------------
echo //	�t�@�C�����R�s�[���܂��B
echo //-------------------------------------------------------------------
pause
REM echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\GP%RVL%.m01 /Y >>%TMP%
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i01 /Y >>%TMP%
REM echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i11 /Y >>%TMP%
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i51 /Y >>%TMP%
REM echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\GP%RVL%.i61 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\CPanel\Release\CPANEL.mot  %InstFolder%\AMS-F\SFORMAT\DP%RVL%.m01 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\IMG.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i01 /Y >>%TMP%
echo F|xcopy %basedir%..\AMS\AMS_F\DISP_DATA\BMP\Bin\JPN.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i11 /Y >>%TMP%
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i51 /Y >>%TMP%
echo F|xcopy %basedir%..\PANEL\JOG\DISP_DATA\BMP\Bin\JOG_JPN.bin  %InstFolder%\AMS-F\IMG\DP%RVL%.i61 /Y >>%TMP%
type %TMP%
copy %LOG% + %TMP% >>%TMP%
del %TMP%
echo �yCOPYFILE�z�I��: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
REM �C�ϊ��c�[���Ɏg�p����p�X��ύX����(�w��p�X�܂��̓f�t�H���g�p�X)
:FINDWORD
echo �yFINDWORD�z�J�n: %DATE% %TIME% >>%LOG%
set searchWD=��f�B���N�g��
set searchVN=�V�X�e��RVL
set KEY1=
set KEY2=

type nul > %outpath%
setlocal enabledelayedexpansion
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchWD% %inpath%') do (
 set line=%%a
 set KEY1=%%b
 echo line=    : "!line!" >>%LOG%
 echo KEY1=    : "!KEY1!" >>%LOG%
REM echo.!KEY1!>> %outpath%
)
for /f "tokens=1* delims==" %%a in ('findstr /n /i %searchVN% %inpath%') do (
 set line=%%a
 set KEY2=%%b
 echo line=    : "!line!" >>%LOG%
 echo KEY2=    : "!KEY2!" >>%LOG%
REM echo.!KEY1!>> %outpath%
)
echo �yFINDWORD�z�I��: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
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
 echo �o�̓t�@�C�����݂���̂ō폜���܂��B%outpath% >>%LOG%
)
for /f "tokens=1* delims=: eol=" %%a in ('findstr /n "^" %inpath%') do (
 set line2=%%b
REM echo line2=: "!line2!" >>%LOG%
 if not "!line2!" == "" (
  set line2=!line2:%before%=%after%!
  set line2=!line2:%before1%=%after1%!
REM echo line2=: "!line2!" >>%LOG%
 )
 echo.!line2!>> %outpath%
)
endlocal
echo �yMAIN�z�I��: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo { >>%LOG%
:TOOLSTARTUP
echo �yTOOLSTARTUP�z�J�n: %DATE% %TIME% >>%LOG%
REM �C�ϊ�TOOL�𗧂��グ��
echo �ϊ�TOOL�𗧂��グ�܂��B >>%LOG%

echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��B���Ȃ��ꍇ�́hCTL+C�h�Œ��f���܂��B
echo //-------------------------------------------------------------------
pause
%basedir%HENKATOOL\henkantool5\HenkanTool5_20160208.exe %outpath%
echo �ϊ�TOOL�𗧂��グ�܂��g�p�ݒ�t�@�C���́u%outpath%�v�ł��B >>%LOG%
echo �yTOOLSTARTUP�z�I��: %DATE% %TIME% >>%LOG%
echo } >>%LOG%

echo //
echo //-------------------------------------------------------------------
echo //	USB��PRG�t�@�C�����R�s�[���܂��B
echo //	�R�s�[���Ȃ��ꍇ�́hCTL+C�h�Œ��f���܂��B
echo //-------------------------------------------------------------------
pause
:USBCOPY
echo �yUSBCOPY�z�J�n: %DATE% %TIME% >>%LOG%
set __USB_DRV=%USBDriveVol%
echo USB�̃p�X��%__USB_DRV% >>%LOG%

REM set __CHECK_DIR_PUSHD=pushd %__USB_DRV% >nul 2>&1
REM echo USB��%__CHECK_DIR_PUSHD% >>%LOG%
REM if ERRORLEVEL 1 (
REM     echo ## pushd���s�u%~dp0�v >>%LOG%
REM     dir >>%LOG%
REM     set __CHECK_DIR_PUSHD=UNSUCCESS
REM ) else (
REM �@�@echo ## pushd�����u%~dp0�v �� popd�Ŗ߂��Ă��� >>%LOG%
REM     dir >>%LOG%
REM �@�@set __CHECK_DIR_PUSHD=SUCCESS
REM �@�@popd
REM �@�@echo ## popd�ꏊ�u%~dp0�v >>%LOG%
REM )
set USBpath=%__USB_DRV%\Prog\AMSEN\BP%RVL%\
if exist %__USB_DRV% (
  REM xcopy %InstFolder%\prog\AMS-F\GP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\GP%RVL%\ /Y >>%TMP%
  xcopy %InstFolder%\prog\AMS-F\DP%RVL%\*.* %USBDriveVol%\Prog\AMS-F\DP%RVL%\ /Y >>%TMP%
  type %TMP%
  copy %LOG% + %TMP% >>%TMP%
  del %TMP%
) else (
    echo USB�p�X�u%__USB_DRV%�v�܂��́u%USBpath%�v�����݂��Ȃ� >>%LOG%
)
echo �yUSBCOPY�z�I��: %DATE% %TIME% >>%LOG%
pause

:EOF
