@echo off
set basedir=%~dp0
set RVL=_MAIN

setlocal
rem ��ƃf�B���N�g���N���A�i�S�폜�j
rmdir /S /Q %basedir%change\

rem �ϊ����t�@�C������ƃf�B���N�g���ɃR�s�[
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\�\���f�[�^\DATA\CSV %basedir%change\IF /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL���t�@�C��*.csv��*.inf�ɕϊ�
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.csv) do (
	DINFO200 %%i
)

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	����BMP����ƃt�H���_���ɃR�s�[��(���X���Ԃ�����c)
echo //-------------------------------------------------------------------
pause 
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\�\���f�[�^\BMP\�؂�o���f�[�^ %basedir%change /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL���t�@�C��BMP��IMG�ɕϊ�
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.bmp) do (
	bmpimgx64 %%i /I
)

del /S /A *.bmp *.csv *.jbf *.db

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��BIMG��BIN
echo //	�EFormat B:�{�^����drv.bin�t�@�C�����N���A����(size=0)
echo //	�ED��D�Łuchange�v�t�H���_��wFtool�ɓ����
echo //-------------------------------------------------------------------
pause 
%basedir%wFtool.exe

:pause 
echo //-------------------------------------------------------------------
echo //	��������BIN�t�@�C�����w��p�X�ɃR�s�[���t�H���_���J��
echo //-------------------------------------------------------------------
set mainbin=%basedir%..\..\..\AMS\AMS210E\�\���f�[�^\
echo F|xcopy %basedir%drv.bin %mainbin%IM%RVL%.b01 /Y
EXPLORER "%mainbin%"

endlocal
