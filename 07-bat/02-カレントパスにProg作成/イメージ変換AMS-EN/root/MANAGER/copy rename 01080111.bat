echo off
echo //-------------------------------------------------------------------
echo //	
echo //	C:\PROJECT5\AMS\AMS210E\CPanel\LD_ROM_Little\
echo //	�̃t�H���_����CAMS210E.mot���R�s�[����rename����B
echo //	
echo //	RVL��ς����ꍇ�͂���BAT�t�@�C�����G�f�B�^�ŊJ���āA
echo //	copy�s��IP******.m01��ҏW����B
echo //	
echo //	�e����PC��CF�h���C�u�͈قȂ邽�߁APRG�t�@�C���̃R�s�[���
echo //	���ɍ��킹�ĕҏW�̕K�v������B
echo //	
echo //	BAT�t�@�C���̎��s��r���Ŏ~�߂����ꍇ�́hCTL+C�h�������B
echo //	
echo //	pause���̓L�[���͂����߂�R�}���h�ŁA�K�v�Ȃ���΁h�F�h��t�����
echo //	���s����Ȃ��B
echo //	
echo //-------------------------------------------------------------------
:pause
echo //
echo //-------------------------------------------------------------------
echo //	�t�@�C�����R�s�[���܂��B
echo //-------------------------------------------------------------------
:pause
echo //
copy C:\PROJECT5\AMS\AMS210E\CPanel\LD_ROM_Little\CAMS210E.mot C:\PROJECT5\root\AMS210E\IP01080111.m01
:pause 
echo //-------------------------------------------------------------------
echo //	�ϊ�TOOL�𗧂��グ�܂��B
echo //-------------------------------------------------------------------
:pause 
C:\project5\root\henkantool4\HenKanTool4.exe C:\PROJECT5\root\MANAGER\AMSEN_IP420_BP01080111.he2
echo //
echo //-------------------------------------------------------------------
echo //	CF��PRG�t�@�C�����R�s�[���܂��B
echo //	�R�s�[���Ȃ��ꍇ�́hCTL+C�h�Œ��f���܂��B
echo //-------------------------------------------------------------------
pause
;copy C:\PROJECT5\root\Prog\AMSEN\BP010602\*.* G:\Prog\AMSEN\BP010602\
pause


