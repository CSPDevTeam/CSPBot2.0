>[!Warning]
>
>Some of this article has been translated by machine
# Listener
- `onServerStart When the server is turned on`
- `onServerStop When the server is stopped`
- `onSendCommand When entering a command`
- `onReceiveMsg When a message is received`
- `onReceivePacket When the package is received`
- `onStop When the program is about to stop`
- `onLogin When the robot lands`
- `onImport When a module is imported`
- `onSendMsg When sending a message`
- `onRecall When recalling a message`
- `onConnectError When error occurs in websocket connection`
- `onConnectLost When websocket connection is lost`

>[!tip]
>
>Recommended .pyi file for development👉[Link](https://github.com/CSPBot-Devloper/PyDevelopModule)👈


## Logging API
#### Output log
- `Logger.info` Output info information
- `Logger.debug` Output debug information
- `Logger.error` Output error information
- `Logger.warn` Output warn information
```python
import bot
logger = bot.Logger("I'm a test plug-in") #Initialize logger
logger.debug("I am a debug prompt") #Output debug information
logger.info("I am an info prompt") #Output info information
logger.warn("I'm a warning") #Output warn information
logger.error("I am an error prompt") #Output error information
```

## CSPBot API
#### Get BOT version string
`bot.getVersion()`
- Return value: version string
- return type：`str`
```python
import bot
version = bot.getVersion()
```

#### Set up listeners
`bot.setListener(event,function)`
- parameter：

  - event : `str`  
    See details of the event[Listener](en_US/Develop/api?id=listener)

  - function : `function`  
    Callback function

- Return value: None
```python
import bot
logger = bot.Logger("Example")
def onReceiveMsg(data):
    logger.info("I got the message:"+data["msg"])
def onServerStart():
		logger.info("The server is turned on")
bot.setListener("onReceiveMsg",onReceiveMsg)
bot.setListener("onServerStart",onServerStart)
```
- `Note: some listeners do not need parameters, that is, the parentheses of def are empty`

#### Get a list of all APIs
`bot.getAllAPIList()`
- Parameter: None
- Return value: API list
- return type：`list`
```python
import bot
api = bot.getAllAPIList()
```

#### pop-up notification
`bot.tip(type,title,content,options)`
- parameter：

	- event : `type`  
    Enumerate popup types
		|  type   | describe  |
		|  ----  | ----  |
		| information  | news |
		| question  | inquiry |
		| warning  | warning |
		| critical  | error |

  - title : `str`  
    Window title

  - content : `str`  
    Window content

  - options : `list`  
    Button enumeration array see[Button enumeration](en_US/Develop/buttom.md)
	

- Return value: button enumeration value
- Return value type:`str`
```python
import bot
button = bot.tip(
    "question",
		"Test form",
		"I am a test form",
		["Yes","No"]
)
```
#### Get MOTD information of bedrock edition server
`bot.motdbe(ip_port)`
- Parameters:

	- ip_port : `str`  
    IP and port examples:127.0.0.1:19132
- Return value: JSON string after MOTD
- Return value type:`str`
```python
import bot
motd = bot.motdbe("127.0.0.1:19132")
```

#### Get MOTD information of Java edition server
`bot.motdje(ip_port)`
- Parameters:

	- ip_port : `str`  
    IP and port examples:127.0.0.1:25565
- Return value: JSON string after MOTD
- Return value type:`str`
```python
import bot
motd = bot.motdje("127.0.0.1:25565")
```

#### Register an instruction
`bot.registerCommand(cmd,function)`
- Parameters:

  - cmd : `str`  
    Instructions requiring registration

  - function : `function`  
    Callback function

- Return value: whether the registration is successful
- Return value type:`bool`
```python
import bot
def cmdcb(data):
    logger.info("收到命令: "+str(data))
bot.registerCommand("test",cmdcb)
```

#### Get the current API version of cspbot
`bot.getAPIVersion()`
- Parameters:

  - None

- Return value: API version
- Return value type:`int`
```python
import bot
apiVersion = bot.getAPIVersion():
```

#### Get the list of managed groups
`bot.getGroup()`
- Parameters:

  - None

- Return value: API version
- Return value type:`list[str]`
```python
import bot
groupList = bot.getGroup():
```

#### Get administrator list
`bot.getAdmin()`
- Parameters:

  - nothing

- Return value: API version
- Return value type:`list[str]`
```python
import bot
adminList = bot.getAdmin():
```

## Xbox API
#### Bind XBOX
`bot.bind(name,qq)`
- Parameters:

  - name : `str`  
    Xboxid to bind

  - qq : `string`  
    QQ number to be bound

- Return value: whether the binding is successful
- Return value type:`bool`
```python
import bot
binded = bot.bind("test","114514")
```

#### Bind XBOX
`bot.unbind(qq)`
- Parameters:


  - qq : `string`  
    QQ number that needs to be unbound

- Return value: whether to unbind successfully
- Return value type:`bool`
```python
import bot
unbinded = bot.unbind("114514")
```

#### Query Xbox related information
`bot.queryData(type,key)`
- Parameters:

  - type : `str`  
    Please fill in`qq`or`xuid`or`player`

  - qq : `string`  
    QQ number to be bound

- Return value: information found (empty dict if not found)
- Return value type:`dict`
```python
import bot
data = bot.queryData("qq","114514")
```


## Server API
#### Send commands to the server
`bot.runCommand(cmd)`
- parameter
    - cmd: `str`
- Return value: whether the execution was successful
- return type：`bool`
```python
import bot
runable = bot.runCommand("stop")
```

#### Query server startup status
`bot.getServerStatus()`
- Parameters:

  - None

- Return value: enable or not
- Return value type:`bool`
```python
import bot
started = bot.getServerStatus():
```

## Mirai API
#### Send a message to the specified group
`bot.sendGroupMsg(group,msg)`
- Parameters:

  - group : `str`  
    Group chat to be sent

  - msg : `str`  
    Messages to be sent

- Return value: None
```python
import bot
bot.sendGroupMsg("123456789","Test message")
```

#### Send messages to all groups
`bot.sendAllGroupMsg(msg)`
- Parameters:

  - msg : `str`  
    Messages to be sent

- Return value: None
```python
import bot
bot.sendAllGroupMsg("Test message")
```

#### Send messages to all groups
`bot.recallMsg(target)`
- Parameters:

  - target : `str`  
    Target of the message to be recalled

- Return value: None
```python
import bot
bot.recallMsg("123456")
```

#### 向指定的群发送APP消息
`bot.sendApp(group,code)`
- Parameters:

  - group : `str`  
    Group chat to be sent

  - code : `str`  
    App code to be sent

- Return value: None
```python
import bot
bot.sendApp("123456","<xml></xml")
```

#### Send custom package to Mirai
`bot.sendPacket(packet)`
- Parameters:

  - packet : `str`  
    Packets to be sent

- Return value: None
```python
import bot
bot.sendPacket("{...}")
```




