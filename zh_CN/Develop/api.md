# 监听器
- `onServerStart 当服务器被开启时`
- `onServerStop 当服务器被停止时`
- `onSendCommand 当输入命令时`
- `onReceiveMsg 当收到消息时`
- `onReceivePacket 当收到包时`
- `onStop 当程序要停止时 可拦截`
- `onLogin 当机器人登陆后`
- `onImport 当有模块被import的时候`
- `onSendMsg 当发送消息时`
- `onRecall 当撤回消息时`
- `onConnectError 当Websocket连接出现Error时`
- `onConnectLost 当Websocket连接丢失时`

>[!tip]
>
>推荐使用.pyi文件进行开发👉[Link](https://github.com/CSPBot-Devloper/PyDevelopModule)👈


## 日志API
#### 输出日志
- `Logger.info` 输出info信息
- `Logger.debug` 输出debug信息
- `Logger.error` 输出error信息
- `Logger.warn` 输出warn信息
```python
import bot
logger = bot.Logger("我是个测试插件") #初始化Logger
logger.debug("我是一个DEBUG提示") #输出DEBUG信息
logger.info("我是一个INFO提示") #输出INFO信息
logger.warn("我是一个WARN提示") #输出WARN信息
logger.error("我是一个ERROR提示") #输出ERROR信息
```

## CSPBot API
#### 获取Bot版本字符串
`bot.getVersion()`
- 返回值：版本字符串
- 返回值类型：`str`
```python
import bot
version = bot.getVersion()
```

#### 设置监听器
`bot.setListener(event,function)`
- 参数：

  - event : `str`  
    事件详见[监听器](zh-cn/开发文档/API文档?id=监听器)

  - function : `function`  
    回调函数

- 返回值：无
```python
import bot
logger = bot.Logger("Example")
def onReceiveMsg(data):
    logger.info("我收到了消息:"+data["msg"])
def onServerStart():
		logger.info("服务器被开启了")
bot.setListener("onReceiveMsg",onReceiveMsg)
bot.setListener("onServerStart",onServerStart)
```
- `注:有些监听器是无需参数的，即def的括号为空`

#### 获取所有API列表
`bot.getAllAPIList()`
- 参数：无
- 返回值：API列表
- 返回值类型：`list`
```python
import bot
api = bot.getAllAPIList()
```

#### 弹窗
`bot.tip(type,title,content,options)`
- 参数：

	- event : `type`  
    枚举弹窗类型
		|  类型   | 描述  |
		|  ----  | ----  |
		| information  | 消息 |
		| question  | 询问 |
		| warning  | 警告 |
		| critical  | 错误 |

  - title : `str`  
    窗口标题

  - content : `str`  
    窗口内容

  - options : `list`  
    按钮枚举数组 见[按钮枚举值](zh-cn/开发文档/按钮枚举.md)
	

- 返回值：按钮枚举值
- 返回值类型：`str`
```python
import bot
button = bot.tip(
    "question",
		"测试窗体",
		"我是一个测试窗体",
		["Yes","No"]
)
```
#### 获取BE的Motd信息
`bot.motdbe(ip_port)`
- 参数：

	- ip_port : `str`  
    IP及端口 例:127.0.0.1:19132
- 返回值：Motd后的json字符串
- 返回值类型：`str`
```python
import bot
motd = bot.motdbe("127.0.0.1:19132")
```

#### 获取JE的Motd信息
`bot.motdje(ip_port)`
- 参数：

	- ip_port : `str`  
    IP及端口 例:127.0.0.1:25565
- 返回值：Motd后的json字符串
- 返回值类型：`str`
```python
import bot
motd = bot.motdje("127.0.0.1:25565")
```

#### 注册一个指令
`bot.registerCommand(cmd,function)`
- 参数：

  - cmd : `str`  
    需要注册的指令

  - function : `function`  
    回调函数

- 返回值：是否注册成功
- 返回值类型：`bool`
```python
import bot
def cmdcb(data):
    logger.info("收到命令: "+str(data))
bot.registerCommand("test",cmdcb)
```

#### 获取CSPBot当前的API版本
`bot.getAPIVersion()`
- 参数：

  - 无

- 返回值：API版本
- 返回值类型：`int`
```python
import bot
apiVersion = bot.getAPIVersion():
```

#### 获取管理的群列表
`bot.getGroup()`
- 参数：

  - 无

- 返回值：API版本
- 返回值类型：`list[str]`
```python
import bot
groupList = bot.getGroup():
```

#### 获取管理员列表
`bot.getAdmin()`
- 参数：

  - 无

- 返回值：API版本
- 返回值类型：`list[str]`
```python
import bot
adminList = bot.getAdmin():
```

## Xbox API
#### 绑定XBox
`bot.bind(name,qq)`
- 参数：

  - name : `str`  
    需要绑定的XBoxid

  - qq : `string`  
    需要绑定的QQ号

- 返回值：是否绑定成功
- 返回值类型：`bool`
```python
import bot
binded = bot.bind("test","114514")
```

#### 绑定XBox
`bot.unbind(qq)`
- 参数：


  - qq : `string`  
    需要解绑的QQ号

- 返回值：是否解绑成功
- 返回值类型：`bool`
```python
import bot
unbinded = bot.unbind("114514")
```

#### 查询Xbox相关信息
`bot.queryData(type,key)`
- 参数：

  - type : `str`  
    请填入`qq`或`xuid`或`player`

  - qq : `string`  
    需要绑定的QQ号

- 返回值：查询到的信息(没查到返回空Dict)
- 返回值类型：`dict`
```python
import bot
data = bot.queryData("qq","114514")
```


## 服务器API
#### 向服务端发送命令
`bot.runCommand(cmd)`
- 参数
    - cmd: `str`
- 返回值：是否执行成功
- 返回值类型：`bool`
```python
import bot
runable = bot.runCommand("stop")
```

#### 查询服务器开启状态
`bot.getServerStatus()`
- 参数：

  - 无

- 返回值：是否开启
- 返回值类型：`bool`
```python
import bot
started = bot.getServerStatus():
```

## Mirai API
#### 向指定的群发送消息
`bot.sendGroupMsg(group,msg)`
- 参数：

  - group : `str`  
    待发送的群聊

  - msg : `str`  
    待发送的消息

- 返回值：无
```python
import bot
bot.sendGroupMsg("123456789","测试消息")
```

#### 向所有的群发送消息
`bot.sendAllGroupMsg(msg)`
- 参数：

  - msg : `str`  
    待发送的消息

- 返回值：无
```python
import bot
bot.sendAllGroupMsg("测试消息")
```

#### 向所有的群发送消息
`bot.recallMsg(target)`
- 参数：

  - target : `str`  
    待撤回消息的Target

- 返回值：无
```python
import bot
bot.recallMsg("123456")
```

#### 向指定的群发送APP消息
`bot.sendApp(group,code)`
- 参数：

  - group : `str`  
    待发送的群聊

  - code : `str`  
    待发送的APP Code

- 返回值：无
```python
import bot
bot.sendApp("123456","<xml></xml")
```

#### 向Mirai发送自定义包
`bot.sendPacket(packet)`
- 参数：

  - packet : `str`  
    待发送的包

- 返回值：无
```python
import bot
bot.sendPacket("{...}")
```




