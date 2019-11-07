echo off
set RVL=01080115
echo //-------------------------------------------------------------------
echo //	
echo //	C:\PROJECT5\AMS\AMS210E\CPanel\LD_ROM_Little\
echo //	のフォルダからCAMS210E.motをコピーしてrenameする。
echo //	
echo //	RVLを変えた場合はこのBATファイルをエディタで開いて、
echo //	copy行のIP******.m01を編集する。
echo //	
echo //	各自のPCのCFドライブは異なるため、PRGファイルのコピー先は
echo //	環境に合わせて編集の必要がある。
echo //	
echo //	BATファイルの実行を途中で止めたい場合は”CTL+C”を押す。
echo //	
echo //	pause文はキー入力を求めるコマンドで、必要なければ”：”を付けると
echo //	実行されない。
echo //	
echo //-------------------------------------------------------------------
:pause
echo //
echo //-------------------------------------------------------------------
echo //	ファイルをコピーします。
echo //-------------------------------------------------------------------
:pause
echo //
copy C:\PROJECT5\AMS\AMS210E\CPanel\LD_ROM_Little\CAMS210E.mot C:\PROJECT5\root\AMS210E\IP%RVL%.m01
:pause 
echo //-------------------------------------------------------------------
echo //	変換TOOLを立ち上げます。
echo //-------------------------------------------------------------------
:pause 
C:\project5\root\henkantool4\HenKanTool4.exe C:\PROJECT5\root\MANAGER\AMSEN_IP420_BP%RVL%.he2
echo //
echo //-------------------------------------------------------------------
echo //	CFにPRGファイルをコピーします。
echo //	コピーしない場合は”CTL+C”で中断します。
echo //-------------------------------------------------------------------
pause
:copy C:\PROJECT5\root\Prog\AMSEN\BP%RVL%\*.* G:\Prog\AMSEN\BP%RVL%\
pause


