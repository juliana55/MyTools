set path=%PATH%;C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;

cd ..\..
devenv /rebuild Debug		Tools-Source\PwbDataAccess\PwbDataAccess.sln>Tools-Source\FPartsGenerator\01_DbgHibuild.log
devenv /rebuild Debug		Tools-Source\IPDPartsView\IPDPartsView.sln>>Tools-Source\FPartsGenerator\01_DbgHibuild.log
devenv /rebuild Debug		Tools-Source\IPDFileGenerate\IPDFileGenerate.sln>>Tools-Source\FPartsGenerator\01_DbgHibuild.log

REM FPartsGenerator�̓v���W�F�N�g�ݒ肪���������̂łQ��r���h���K�v
devenv /rebuild Debug		Tools-Source\FPartsGenerator\FPartsGenerator.sln>>Tools-Source\FPartsGenerator\01_DbgHibuild.log
devenv /build Debug			Tools-Source\FPartsGenerator\FPartsGenerator.sln>>Tools-Source\FPartsGenerator\01_DbgHibuild.log
cd Tools-Source\FPartsGenerator




