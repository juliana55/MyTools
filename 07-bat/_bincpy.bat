@echo off
REM ***********************************
REM �@���������o�C�i��IMG.bin�̃R�s�[
REM �A�s�v��Src�AInc�t�H���_���폜
REM �B�폜�p.bat�t�@�C���폜
REM ***********************************

SET LOG= .\___log.txt
SET TEMP= .\_temp.txt
SET DELBAT= .\_temp.bat

echo %DATE% %TIME% �����J�n >%LOG%

copy .\BMP\Bin\IMG.bin      .\IMG_small.bin
echo %DATE% %TIME% IMG.bin�R�s�[ >>%LOG%


dir /S /B /A:D "Src" >>%TEMP%
dir /S /B /A:D "Inc" >>%TEMP%

echo %DATE% %TIME% �폜���X�g:{} >>%LOG%
echo { >>%LOG%
copy /B %LOG% + %TEMP%
echo } >>%LOG%

for /f "delims=" %%l in (_temp.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
echo %DATE% %TIME% �폜.bat�t�@�C���쐬 >>%LOG%

call %DELBAT%

del %DELBAT%
del %TEMP%
echo %DATE% %TIME% �s�v�t�@�C���폜�����B >>%LOG%

echo %DATE% %TIME% �����I�� >>%LOG%


