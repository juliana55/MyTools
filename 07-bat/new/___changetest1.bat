echo off
set before=username
set after=%USERNAME%
echo 置換開始
setlocal enabledelayedexpansion
for /f "delims=" %%A in (D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\tortoisesvninfo_src.txt) do (
rem    set line0=%%A
rem    set line1=!line0:copy_base1=%tikan1%!
rem    set line2=!line1:copy_base2=%tikan2%!
rem    echo !line2!>>D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\output.txt

 set line=%%A

 if "%line%"=="""" (
  echo space>>D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\output.txt 
 ) 

 echo !line:%before%=%after%!>>D:\01-Src\01-AMS-F\02-ReleasePanel\RMLog\Inc\output.txt
)
endlocal
echo 置換完了
pause

