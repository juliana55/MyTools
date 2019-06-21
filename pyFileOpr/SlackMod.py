# coding: utf-8
import os
from slacker import Slacker
from slackbot.bot import Bot
import slackbot_settings
import requests
import codecs
from FileOperation.fileoperat import FileOperat


class SlackApi:
    """Slack Api"""
    def __init__(self, path):
        self.channel = 'iot'
        self.slacker = Slacker(slackbot_settings.API_TOKEN)
        self.file_types = ['png', 'gif', 'jpg', 'text']
        # self.save_directly = "c:/download/"
        self.save_directly = path
        if os.path.isdir(self.save_directly) != True:
            os.makedirs(self.save_directly, exist_ok=True)

    def upload(self, filename):
        self.slacker.files.upload(file_=filename, channels=self.channel)

    def download(self, file_info):
        file_name = file_info['name']
        url_private = file_info['url_private_download']
        # 保存対象のファイルかチェックする
        if file_info['filetype'] in self.file_types:
            # ファイルをダウンロード
            self.file_download(url_private, self.save_directly + file_name)

            # ファイルまとめチェック
            path = FileOperat.getTotalsavefilepath()

            # まとめ保存
            result = FileOperat.save2totalfile(path)
            if result == 'ok':
                return 'ok'
            else:
                return result
            # return 'ok'
        else:
            # 保存対象外ファイル
            return 'file type is not applicable.'
 
    def file_download(self, download_url, save_path):
        content = requests.get(
            download_url,
            allow_redirects=True,
            headers={'Authorization': 'Bearer %s' % slackbot_settings.API_TOKEN}, stream=True
        ).content
        # 保存する
        target_file = codecs.open(save_path, 'wb')
        target_file.write(content)
        target_file.close()

    def SendMessage(self, Message):
        self.slacker.chat.post_message(self.channel, Message, as_user=True)

    def run(self):
        bot = Bot()
        bot.run()

