@echo off
REM ***********************************
REM �@生成したバイナリIMG.binのコピー
REM �A不要なSrc、Incフォルダを削除
REM �B削除用.batファイル削除
REM ***********************************

start "" %windir%\system32\rundll32.exe shell32.dll,Control_RunDLL hotplug.dll
exit