@echo off
REM ***********************************
REM �@�ϊ�����.txt�t�@�C�����X�g���쐬
REM �A�S�Ă̕����ϊ�*.txt�t�@�C�����P��txt�t�@�C���ɏW��
REM �B�R�s�[�p.bat�t�@�C���폜
REM ***********************************

SET LOG= .\___log.txt
SET TXTLIST= .\_txtList.txt
SET DELBAT= .\_temp.bat
rem set	/a pls=%cnt%+1

echo %DATE% %TIME% �����J�n >%LOG%

setlocal enabledelayedexpansion
set OutPutPath=.\TXT\
set OutPutFile=AllLanguage.txt
set OutPutUTF16=test.txt
set OutPutInfo=%OutPutPath%%OutPutFile%
set OutPututf=%OutPutPath%%OutPutUTF16%

if exist %OutPutInfo% @echo rmdir /S /Q %OutPutPath% >>%LOG%

dir /S /B ".\BMP\*.txt" >%TXTLIST%

@echo F|xcopy %TXTLIST% %OutPutInfo% /Y >>%LOG%

chcp 65001
set /A num=0
set split=**************************************
set hisi=����
cmd /U /c type %TXTLIST% >%OutPutInfo%

for /f "delims=" %%a in (%TXTLIST%) do (
     set /A num=num+1
     cmd /U /c @echo %hisi% !num! %split% %%a>>%OutPutInfo%
     cmd /U /c type %%a >>%OutPutInfo%
  )


chcp 932

if exist %TXTLIST% del %TXTLIST% >>%LOG%

echo %DATE% %TIME% �����I�� >>%LOG%

