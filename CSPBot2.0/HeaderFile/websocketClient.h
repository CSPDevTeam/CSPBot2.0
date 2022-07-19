#pragma once
// self
#include "helper.h"
#include "cspbot20.h"
// third-party
#include <QObject>
#include <QtWebSockets>
#include <QDebug>
#include <QUrl>
#include <LightWSClient/WebSocketClient.h>
#include <magic_enum.hpp>

using namespace cyanray;

class WsClient : public QObject {
	Q_OBJECT
private:
	WebSocketClient ws;
	bool connected = false;
signals:
	void sigConnected(mTime nowTime);				   //连接成功
	void setUserImages(QString qqNum, QString qqNick); //更改UserImages
	void updateSendRecive(int send, int recive);	   //更新界面
public:
	explicit WsClient();
	// API
	bool sendTextMsg(string msg);
	bool connect(string url);
	bool close();
	bool shutdown();
	int getStatus();
};

std::string getConfig(std::string key);

typedef std::string groupNum;
typedef std::string qqNum;

enum Permission {
	OWNER,
	ADMINISTRATOR,
	MEMBER
};

struct messagePacket {
	std::string message;
	groupNum group;
	std::string groupName;
	qqNum qq;
	std::string memberName;
	std::string syncId;
	Permission perm;
};

//消息类型
inline std::unordered_map<std::string, std::string> messageType = {
	{"Image",		  " [图片]"		   },
	{"Quote",		  " [回复的消息]"},
	{"AtAll",		  " [@全体成员]"	},
	{"Face",			 " [QQ表情]"		},
	{"FlashImage",	   " [闪照]"			},
	{"Voice",		  " [语音]"		   },
	{"Xml",			" [XML消息]"		},
	{"Json",			 " [JSON消息]"	  },
	{"App",			" [小程序]"	   },
	{"Poke",			 " [戳一戳]"		},
	{"Dice",			 " [未知消息]"	  },
	{"ForwardMessage", " [转发的消息]"}
};

inline std::string transMessage(json j) {
	std::string msg = "";
	for (auto& i : j["data"]["messageChain"]) {
		//分析消息
		if (i["type"] == "Plain") {
			msg += " " + i["text"].get<std::string>();
		}
		else if (i["type"] == "At") {
			msg += " " + i["display"].get<std::string>();
		}
		else if (i["type"] == "File") {
			msg += " [文件]" + i["name"].get<std::string>();
		}
		else if (i["type"] == "MusicShare") {
			msg += " [音乐分享]" + i["musicUrl"];
		}
		else if (i["type"] == "Source") {
		}
		else {
			if (std::find(messageType.begin(), messageType.end(), i["type"]) != messageType.end()) {
				msg += messageType[i["type"]];
			}
			else {
				msg += " [未知消息]";
			}
		}
	}
	return msg;
}

inline messagePacket transMessagePacket(json j) {
	qqNum qq = std::to_string(j["data"]["sender"]["id"].get<long long>());
	std::string qqnick = j["data"]["sender"]["memberName"].get<std::string>();
	groupNum group = std::to_string(j["data"]["sender"]["group"]["id"].get<long long>());
	std::string message = transMessage(j);
	std::string syncId = j["syncId"].get<string>();
	std::string groupName = j["data"]["sender"]["group"]["name"].get<std::string>();
	std::string permissionString = j["data"]["sender"]["permission"].get<std::string>();
	auto permission = magic_enum::enum_cast<Permission>(permissionString);
	messagePacket msgPacket = {
		message,
		group,
		groupName,
		qq,
		qqnick,
		syncId,
		permission.value()};
	return msgPacket;
}

class Mirai : public QThread {
	Q_OBJECT
protected:
	void run();

public:
	Mirai();
	inline ~Mirai(){
		// delete mirai;
	};
	bool login();
	void sendGroupMsg(std::string group, std::string msg, bool callback = true);
	void sendAllGroupMsg(std::string msg, bool callback = true);
	void recallMsg(std::string target, bool callback = true);
	void botProfile();
	void changeName(std::string qq, std::string group, std::string name);
	void send_app(std::string group, std::string code);
	void SendPacket(std::string packet);
	bool connectMirai();
	bool logined = false;

	// Callback Event
	void onText(WebSocketClient& client, string msg);
	void onError(WebSocketClient& client, string msg);
	void onLost(WebSocketClient& client, int code);
signals:
	void setUserImages(QString qqNum, QString qqNick);		   //更改UserImages
	bool OtherCallback(QString listener, StringMap args = {}); // Callback
	void sigLogin();										   //登录成功
	void updateSendRecive(int send, int recive);			   //更新界面
	void signalConnect(mTime time);							   //连接成功传输
	void signalMiraiMessageBox();							   // Mirai弹窗
	void sendServerCommand(QString cmd);					   //输入命令
	void packetCallback(QString msg);						   //自定义包
private slots:
	void slotUpdateSendRecive(int send, int recive);
	void slotConnected(mTime time);						   //连接成功
	void slotSetUserImages(QString qqNum, QString qqNick); //更新用户图片
private:
	void selfGroupCatchLine(messagePacket message);

	// WsClient* wsc;
};

class CommandAPI : public QObject {
	Q_OBJECT
signals:
	void signalStartServer();
	void signalCommandCallback(QString cmd, StringVector args);
	void Callback(QString listener, StringMap args);

public:
	explicit CommandAPI(){};
	std::vector<std::string> SplitCommand(const std::string& parent);
	void CustomCmd(std::string cmd, std::string group);
};