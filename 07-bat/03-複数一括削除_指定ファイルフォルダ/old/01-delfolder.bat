@echo off
REM ***********************************
REM �@生成したバイナリIMG.binのコピー
REM �A不要なSrc、Incフォルダを削除
REM �B削除用.batファイル削除
rem CMDにて作業したパスへ移動後このコマンドで一括フォルダ削除可能「For /r %d In (Release) Do Rmdir /s /q "%d"」
rem CMDにて作業したパスへ移動後このコマンドで一括指定拡張子削除可能「del /s /a *.scc」
REM ***********************************

SET LOG= .\___log.txt
SET TEMP= .\_temp.txt
SET DELBAT= .\_temp.bat

echo %DATE% %TIME% 処理開始 >%LOG%

rem copy .\BMP\Bin\IMG.bin      .\IMG_small.bin
rem copy .\BMP\Bin\IMG.bin      .\IMG.bin
rem echo %DATE% %TIME% IMG.binコピー >>%LOG%


dir /S /B /A:D "Src" >>%TEMP%
dir /S /B /A:D "Inc" >>%TEMP%
dir /S /B /A:D "CH1" >>%TEMP%
dir /S /B /A:D "ENG" >>%TEMP%

echo %DATE% %TIME% 削除リスト:{} >>%LOG%
echo { >>%LOG%
copy /B %LOG% + %TEMP%
echo } >>%LOG%

for /f "delims=" %%l in (_temp.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
echo %DATE% %TIME% 削除.batファイル作成 >>%LOG%

call %DELBAT%

rem del %DELBAT%
rem del %TEMP%
echo %DATE% %TIME% 不要ファイル削除完了。 >>%LOG%

echo %DATE% %TIME% 処理終了 >>%LOG%



