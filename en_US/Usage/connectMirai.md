>[!Warning]
>
>Some of this article has been translated by machine
## ⚙ Configure Mirai
1. Find Mirai
2. Open the `config/net.mamoe.mirai-api-http/settings.yml` file
3. Add webSocket in the Adapters area

```yaml
adapters:
- 'http'
- 'ws'
```

4. Start your Mirai

<img src="Pic/3.png" alt="Mirai-Http-API输出" style="zoom:60%"/>

5. Mirai is configured when the image above appears

## 🥽 Login robot
1. Enter 'login QQ id and password' in the Mirai console and press enter to login

2. Follow the instructions

- `Note: It is recommended to enter help on the console using Autologin to obtain help`

## 🔩 Configure the CSPBot
1. Open the `config/config.yml` file

<img src="Pic/3.png" alt=" mirai-http-API output " style="zoom:60%">

2. Paste `[ws adapter] is listening at ws://xxxx:xxxx` into `connectUrl`

```yaml
ConnectUrl: 'ws://127.0.0.1:8080'
```

3. Copy `XXXX` in `Http API Server is running with verifyKey: XXXX` to `key`

```yaml
key: 'ABCDEFG'
```

4. Write the QQ number of the robot into QQ

```yaml
qq: 123456789
```

5. Write 'group' as the group for sending messages

```yaml
group:
- 123456.
- 654321.
```