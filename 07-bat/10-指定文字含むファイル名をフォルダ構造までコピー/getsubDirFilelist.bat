@echo off
 
rem set /p string="���W����t�@�C�����Ɋ܂܂�镶������͂��Ă��������F"
rem set /p folder="���W�Ώۂ��Ȃ�t�H���_����͂��Ă��������F"
 
rem �w�蕶���̃t�H���_���쐬
rem if not exist %string% mkdir %string%
 
rem ���W�Ώۃt�H���_�ֈړ�
rem cd %folder%

set string=K202.bmp
 
rem �w�蕶�����܂ރt�@�C����T�����W����
for /r %%i in (*%string%*.*) do  (
rem   copy /y %%i %~dp0\%string%
 echo %%i>>j.txt
)
pause
