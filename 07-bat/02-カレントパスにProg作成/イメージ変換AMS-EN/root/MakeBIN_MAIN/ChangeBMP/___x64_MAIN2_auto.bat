@echo off
set basedir=%~dp0
set RVL=_MAIN2

setlocal
rem ��ƃf�B���N�g���N���A�i�S�폜�j
rmdir /S /Q %basedir%change\

rem �ϊ����t�@�C������ƃf�B���N�g���ɃR�s�[
echo D|xcopy %basedir%..\..\..\AMS\AMS210E\�\���f�[�^\BMP\LAN\MAINL2 %basedir%change\MAINL2 /Q /Y /S

:pause 
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL���t�@�C��BMP��IMG�ɕϊ�
echo //-------------------------------------------------------------------
pause 
echo on
for /R %%i in (*.bmp) do (
  	bmpimgx64 %%i /I
)

rem �s�v���t�@�C���폜
del /S /A *.bmp *.csv *.jbf *.db

:pause 
@echo off
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��BIMG��BIN
echo //	���L�菇�ōs���Ă�������
echo //	�EFormat B:�{�^����drv.bin�t�@�C�����N���A����(size=0)
echo //	�E�`�F�b�N�����u�C���|�[�g���Ɉ��k����v�B
echo //	�E���`�F�b�N�{�b�N�X�Y�ꂸ�ɓ���Ă��������B��
echo //	�ED��D�Łuchange�v���́u\MAINL2�v�t�H���_��wFtool�ɓ����
echo //-------------------------------------------------------------------
pause 
%basedir%wFtool.exe

echo //-------------------------------------------------------------------
echo //	��������BIN�t�@�C�����w��t�H���_�ɃR�s�[���t�H���_���J��
echo //-------------------------------------------------------------------
set mainbin=%basedir%..\..\..\AMS\AMS210E\�\���f�[�^\
echo F|xcopy %basedir%drv.bin %mainbin%IM%RVL%.b22 /Y
EXPLORER "%mainbin%"

endlocal
