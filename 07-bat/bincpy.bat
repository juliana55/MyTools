@echo off
REM ***********************************
REM ①生成したバイナリIMG.binのコピー
REM ②不要なSrc、Incフォルダを削除
REM ③削除用.batファイル削除
REM ***********************************

rem ログファイルのパスを指定
SET LOG= .\___log.txt
rem 臨時作業ファイル作成
SET TEMP= .\_temp.txt
SET DELBAT= .\_temp.bat

REM 開始ログ出力
echo %DATE% %TIME% 処理開始 >%LOG%

REM バイナリコピー+++
copy .\BMP\Bin\IMG.bin      .\IMG_small.bin
echo %DATE% %TIME% IMG.binコピー >>%LOG%
REM バイナリコピー---


REM フォルダパス取得成功+++
dir /S /B /A:D "Src" >>%TEMP%
dir /S /B /A:D "Inc" >>%TEMP%
REM フォルダパス取得成功---

REM 削除リストをログ出力
echo %DATE% %TIME% 削除リスト:{} >>%LOG%
echo { >>%LOG%
copy /B %LOG% + %TEMP%
echo } >>%LOG%
rem copy .\_temp.txt    .\___log.txt

REM コマンド文字列追加バッチファイル作成+++
REM rm %DELBAT%
REM for /f "delims=" %%l in (_LOG.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
for /f "delims=" %%l in (_temp.txt) do @echo rmdir /S /Q  %%l >> %DELBAT%
echo %DATE% %TIME% 削除.batファイル作成 >>%LOG%
REM コマンド文字列追加バッチファイル作---

REM 不要フォルダ削除+++
call %DELBAT%
REM 不要フォルダ削除---

REM 削除用臨時.batファイル削除+++
del %DELBAT%
del %TEMP%
echo %DATE% %TIME% 不要ファイル削除完了。 >>%LOG%
REM 削除用臨時.batファイル削除---

REM 終了ログ出力
echo %DATE% %TIME% 処理終了 >>%LOG%









REM 参考+++
REM @echo off

REM rem ログファイルのパスを指定
REM SET LOG= .\LOG.txt

REM rem 削除対象のフォルダを指定
REM SET TARGET = D:\01-Src\03-AMS_F\AMS\AMS_F\DISP_DATA\BMP\Inc

REM rem 開始ログ出力
REM echo %DATE% %TIME% 処理開始 >%LOG%

REM rem ファイルをすべて削除
REM del /S /Q %TARGET% >>%LOG%

REM rem 削除対象フォルダの中のフォルダ構造をすべて削除
REM for /D %%1 in (%TARGET%) do rmdir /S /Q "%%1" >>%LOG%


REM rem バイナリコピー
REM copy .\BMP\Bin\IMG.bin      .\IMG_small.bin

REM rem 終了ログ出力
REM echo %DATE% %TIME% 処理終了 >>%LOG%
REM 参考---

