@echo off
REM ***********************************
REM ①生成したバイナリIMG.binのコピー
REM ②不要なSrc、Incフォルダを削除
REM ③削除用.batファイル削除
REM ***********************************

start "" %windir%\system32\rundll32.exe shell32.dll,Control_RunDLL hotplug.dll
exit