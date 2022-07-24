#include "ws_client.h"
#include "global.h"
#include "logger.h"
#include "helper.h"
#include "server.h"
#include "config_reader.h"
#include <qmessagebox.h>
#include <qdebug.h>

int sendMsg = 0, reciveMsg = 0; //收发消息

string transMessage(json j) {
	string msg = "";
	for (auto& i : j["data"]["messageChain"]) {
		//分析消息
		if (i["type"] == "Plain") {
			msg += " " + i["text"].get<string>();
		}
		else if (i["type"] == "At") {
			msg += " " + i["display"].get<string>();
		}
		else if (i["type"] == "File") {
			msg += " [文件]" + i["name"].get<string>();
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
messagePacket transMessagePacket(json j) {
	qqNum qq = std::to_string(j["data"]["sender"]["id"].get<long long>());
	string qqnick = j["data"]["sender"]["memberName"].get<string>();
	groupNum group = std::to_string(j["data"]["sender"]["group"]["id"].get<long long>());
	string message = transMessage(j);
	string syncId = j["syncId"].get<string>();
	string groupName = j["data"]["sender"]["group"]["name"].get<string>();
	string permissionString = j["data"]["sender"]["permission"].get<string>();
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

// API
bool Mirai::connectMirai() {
	bool connected = g_mirai->login();
	if (connected) {
		g_mirai_logger.info("连接到Mirai成功");
	}
	else {
		emit signalMiraiMessageBox();
		emit signalConnect(0);
		g_mirai_logger.error("无法连接到 Mirai");
	}

	g_mirai->botProfile();
	return connected;
}

WsClient::WsClient() {
	ws.OnTextReceived([](WebSocketClient& client, string msg) {
		reciveMsg += 1;
		g_mirai->onText(client, msg);
	});
	ws.OnError([](WebSocketClient& client, string msg) { g_mirai->onError(client, msg); });
	ws.OnLostConnection([](WebSocketClient& client, int code) { g_mirai->onLost(client, code); });
}

int WsClient::getStatus() {
	return (int)ws.GetStatus();
}

bool WsClient::connect(string url) {
	try {
		ws.Connect(url);
		this->connected = true;
		time_t nowTime = time(NULL);
		emit sigConnected(nowTime);
		return true;
	}
	catch (std::runtime_error e) {
		return false;
	}
}

bool WsClient::close() {
	if (this->connected) {
		try {
			ws.Close();
			return true;
		}
		catch (std::runtime_error e) {
		}
	}
	return false;
}

bool WsClient::sendTextMsg(string msg) {
	if (connected) {
		try {
			ws.SendText(msg);
			sendMsg += 1;
			emit updateSendRecive(sendMsg, reciveMsg);
			return true;
		}
		catch (std::runtime_error e) {
		}
	}
	return false;
}

bool WsClient::shutdown() {
	if (connected) {
		try {
			ws.Shutdown();
			g_mirai->logined = false;
			g_mirai_logger.warn("Mirai已断开连接");
			reciveMsg = 0;
			sendMsg = 0;
			emit updateSendRecive(sendMsg, reciveMsg);
			emit sigConnected(0);
			emit setUserImages("", "");
			return true;
		}
		catch (std::runtime_error e) {
		}
	}
	return false;
}

Mirai::Mirai() {
	g_wsc = new WsClient();
	connect(g_wsc, SIGNAL(updateSendRecive(int, int)), this, SLOT(slotUpdateSendRecive(int, int)));
	connect(g_wsc, SIGNAL(sigConnected(mTime)), this, SLOT(slotConnected(mTime)));
	connect(g_wsc, SIGNAL(setUserImages(QString, QString)), this, SLOT(slotSetUserImages(QString, QString)));
}

void Mirai::run() {
	g_mirai_logger.info("Mirai启动中...");
	connectMirai();
	g_mirai_logger.info("Mirai启动完成");
}

void otherSendCmd(string cmd);

void Mirai::selfGroupCatchLine(messagePacket message) {
	vector<Regular> regularList;

	//读取正则组
	for (auto i : g_regular.raw()) {
		string mRegular = i["Regular"].as<string>();
		string Action = i["Action"].as<string>();
		string From = i["From"].as<string>();
		bool Permissions = i["Permissions"].as<bool>();

		//转换type
		string Action_type = Action.substr(0, 2);
		regularAction regular_action;
		if (Action_type == "<<") {
			regular_action = regularAction::Console;
			Action = Action.erase(0, 2);
		}
		else if (Action_type == ">>") {
			regular_action = regularAction::Group;
			Action = Action.erase(0, 2);
		}
		else {
			regular_action = regularAction::Command;
		};

		//转换来源
		regularFrom regular_from;
		transform(From.begin(), From.end(), From.begin(), ::tolower);
		if (From == "group") {
			regular_from = regularFrom::group;
		}
		else {
			regular_from = regularFrom::console;
		};
		Regular regular = {QString::fromStdString(mRegular), QString::fromStdString(Action), regular_action, regular_from, Permissions};
		//推入vector
		regularList.push_back(regular);
	}
	//使用正则组
	for (Regular i : regularList) {
		auto regex = QRegularExpression(i.regular);
		auto match = regex.match(QString::fromStdString(message.message));

		bool qqAdmin = false;
		for (auto j : g_config["admin"]) {
			if (j.as<string>() == message.qq) {
				qqAdmin = true;
				break;
			}
		}

		//执行操作
		if (match.hasMatch() && i.from == regularFrom::group && (i.permission == false || (i.permission == true && qqAdmin))) {
			int num = 0;
			for (auto& replace : match.capturedTexts()) {
				i.action = QString::fromStdString(helper::replace(i.action.toStdString(), "$" + std::to_string(num), replace.toStdString()));
				num++;
			}
			//执行操作
			string cmd = fmtConsole::FmtGroupRegular(message, i.action.toStdString());
			if (i.type == regularAction::Console) {
				string fCmd = cmd + "\n";
				emit sendServerCommand(QString::fromStdString(fCmd));
			}
			else if (i.type == regularAction::Group) {
				g_mirai->sendAllGroupMsg(cmd);
			}
			else {
				g_cmd_api->CustomCmd(cmd, message.group);
			}
		}
	}
}

// Msg
void Mirai::onText(WebSocketClient& client, string msg) {
	json msg_json = json::parse(msg);
	string syncId = msg_json["syncId"].get<string>();
	emit updateSendRecive(sendMsg, reciveMsg); //更新
	g_mirai_logger.debug(msg_json.dump());
	emit packetCallback(QString::fromStdString(msg));
	//登录包
	if (syncId == "1") {
		//设置UserImage
		string qqNick = msg_json["data"]["nickname"].get<string>();
		g_mirai_logger.info("{}登录 Mirai 成功", qqNick);
		logined = true;
		emit OtherCallback("onLogin");
		string qqNum = GetConfig("qq");
		if (qqNum == "!failed!") {
			qqNum = "0";
		}
		emit setUserImages(QString::fromStdString(qqNum), QString::fromStdString(qqNick));
	}
	//发消息包
	else if (syncId == "2") {
		int msgId = msg_json["data"]["messageId"];
		if (msgId == -1) {
			g_mirai_logger.warn("已发出信息但可能遭到屏蔽");
		}
	}
	else if (syncId == "-1") {
		//消息处理
		if (msg_json["data"].find("type") != msg_json["data"].end() && msg_json["data"]["type"] == "GroupMessage") {
			messagePacket msgPacket = transMessagePacket(msg_json);

			bool inGroup = false;
			for (auto i : g_config["group"]) {
				if (i.as<string>() == msgPacket.group) {
					inGroup = true;
					break;
				}
			}

			if (inGroup) {
				msgPacket.message = msgPacket.message.erase(0, 1);
				//存储结构体

				// Callback
				std::unordered_map<string, string> args;
				args.emplace("group", msgPacket.group);
				args.emplace("msg", msgPacket.message);
				args.emplace("qq", msgPacket.qq);
				args.emplace("qqnick", msgPacket.memberName);
				emit OtherCallback("onReceiveMsg", args);
				selfGroupCatchLine(msgPacket);
			}
		}
		//事件处理(群成员改名)
		else if (msg_json["data"].find("type") != msg_json["data"].end() &&
			msg_json["data"]["type"] == "MemberCardChangeEvent") {
		}
	}
}

//相应连接报错
void Mirai::onError(WebSocketClient& client, string msg) {
	g_mirai_logger.error("WebsocketClient error:{}", msg);
	logined = false;
	reciveMsg = 0;
	sendMsg = 0;
	emit updateSendRecive(sendMsg, reciveMsg);
	emit OtherCallback("onConnectError");
}

//响应连接丢失
void Mirai::onLost(WebSocketClient& client, int code) {
	g_mirai_logger.error("WebsocketClient Connect Lost,errorCode:{}", code);
	logined = false;
	reciveMsg = 0;
	sendMsg = 0;
	emit updateSendRecive(sendMsg, reciveMsg);
	emit OtherCallback("onConnectLost");
	emit setUserImages("", "");
	slotConnected(0);
}

//更新界面
void Mirai::slotUpdateSendRecive(int send, int recive) {
	emit updateSendRecive(send, recive);
}

void Mirai::slotConnected(time_t time) {
	emit signalConnect(time);
}

void Mirai::slotSetUserImages(QString qqNum, QString qqNick) {
	emit setUserImages(qqNum, qqNick);
}

bool Mirai::login() {
	try {
		string qq = GetConfig("qq");
		string key = GetConfig("key");
		string wsUrl = GetConfig("connectUrl");
		if (qq == "!failed!") {
			qq = "0";
		}
		if (key == "!failed!") {
			key = "abcd";
		}
		if (wsUrl == "!failed!") {
			return false;
		}

		//拼接URL
		string url = wsUrl + "/all?verifyKey=" + key + "&qq=" + qq;
		return g_wsc->connect(url);
	}
	catch (...) {
		return false;
	}
}

void Mirai::botProfile() {
	string mj = "{\"syncId\": 1,\"command\" : \"botProfile\",\"subCommand\":null,\"content\":{}}";
	g_wsc->sendTextMsg(mj);
}

void Mirai::sendGroupMsg(string group, string msg, bool callback) {
	if (logined) {
		string mj = "{\"syncId\": 2, \"command\":\"sendGroupMessage\", \"subCommand\" : null,\
					\"content\": {\"target\":" +
			group + ", \"messageChain\": [{ \"type\":\"Plain\", \"text\" : \"" + msg + "\"}]}}";
		std::unordered_map<string, string> args;
		args.emplace("group", group);
		args.emplace("msg", msg);
		emit OtherCallback("onSendMsg", args);
		g_wsc->sendTextMsg(mj);
	}
}

void Mirai::recallMsg(string target, bool callback) {
	// recall
	if (logined) {
		string mj = "{\"syncId\": 3,\"command\" : \"recall\",\"subCommand\":null,\"content\":{\"target\":" + target + "}}";
		std::unordered_map<string, string> args;
		args.emplace("target", target);
		emit OtherCallback("onRecall", args);
		g_wsc->sendTextMsg(mj);
	}
}

void Mirai::sendAllGroupMsg(string msg, bool callback) {
	for (auto igroup : g_config["group"]) {
		string group = std::to_string(igroup.as<long long>());
		if (logined) {
			sendGroupMsg(group, msg, callback);
		}
	}
}

void Mirai::changeName(string qq, string group, string name) {
	string packet_Json = "{\"syncId\": 4,\"command\" : \"memberInfo\",\"subCommand\":\"update\",\"content\":{\"target\":" + group + ",\"memberId\":" + qq + ",\"info\" : {\"name\":" + name + "}}}";
	if (logined) {
		g_wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::send_app(string group, string code) {
	string packet_Json = "{\"syncId\": 5,\"command\" : \"sendGroupMessage\",\"subCommand\" : null,\"content\" : {\"target\":" + group + ",\"messageChain\" : [{\"type\": \"App\",\"content\" :" + code + "}]}}";
	if (logined) {
		g_wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::SendPacket(string packet) {
	g_wsc->sendTextMsg(packet);
}
