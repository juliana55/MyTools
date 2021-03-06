@echo off
REM ***********************************
REM �@生成したバイナリIMG.binのコピー
REM �A不要なSrc、Incフォルダを削除
REM �B削除用.batファイル削除
REM ***********************************

SET LOG= .\___log.txt
SET TEMP= .\_temp.txt
SET DELBAT= .\_temp.bat

echo %DATE% %TIME% 処理開始 >%LOG%

copy .\BMP\Bin\IMG.bin      .\IMG_small.bin
echo %DATE% %TIME% IMG.binコピー >>%LOG%


dir /S /B /A:D "Src" >>%TEMP%
dir /S /B /A:D "Inc" >>%TEMP%

echo %DATE% %TIME% 削除リスト:{} >>%LOG%
echo { >>%LOG%
copy /B %LOG% + %TEMP%
echo } >>%LOG%

for /f "delims=" %%l in (_temp.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
echo %DATE% %TIME% 削除.batファイル作成 >>%LOG%

call %DELBAT%

del %DELBAT%
del %TEMP%
echo %DATE% %TIME% 不要ファイル削除完了。 >>%LOG%

echo %DATE% %TIME% 処理終了 >>%LOG%



