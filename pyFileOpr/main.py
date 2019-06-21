
import SlackMod
from FileOperation import fileoperat

if __name__ == "__main__":
    # リモートからのアタッチ待ち
#    ptvsd.enable_attach(address=('0.0.0.0', 5678))
#    ptvsd.wait_for_attach()

    path = fileoperat.FileOperat.getfilesavepath()

    slack = SlackMod.SlackApi(path)
    slack.run()
