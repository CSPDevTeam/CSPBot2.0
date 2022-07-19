import bot
import re

logger = bot.Logger("玩家列表")
lastLog = ""
sendGroup = "0000"

def onConsoleUpdate(data):
    global lastLog,sendGroup
    line = data["line"]

    #检测是否符合查询格式
    matchObj = re.search(r"are\s(.+)\/(.+)\splayers",lastLog)
    if(matchObj):
        nowNum = matchObj.group(1)
        totalNum = matchObj.group(2)
        li = line.split("] ")[1]
        if(li == ""):
            li = "无"
        bot.sendAllGroupMsg("当前在线玩家: "+nowNum+"/"+totalNum+"\n玩家列表:"+li)

    lastLog = line

def commandCallback(data):
    if(bot.getServerStatus()):
        bot.runCommand("list")
    else:
        bot.sendAllGroupMsg(data[0],"服务器未启动")

def Register():
    return {
        "name": "玩家列表",
        "info": "查询在线玩家列表",
        "Author":"HuoHua",
        "Version":"0.0.1"
    }


def InitPlugin():
    bot.setListener("onConsoleUpdate",onConsoleUpdate)

    #注册命令
    bot.registerCommand("queryPlayer",commandCallback)
    logger.info("玩家列表加载完成,作者:HuoHua")

InitPlugin()