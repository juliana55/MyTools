@echo off
set RVL=01020300

set basedir=%~dp0
set projdir=%basedir%..\
set rootdir=.\
set MANEGE_FILE=SC952_%RVL%.he2
set MANEGE_PATH=%basedir%manager\%MANEGE_FILE%
echo //-------------------------------------------------------------------
echo //	
echo //	�C���[�W�t�@�C���̃t�@�C�����R�s�[���ă��l�[��
echo //	
echo //-------------------------------------------------------------------

:pause
echo //
echo //-------------------------------------------------------------------
echo //	�t�@�C�����R�s�[���܂��B
echo //-------------------------------------------------------------------
:pause
echo //
echo F|xcopy %projdir%SC\SC952\BMP\Bin\CM.bin		%basedir%SC952\IMG\GP%RVL%.i01 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\FG.bin		%basedir%SC952\IMG\GP%RVL%.i02 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\SEW.bin		%basedir%SC952\IMG\GP%RVL%.i03 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\Mode.bin	%basedir%SC952\IMG\GP%RVL%.i04 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\WC.bin		%basedir%SC952\IMG\GP%RVL%.i05 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\JPN.bin		%basedir%SC952\IMG\GP%RVL%.i11 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\ENG.bin		%basedir%SC952\IMG\GP%RVL%.i12 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\CH1.bin		%basedir%SC952\IMG\GP%RVL%.i13 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LN.bin		%basedir%SC952\IMG\GP%RVL%.i10 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LN1.bin		%basedir%SC952\IMG\GP%RVL%.i90 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LANG.bin	%basedir%SC952\IMG\GP%RVL%.i91 /Y
echo F|xcopy %projdir%sc\SC952\CPANEL\Release\CPANEL.mot %basedir%SC952\SFORMAT\GP%RVL%.p01 /Y 
:pause 

echo //
echo //-------------------------------------------------------------------
echo //	he2�t�@�C���̐���
echo //-------------------------------------------------------------------
echo [ModeType]> %MANEGE_PATH% 
echo Mode=2 >> %MANEGE_PATH% 
echo [OPTION] >> %MANEGE_PATH% 
echo ��f�B���N�g��=%rootdir% >> %MANEGE_PATH% 
echo [COMMON] >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo CPU�^�C�v=R5S72670W144FP >> %MANEGE_PATH% 
echo �V�X�e��RVL=%RVL% >> %MANEGE_PATH% 
echo ����FROM��`�t�@�C���p�X= >> %MANEGE_PATH% 
echo �O��FROM��`�t�@�C���p�X= >> %MANEGE_PATH% 
echo �������ݐ���t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.hed >> %MANEGE_PATH% 
echo ����FROM�J�n�A�h���X= >> %MANEGE_PATH% 
echo ����FROM�I���A�h���X= >> %MANEGE_PATH% 
echo �O��FROM�J�n�A�h���X= >> %MANEGE_PATH% 
echo �O��FROM�I���A�h���X= >> %MANEGE_PATH% 
echo �������ݐ���t�@�C���i�[�A�h���X= >> %MANEGE_PATH% 
echo �A�h���X��ԃ}�X�N=1FFFFFFF >> %MANEGE_PATH% 
echo �_�E�����[�h����R�[�h=FF >> %MANEGE_PATH% 
echo �t�@�C�����������P��=0 >> %MANEGE_PATH% 
echo �p�l�����=14 >> %MANEGE_PATH% 
echo ���u���=0 >> %MANEGE_PATH% 
echo ���������t�@�C����=12 >> %MANEGE_PATH% 
echo [FILE1] >> %MANEGE_PATH% 
echo �������݃f�[�^���=0 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\SFORMAT\GP%RVL%.p01 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.p01 >> %MANEGE_PATH% 
echo �J�n�A�h���X= >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=1C0FFB00 >> %MANEGE_PATH% 
echo �A�h���X���=0 >> %MANEGE_PATH% 
echo �@��R�[�h= >> %MANEGE_PATH% 
echo RVL�R�[�h= >> %MANEGE_PATH% 
echo ���W���[����= >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=GP%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=1C000000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE2] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i01 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i01 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C000010 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE3] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i02 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i02 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C200000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE4] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i03 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i03 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C300000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE5] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i04 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i04 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C500000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE6] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i05 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i05 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0D000000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE7] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i11 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i11 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE8] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i12 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i12 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE9] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i13 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i13 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE10] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i10 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i10 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C800000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE11] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i90 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i90 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C80A000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE12] >> %MANEGE_PATH% 
echo �������݃f�[�^���=1 >> %MANEGE_PATH% 
echo ���̓t�@�C���p�X=%rootdir%SC952\IMG\GP%RVL%.i91 >> %MANEGE_PATH% 
echo �o�̓t�@�C���p�X=prog\SC952\GP%RVL%\GP%RVL%.i91 >> %MANEGE_PATH% 
echo �J�n�A�h���X=0 >> %MANEGE_PATH% 
echo �@��R�[�h�A�h���X=FFFFFFFF >> %MANEGE_PATH% 
echo �A�h���X���=1 >> %MANEGE_PATH% 
echo �@��R�[�h=SC-952 $ >> %MANEGE_PATH% 
echo RVL�R�[�h=%RVL% >> %MANEGE_PATH% 
echo ���W���[����=IPMAINPRGM >> %MANEGE_PATH% 
echo �@��ˑ�=1 >> %MANEGE_PATH% 
echo IPL�t���O=00 >> %MANEGE_PATH% 
echo �t�@�C����=IM%RVL% >> %MANEGE_PATH% 
echo �]����A�h���X=0C810000 >> %MANEGE_PATH% 


echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��B
echo //-------------------------------------------------------------------
:pause 
%rootdir%henkantool5\HenkanTool6.exe %rootdir%MANAGER\SC952_%RVL%.he2

pause

