set path=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;C:\asm\MainApp\Editor;
set MY_PATH=%CD%
set MY_LOG=%MY_PATH%\00_DbgHibuild.log

REM HiView�̃f�o�b�O�r���h��AsmController��Release�C���[�W���Q�Ƃ��Ă���̂ŗ����r���h
devenv /rebuild Debug		%MY_PATH%\..\..\..\Controller\Controller.sln>%MY_LOG%
devenv /rebuild Release		%MY_PATH%\..\..\..\Controller\Controller.sln>%MY_LOG%
devenv /rebuild Debug 		%MY_PATH%\..\..\..\AsmController\AsmController.sln>>%MY_LOG%
devenv /rebuild Release		%MY_PATH%\..\..\..\AsmController\AsmController.sln>>%MY_LOG%
devenv /rebuild Debug 		%MY_PATH%\..\..\..\HiView\HiView.sln>>%MY_LOG%

REM �r���h�ɕK�v��DLL��z�u�B����m�F�p��DLL�z�u
call update %MY_PATH%\..\..\..\..\Bin\Debug        %MY_PATH%\..\..\..\Editor\bin_HI                                           %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Debug        %MY_PATH%\..\..\..\Editor\bin_HI\Tools\OfflinePartsGenerator               %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Debug        %MY_PATH%\..\..\..\Editor\bin_HI                                           %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Debug        %MY_PATH%\..\..\..\HiView\bin                                              %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Debug        %MY_PATH%\..\..\..\..\SubApp\SoftwareKB\bin                                %MY_LOG% %MY_PATH%

cd %MY_PATH%
devenv /rebuild Debug		              %MY_PATH%\..\PwbDataAccess\PwbDataAccess.sln>>%MY_LOG%
devenv /rebuild Debug		              %MY_PATH%\..\IPDPartsView\IPDPartsView.sln>>%MY_LOG%
devenv /rebuild Debug_HI	              %MY_PATH%\..\IPDFileGenerate\IPDFileGenerate.sln>>%MY_LOG%
devenv /rebuild Debug_HI	              %MY_PATH%\FPartsGenerator.sln>>%MY_LOG%

call update %MY_PATH%\..\bin.debug        %MY_PATH%\..\..\..\Editor\bin_HI\Tools\OfflinePartsGenerator               %MY_LOG% %MY_PATH%



