set path=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;C:\asm\MainApp\Editor;
set MY_PATH=%CD%
set MY_LOG=%MY_PATH%\10_RelHibuild.log

REM HiViewのデバッグビルドでAsmControllerのReleaseイメージを参照しているので両方ビルド
REM devenv /rebuild Debug		%MY_PATH%\..\..\..\Controller\Controller.sln>%MY_LOG%
devenv /rebuild Release		%MY_PATH%\..\..\..\Controller\Controller.sln>%MY_LOG%
REM devenv /rebuild Debug 		%MY_PATH%\..\..\..\AsmController\AsmController.sln>>%MY_LOG%
devenv /rebuild Release		%MY_PATH%\..\..\..\AsmController\AsmController.sln>>%MY_LOG%
REM devenv /rebuild Debug 		%MY_PATH%\..\..\..\HiView\HiView.sln>>%MY_LOG%
devenv /rebuild Release 		%MY_PATH%\..\..\..\HiView\HiView.sln>>%MY_LOG%

REM ビルドに必要なDLLを配置。動作確認用にDLL配置
call update %MY_PATH%\..\..\..\..\Bin\Release        %MY_PATH%\..\..\..\Editor\bin_HI                                           %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Release        %MY_PATH%\..\..\..\Editor\bin_HI\Tools\OfflinePartsGenerator               %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Release        %MY_PATH%\..\..\..\Editor\bin_HI                                           %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Release        %MY_PATH%\..\..\..\HiView\bin                                              %MY_LOG% %MY_PATH%
call update %MY_PATH%\..\..\..\..\Bin\Release        %MY_PATH%\..\..\..\..\SubApp\SoftwareKB\bin                                %MY_LOG% %MY_PATH%

cd %MY_PATH%
devenv /rebuild Release		              %MY_PATH%\..\PwbDataAccess\PwbDataAccess.sln>>%MY_LOG%
devenv /rebuild Release		              %MY_PATH%\..\IPDPartsView\IPDPartsView.sln>>%MY_LOG%
devenv /rebuild Release_HI	              %MY_PATH%\..\IPDFileGenerate\IPDFileGenerate.sln>>%MY_LOG%
devenv /rebuild Release_HI	              %MY_PATH%\FPartsGenerator.sln>>%MY_LOG%

call update %MY_PATH%\..\Release_HI\bin\Tools\OfflinePartsGenerator        %MY_PATH%\..\..\..\Editor\bin_HI\Tools\OfflinePartsGenerator               %MY_LOG% %MY_PATH%



