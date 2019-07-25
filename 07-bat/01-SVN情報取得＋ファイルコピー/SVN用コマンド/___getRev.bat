@echo off
rem SVN_revision���擾
rem ���O�Ɏc�����߂̃w�b�_�[�t�@�C�����쐬
rem SVN�����Ȃ��ꍇ�͏�񖳂��w�b�_�[���쐬
set basedir=%~dp0
set getrevdir=%~dp0..\..\
set LOG=%basedir%__built__.log
set orgtxt=%basedir%revision_src.txt
set cresrc=%basedir%revision.h

set separate=************

echo %separate%>%LOG%
echo %getrevdir%>>%LOG%

echo %separate%>>%LOG%
setlocal enabledelayedexpansion
SubWCRev %getrevdir% %orgtxt% %cresrc%>>%LOG%

echo %separate%>>%LOG%
echo [!errorlevel!]>>%LOG%
if %errorlevel% neq 0 (
 echo [NG]>>%LOG%

 for /f "delims=" %%a in (%orgtxt%) do (
  set line=%%a
  set line=!line:#if 1=#if 0!
  echo !line! >> %cresrc%
 )
) else (
 echo [OK]>>%LOG%
)
endlocal

echo %separate%>>%LOG%
type %cresrc%>>%LOG%


exit /b
