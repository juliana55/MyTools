@echo off
rem SVN_revision���擾(Panel)
rem ���O�Ɏc�����߂̃w�b�_�[�t�@�C�����쐬(�w�b�_�[�t�@�C���̓R�~�b�g�ΏۊO�ɂ��܂�)
rem SVN�����Ȃ��ꍇ�͏�񖳂��w�b�_�[���쐬
rem set basedir=%~dp0
rem set getrevdir=%~dp0..\..\
rem set LOG=%basedir%__built__.log
rem set orgtxt=%basedir%tortoisesvninfo_src.txt
rem set cresrc=%basedir%svninfo.h

set separate=************
@echo off

@REM //////////////////////////////////////////////////////////////
@REM ���擾�o�b�`�t�@�C��
@REM �A����F�i���t�҂̖��O�ƃ����A�h���炢����Ă����j
@REM //////////////////////////////////////////////////////////////

@echo ���ꂩ��V�X�e���̏������W���܂��B
@echo ���������Əo��܂ł��̉�ʂ͕��Ȃ��ł��������B �@�@

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


@echo. �@�@
@echo. 
@echo/////////////////////////////////////////////////////
@echo ���̎擾���������܂����B
@echo %OutputDir%�̃t�H���_�ɁA%FileName%�ƌ������O�̃t�@
@echo �C���ŋL�^����Ă��܂��B
@echo ���萔�ł���������A���[���ɓY�t���A(���t�҂̖��O)
@echo (�����A�h�j
@echo �܂ł����肭�������B
@echo.
@echo �Ō�ɃG�N�X�v���[����%OutputDir%���J���܂��B
@echo/////////////////////////////////////////////////////

@pause

call explorer.exe /select,%OutputDir%%FileName%

exit /b
