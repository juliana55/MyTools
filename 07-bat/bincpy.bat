@echo off
REM ***********************************
REM �@���������o�C�i��IMG.bin�̃R�s�[
REM �A�s�v��Src�AInc�t�H���_���폜
REM �B�폜�p.bat�t�@�C���폜
REM ***********************************

rem ���O�t�@�C���̃p�X���w��
SET LOG= .\___log.txt
rem �Վ���ƃt�@�C���쐬
SET TEMP= .\_temp.txt
SET DELBAT= .\_temp.bat

REM �J�n���O�o��
echo %DATE% %TIME% �����J�n >%LOG%

REM �o�C�i���R�s�[+++
copy .\BMP\Bin\IMG.bin      .\IMG_small.bin
echo %DATE% %TIME% IMG.bin�R�s�[ >>%LOG%
REM �o�C�i���R�s�[---


REM �t�H���_�p�X�擾����+++
dir /S /B /A:D "Src" >>%TEMP%
dir /S /B /A:D "Inc" >>%TEMP%
REM �t�H���_�p�X�擾����---

REM �폜���X�g�����O�o��
echo %DATE% %TIME% �폜���X�g:{} >>%LOG%
echo { >>%LOG%
copy /B %LOG% + %TEMP%
echo } >>%LOG%
rem copy .\_temp.txt    .\___log.txt

REM �R�}���h������ǉ��o�b�`�t�@�C���쐬+++
REM rm %DELBAT%
REM for /f "delims=" %%l in (_LOG.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
for /f "delims=" %%l in (_temp.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
echo %DATE% %TIME% �폜.bat�t�@�C���쐬 >>%LOG%
REM �R�}���h������ǉ��o�b�`�t�@�C����---

REM �s�v�t�H���_�폜+++
call %DELBAT%
REM �s�v�t�H���_�폜---

REM �폜�p�Վ�.bat�t�@�C���폜+++
del %DELBAT%
del %TEMP%
echo %DATE% %TIME% �s�v�t�@�C���폜�����B >>%LOG%
REM �폜�p�Վ�.bat�t�@�C���폜---

REM �I�����O�o��
echo %DATE% %TIME% �����I�� >>%LOG%









REM �Q�l+++
REM @echo off

REM rem ���O�t�@�C���̃p�X���w��
REM SET LOG= .\LOG.txt

REM rem �폜�Ώۂ̃t�H���_���w��
REM SET TARGET = D:\01-Src\03-AMS_F\AMS\AMS_F\DISP_DATA\BMP\Inc

REM rem �J�n���O�o��
REM echo %DATE% %TIME% �����J�n >%LOG%

REM rem �t�@�C�������ׂč폜
REM del /S /Q %TARGET% >>%LOG%

REM rem �폜�Ώۃt�H���_�̒��̃t�H���_�\�������ׂč폜
REM for /D %%1 in (%TARGET%) do rmdir /S /Q "%%1" >>%LOG%


REM rem �o�C�i���R�s�[
REM copy .\BMP\Bin\IMG.bin      .\IMG_small.bin

REM rem �I�����O�o��
REM echo %DATE% %TIME% �����I�� >>%LOG%
REM �Q�l---

