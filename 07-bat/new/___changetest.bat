rem ��t�@�C���쐬
rem type nul> output.txt
@echo off

set filepath=D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\tortoisesvninfo_src.txt

rem �u���O������E�u���㕶�����ϐ��Œ�`
set before=user
set after=%USERNAME%

rem before�Eafter��for�����s�O�ɓW�J����i%�ŕϐ���`�j
rem input�t�@�C���̍s�iline�j��for�����s���ɓW�J����i!�ŕϐ���`�j
setlocal enabledelayedexpansion
for /f "delims=" %%I in (%filepath%) do (
 set line=%%I
 !line!>> output.txt
 
 )
rem for /f "delims=" %%I in (%filepath%) do (
rem     set line=%%I
rem     
rem    rem !line!>>out1.txt
rem     echo !!line:%before%=%after%!!>> output.txt
rem )
endlocal

pause