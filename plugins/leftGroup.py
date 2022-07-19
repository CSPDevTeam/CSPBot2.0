import bot

logger = bot.Logger("退群删白")
commandTemp = []

def processingPackage(data):
    groupId = str(data["member"]["group"]["id"])
    userId = str(data["member"]["id"])
    #检测是否是管理的群
    if(str(groupId) not in bot.getGroup()):
        return

    #查询是否有绑定
    bind = bot.queryData("qq",userId)
    if(len(bind.keys()) == 0):
        return

    #解绑
    bot.unbind(userId)
    logger.info("已解绑QQ:%s" % userId)
    if(bot.getServerStatus()):
        bot.runCommand("allowlist remove "+bind["playerName"])
    else:
        commandTemp.append("allowlist remove "+bind["playerName"])

def onConsoleOutPut(data):
    line = data["line"]
    if("Done" in line) or ("Started" in line):
        for i in commandTemp:
            bot.runCommand(i)
            commandTemp.remove(i)

def receivePacket(data):
    packetData = data["msg"]
    if(packetData["syncId"] == "-1"):
        if("type" in packetData["data"]):
            if(packetData["data"]["type"] == "MemberLeaveEventQuit" or\
                packetData["data"]["type"] == "MemberLeaveEventKick"):
                processingPackage(packetData["data"])

def Register(): #注册插件
    return {
        "name": "退群删白",
        "info": "退群自动删白名单",
        "Author":"HuoHua",
        "Version":"0.0.1"
    }

def initPlugin():
    bot.setListener("onReceivePacket",receivePacket)
    bot.setListener("onConsoleUpdate",onConsoleOutPut)
    if(bot.getAPIVersion() < 1):
        logger.error("API版本过低,请更新CSPBot后使用.")

initPlugin()