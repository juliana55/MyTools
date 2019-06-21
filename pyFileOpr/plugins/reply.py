# coding: utf-8

from slackbot.bot import respond_to
from slackbot.bot import listen_to
import slackbot_settings
import SlackMod
from FileOperation import fileoperat

path = fileoperat.FileOperat.getfilesavepath()
slack = SlackMod.SlackApi(path)

@respond_to('hello')
def reply_hello(message):
    message.reply('hello')

@listen_to("(.*)")
@respond_to("(.*)")
def reply_download(message, txt):
    if 'upload' in message.body:
        result = slack.download(message._body['files'][0])
        if result == 'ok':
            message.send('ファイルをダウンロードしました')
        elif result == 'file type is not applicable.':
            message.send('ファイルのタイプがダウンロード対象外です')
        else:
            message.send('ファイルのダウンロードに失敗しました')
