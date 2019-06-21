import os
from datetime import datetime
import slackbot_settings

class FileOperat:

    def getfilesavepath():
        return slackbot_settings.SAVE_PATH


    def gettotalsavefilepath():
        savepath = slackbot_settings.SAVE_PATH
        # savepath = self.getfilesavepath()
        fltimestamp ="All-" + datetime.now().strftime("%Y%m%d-%H%M%S")
        Exp = ".log"
        mypath = savepath + fltimestamp + Exp
        print(mypath)
        return mypath

    def checkfileexist(self, path):
        result = 'ok'
        if os.path.exists(path):
            print("あるある")
        else:
            print("なしなし")
            result = 'non'
        return result

    # 指定ファイル名をトータルファイルに追加する
    def save2totalfile(self, orgfilepath):
        flexist = self.gettotalsavefilepath()
        if flexist == 'ok':
            # ファイル保存
            flexist='ok'
        else:
            # エラーそのまま返す
            flexist='no'
        return flexist
