@echo off
REM ***********************************
REM �@���������o�C�i��IMG.bin�̃R�s�[
REM �A�s�v��Src�AInc�t�H���_���폜
REM �B�폜�p.bat�t�@�C���폜
REM ***********************************

start "" %windir%\system32\rundll32.exe shell32.dll,Control_RunDLL hotplug.dll
exit