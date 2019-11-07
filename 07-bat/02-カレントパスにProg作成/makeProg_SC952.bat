@echo off
set RVL=01020300

set basedir=%~dp0
set projdir=%basedir%..\
set rootdir=.\
set MANEGE_FILE=SC952_%RVL%.he2
set MANEGE_PATH=%basedir%manager\%MANEGE_FILE%
echo //-------------------------------------------------------------------
echo //	
echo //	イメージファイルのファイルをコピーしてリネーム
echo //	
echo //-------------------------------------------------------------------

:pause
echo //
echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。
echo //-------------------------------------------------------------------
:pause
echo //
echo F|xcopy %projdir%SC\SC952\BMP\Bin\CM.bin		%basedir%SC952\IMG\GP%RVL%.i01 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\FG.bin		%basedir%SC952\IMG\GP%RVL%.i02 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\SEW.bin		%basedir%SC952\IMG\GP%RVL%.i03 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\Mode.bin	%basedir%SC952\IMG\GP%RVL%.i04 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\WC.bin		%basedir%SC952\IMG\GP%RVL%.i05 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\JPN.bin		%basedir%SC952\IMG\GP%RVL%.i11 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\ENG.bin		%basedir%SC952\IMG\GP%RVL%.i12 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\CH1.bin		%basedir%SC952\IMG\GP%RVL%.i13 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LN.bin		%basedir%SC952\IMG\GP%RVL%.i10 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LN1.bin		%basedir%SC952\IMG\GP%RVL%.i90 /Y
echo F|xcopy %projdir%\SC\SC952\BMP\Bin\LANG.bin	%basedir%SC952\IMG\GP%RVL%.i91 /Y
echo F|xcopy %projdir%sc\SC952\CPANEL\Release\CPANEL.mot %basedir%SC952\SFORMAT\GP%RVL%.p01 /Y 
:pause 

echo //
echo //-------------------------------------------------------------------
echo //	he2ファイルの生成
echo //-------------------------------------------------------------------
echo [ModeType]> %MANEGE_PATH% 
echo Mode=2 >> %MANEGE_PATH% 
echo [OPTION] >> %MANEGE_PATH% 
echo 基準ディレクトリ=%rootdir% >> %MANEGE_PATH% 
echo [COMMON] >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo CPUタイプ=R5S72670W144FP >> %MANEGE_PATH% 
echo システムRVL=%RVL% >> %MANEGE_PATH% 
echo 内蔵FROM定義ファイルパス= >> %MANEGE_PATH% 
echo 外部FROM定義ファイルパス= >> %MANEGE_PATH% 
echo 書き込み制御ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.hed >> %MANEGE_PATH% 
echo 内蔵FROM開始アドレス= >> %MANEGE_PATH% 
echo 内蔵FROM終了アドレス= >> %MANEGE_PATH% 
echo 外部FROM開始アドレス= >> %MANEGE_PATH% 
echo 外部FROM終了アドレス= >> %MANEGE_PATH% 
echo 書き込み制御ファイル格納アドレス= >> %MANEGE_PATH% 
echo アドレス空間マスク=1FFFFFFF >> %MANEGE_PATH% 
echo ダウンロード制御コード=FF >> %MANEGE_PATH% 
echo ファイル書き換え単位=0 >> %MANEGE_PATH% 
echo パネル種別=14 >> %MANEGE_PATH% 
echo 装置種別=0 >> %MANEGE_PATH% 
echo 書き換えファイル数=12 >> %MANEGE_PATH% 
echo [FILE1] >> %MANEGE_PATH% 
echo 書き込みデータ種別=0 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\SFORMAT\GP%RVL%.p01 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.p01 >> %MANEGE_PATH% 
echo 開始アドレス= >> %MANEGE_PATH% 
echo 機種コードアドレス=1C0FFB00 >> %MANEGE_PATH% 
echo アドレス種別=0 >> %MANEGE_PATH% 
echo 機種コード= >> %MANEGE_PATH% 
echo RVLコード= >> %MANEGE_PATH% 
echo モジュール名= >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=GP%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=1C000000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE2] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i01 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i01 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C000010 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE3] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i02 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i02 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C200000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE4] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i03 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i03 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C300000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE5] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i04 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i04 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C500000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE6] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i05 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i05 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0D000000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE7] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i11 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i11 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE8] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i12 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i12 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE9] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i13 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i13 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C810000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE10] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i10 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i10 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C800000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE11] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i90 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i90 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C80A000 >> %MANEGE_PATH% 
echo;>> %MANEGE_PATH% 
echo [FILE12] >> %MANEGE_PATH% 
echo 書き込みデータ種別=1 >> %MANEGE_PATH% 
echo 入力ファイルパス=%rootdir%SC952\IMG\GP%RVL%.i91 >> %MANEGE_PATH% 
echo 出力ファイルパス=prog\SC952\GP%RVL%\GP%RVL%.i91 >> %MANEGE_PATH% 
echo 開始アドレス=0 >> %MANEGE_PATH% 
echo 機種コードアドレス=FFFFFFFF >> %MANEGE_PATH% 
echo アドレス種別=1 >> %MANEGE_PATH% 
echo 機種コード=SC-952 $ >> %MANEGE_PATH% 
echo RVLコード=%RVL% >> %MANEGE_PATH% 
echo モジュール名=IPMAINPRGM >> %MANEGE_PATH% 
echo 機種依存=1 >> %MANEGE_PATH% 
echo IPLフラグ=00 >> %MANEGE_PATH% 
echo ファイル名=IM%RVL% >> %MANEGE_PATH% 
echo 転送先アドレス=0C810000 >> %MANEGE_PATH% 


echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。
echo //-------------------------------------------------------------------
:pause 
%rootdir%henkantool5\HenkanTool6.exe %rootdir%MANAGER\SC952_%RVL%.he2

pause

