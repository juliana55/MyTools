
               Universal USB Driver Ver 2.0β

                                                        Oct 24 2001							 
                                                 kashiwano masahiro

UUSBD(Universal USB Driver)は汎用のUSBドライバーです。

このパッケージには次のフォルダーが含まれています

inc\	uusbd.dllを使用するアプリケーションを作成する際に必要
	なincludeファイル

lib\	uusbd.dll 本体
	uusbd.lib アプリケーションにuusbd.dllをリンクする
	ライブラリ

pipedump\
	uusbd.dllを使った簡単なプログラム。
	USBデバイスから送られてくるデータをダンプ表示する。

setup\	uusbd.inf uusbd.sysを組み込むためのINFファイル
	uusbd.sys 汎用USBドライバー本体

sys\	uusbd.sysのソースファイル

uusbddll\
	uusbd.dllのソースファイル


uusbd.dllとは

uusbd.dllは汎用USBデバイスドライバーuusbd.sysをWin32アプリから
簡単に使用できるようにするDLLです。
簡単なOpen処理のあと、ReadFile，WriteFile等のファイル操作APIを
使ってUSBデバイスとデーターのやり取りを行う事ができます。

ドライバーのインストール方法

uusbd.dllを使ってUSBデバイスを制御するにはあらかじめuusbd.sysを
組み込む必要があります。
具体的には次の手順で組み込みます。
１．制御したいUSBデバイスのVID,PIDを調べる
２．setupフォルダーにあるuusbd.infファイルの35,99行目付近にある
　　VID_XXXX&PID_YYYYのXXXX,YYYYの部分を１で調べたVID,PIDに変える。
３．MaxTransferSizeの値を変更したい場合は71～78行のコメントを外し
　　値を77行に記述する。
４．USBデバイスを接続する
５．インストールウイザードを進め、INFファイルの場所を指定する
　　ところで、２．で作成したINFファイルのフォルダーを指定する。
６．ウイザードを進めインストールを完了させる

簡単な動作テスト方法

pipedumpはC言語で記述されたuusbd.dllを使った簡単なアプリケーション
です。これは、USBデバイスのインターフェース０、パイプ番号０から
データを読み取りダンプ表示します。
このパイプにデータを出力するデバイスとしてUSBマウスや、USBジョイス
ティック等のHIDデバイスがあります。
これらは、インターラプト転送を使ってボタンの状態が変わる毎に数バイト
のデーターを送り出します。
pipedumpを実行する場合は、まず、ドライバーのインストール方法にある
方法で使用するHIDデバイスに対応したINFファイルを作成しドライバーを
インストールしておきます。

バージョンアップについて

不具合や、不足な機能、ご意見などありましたら作者までe-mailにて連絡ください。
次期バージョン作成の参考にさせていただきます。

URL	http://www.otto.to/~kasiwano/
e-mail	kasiwano@otto.to


このパッケージについて

このパッケージに含まれるソフトウエアの著作権は柏野政弘が保有します。
このパッケージに含まれるソフトウエアは、USBデバイス、USB関連ソフトの開発、
個人的な利用のために使用できます。
このパッケージを使用したことによる生じる如何なる問題も保証いたしません。
次の用途には使用できません。
１．製品の一部、または全部として使用
２．類似のソフトウエアを作成するために使用


変更履歴

1.00 初期バージョン

1.10 pipedump.c　複数のInterfaceを持つ場合の不具合を修正

1.20 uusbdcrw.c　read/write長をirpのParametersの長さにする
     Uusbd_OpenPipe_Overlappedの追加

1.30 pipeabort機能を追加

2.00 Uusbd_GetDeviceQualifierDescriptor,
     Uusbd_GetOtherSpeedConfigurationDescriptorの追加
     InfファイルによりMaxTransferSizeの値を設定できるようにする
