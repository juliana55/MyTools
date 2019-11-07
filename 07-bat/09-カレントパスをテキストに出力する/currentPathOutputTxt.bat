set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA
echo %A% >j-test.txt


pushd %~dp0..
set A=%CD%
popd
echo %A% >>j-test.txt

set B=%CD%
echo %B% >>j-test.txt
