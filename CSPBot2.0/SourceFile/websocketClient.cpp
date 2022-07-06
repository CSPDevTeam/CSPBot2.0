//引入头文件
#include "websocketClient.h"
#include "global.h"
//#include "CPython.h"
#include "logger.h"
#include <qmessagebox.h>
#include <qdebug.h>
#include <string>
#include "helper.h"
#include "server.h"

using namespace std;


string getConfig(string key);
//构建Client
Logger mirai_logger("Mirai");
int sendMsg = 0, reciveMsg = 0; //收发消息

//API
bool connectMirai() {
	bool connected = mirai->login();
	if (connected) {
		mirai_logger.info(u8"连接到Mirai成功");
	}
	else {
		QMessageBox::critical(window, u8"Mirai错误", u8"无法连接到Mirai", QMessageBox::Ok);
		mirai_logger.error(u8"无法连接到 Mirai");
	}

	mirai->botProfile();
	return connected;
}


WsClient::WsClient() {
	ws.OnTextReceived([]
	(WebSocketClient& client, string msg) {
			reciveMsg += 1;
			mirai->onText(client, msg);
	});
	ws.OnError([]
	(WebSocketClient& client, string msg) {mirai->onError(client, msg);});
	ws.OnLostConnection([]
	(WebSocketClient& client, int code) {mirai->onLost(client, code);});
}

int WsClient::getStatus() {
	return (int)ws.GetStatus();
}

bool WsClient::connect(string url) {
	try {
		ws.Connect(url);
		this->connected = true;
		mTime nowTime = time(NULL);
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
		catch (std::runtime_error e) {}
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
		catch (std::runtime_error e) {}
	}
	return false;
}

bool WsClient::shutdown() {
	if (connected) {
		try {
			ws.Shutdown();
			mirai->logined = false;
			mirai_logger.warn(u8"Mirai已断开连接");
			reciveMsg = 0;
			sendMsg = 0;
			emit updateSendRecive(sendMsg, reciveMsg);
			emit sigConnected(0);
			emit setUserImages("", "");
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

Mirai::Mirai() {
	wsc = new WsClient();
	connect(wsc, SIGNAL(updateSendRecive(int, int)), this, SLOT(slotUpdateSendRecive(int,int)));
	connect(wsc, SIGNAL(sigConnected(mTime)), this, SLOT(slotConnected(mTime)));
	connect(wsc, SIGNAL(setUserImages(QString,QString)), this, SLOT(slotSetUserImages(QString, QString)));
}

void Mirai::selfGroupCatchLine(messagePacket message) {
	YAML::Node regular = YAML::LoadFile("data/regular.yml");
	YAML::Node config = YAML::LoadFile("config/config.yml");

	vector<Regular> regularList;

	//读取正则组
	for (YAML::Node i : regular) {
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
		else { regular_action = regularAction::Command; };

		//转换来源
		regularFrom regular_from;
		transform(From.begin(), From.end(), From.begin(), ::tolower);
		if (From == "group") { regular_from = regularFrom::group; }
		else { regular_from = regularFrom::console; };
		Regular regular = { Helper::stdString2QString(mRegular),Helper::stdString2QString(Action),regular_action,regular_from,Permissions };
		//推入vector
		regularList.push_back(regular);
	}
	//使用正则组
	for (Regular i : regularList) {
		QRegExp r(i.regular);
		int r_pos = r.indexIn(Helper::stdString2QString(message.message));
		//bool qqAdmin = std::find(config["admin"].begin(), config["admin"].end(), message.qqNum) != config["admin"].end();
		bool qqAdmin = true;

		//执行操作
		if (r_pos > -1 && i.from == regularFrom::group && (i.permission == false || (i.permission == true && qqAdmin))) {
			int num = 0;
			for (auto& replace : r.capturedTexts()) {
				i.action = Helper::stdString2QString(
					Helper::replace(Helper::QString2stdString(i.action),
						"$" + std::to_string(num), 
						Helper::QString2stdString(replace)
					)
				);
				num++;
			}
			//执行操作
			string cmd = fmtConsole::FmtGroupRegular(message.qq, message.memberName, Helper::QString2stdString(i.action));
			if (i.type == regularAction::Console) {
				server->sendCmd(cmd + "\n");
			}
			else if (i.type == regularAction::Group) {
				mirai->sendAllGroupMsg(cmd);
			}
			else {
				commandApi->CustomCmd(cmd, message.group);
			}
		}
	}
}

//Msg
void Mirai::onText(WebSocketClient& client, string msg) {
	json msg_json = json::parse(msg);
	string syncId = msg_json["syncId"].get<string>();
	emit updateSendRecive(sendMsg, reciveMsg); //更新
	mirai_logger.debug(msg_json.dump());
	//window->PacketCallback(msg);
	//登录包
	if (syncId == "1") {
		//设置UserImage
		string qqNick = msg_json["data"]["nickname"].get<string>();
		mirai_logger.info(u8"{}登录 Mirai 成功", qqNick);
		logined = true;
		emit OtherCallback("onLogin");
		emit setUserImages(Helper::stdString2QString(getConfig("qq")), Helper::stdString2QString(qqNick));
	}
	//发消息包
	else if (syncId == "2") {
		int msgId = msg_json["data"]["messageId"];
		if (msgId == -1) {
			mirai_logger.warn(u8"已发出信息但可能遭到屏蔽");
		}
	}
	else if (syncId == "-1") {
		//消息处理
		if (msg_json["data"].find("type") != msg_json["data"].end() && msg_json["data"]["type"] == "GroupMessage") {
			messagePacket msgPacket = transMessagePacket(msg_json);

			//vector<string> allowGroup;
			std::ifstream fin("config/config.yml");
			YAML::Node config = YAML::Load(fin);
			bool inGroup = false;
			for (auto i : config["group"]) {
				if (i.as<string>() == msgPacket.group) {
					inGroup = true;
					break;
				}
			}

			if (inGroup) {
				msgPacket.message = msgPacket.message.erase(0, 1);
				//存储结构体

				//Callback
				std::unordered_map<string, string> args;
				args.emplace("group", msgPacket.group);
				args.emplace("msg", msgPacket.message);
				args.emplace("qq", msgPacket.qq);
				args.emplace("qqnick", msgPacket.memberName);
				bool send = emit OtherCallback("onReceiveMsg", args);
				qDebug() << "callBack:" << send;
				if (send) {
					selfGroupCatchLine(msgPacket);
				}
				

			}
		}
		//事件处理(群成员改名)
		else if (msg_json["data"].find("type") != msg_json["data"].end() && \
			msg_json["data"]["type"] == "MemberCardChangeEvent") {

		}
	}

}

//相应连接报错
void Mirai::onError(WebSocketClient& client, string msg) {
	mirai_logger.error("WebsocketClient error:{}", msg);
	logined = false;
	reciveMsg = 0;
	sendMsg = 0;
	emit updateSendRecive(sendMsg, reciveMsg);
	emit OtherCallback("onConnectError");
}

//响应连接丢失
void Mirai::onLost(WebSocketClient& client, int code) {
	mirai_logger.error("WebsocketClient Connect Lost,errorCode:{}", code);
	logined = false;
	reciveMsg = 0;
	sendMsg = 0;
	emit updateSendRecive(sendMsg, reciveMsg);
	emit OtherCallback("onConnectLost");
	emit setUserImages("","");
	slotConnected(0);
}

//更新界面
void Mirai::slotUpdateSendRecive(int send, int recive) {
	emit updateSendRecive(send, recive);
}

void Mirai::slotConnected(mTime time) {
	emit signalConnect(time);
}

void Mirai::slotSetUserImages(QString qqNum, QString qqNick) {
	emit setUserImages(qqNum, qqNick);
}

bool Mirai::login() {
	try {
		string qq = getConfig("qq");
		string key = getConfig("key");
		string wsUrl = getConfig("connectUrl");

		//拼接URL
		string url = wsUrl + "/all?verifyKey=" + key + "&qq=" + qq;
		return wsc->connect(url);
	}
	catch (const char e) {
		return false;
	}
}

void Mirai::botProfile() {
	string mj = "{\"syncId\": 1,\"command\" : \"botProfile\",\"subCommand\":null,\"content\":{}}";
	wsc->sendTextMsg(mj);
}

void Mirai::sendGroupMsg(string group, string msg, bool callback) {
	if (logined) {
		string mj = "{\"syncId\": 2, \"command\":\"sendGroupMessage\", \"subCommand\" : null,\
					\"content\": {\"target\":" + group + ", \"messageChain\": [{ \"type\":\"Plain\", \"text\" : \"" + msg + "\"}]}}";
		std::unordered_map<string, string> args;
		args.emplace("group", group);
		args.emplace("msg", msg);
		bool send = emit OtherCallback("onSendMsg", args);
		if (send) {
			wsc->sendTextMsg(mj);
		}
		
	}

}

void Mirai::recallMsg(string target, bool callback) {
	//recall
	if (logined) {
		string mj = "{\"syncId\": 3,\"command\" : \"recall\",\"subCommand\":null,\"content\":{\"target\":" + target + "}}";
		std::unordered_map<string, string> args;
		args.emplace("target", target);
		bool send = emit OtherCallback("onRecall", args);
		if (send) {
			wsc->sendTextMsg(mj);
		}
		
	}
}

void Mirai::sendAllGroupMsg(string msg, bool callback) {
	YAML::Node config = YAML::LoadFile("config/config.yml")["group"];
	for (auto igroup : config) {
		string group = std::to_string(igroup.as<long long>());
		if (logined) {
			sendGroupMsg(group, msg, callback);
		}
	}

}

void Mirai::changeName(string qq, string group, string name) {
	string packet_Json = "{\"syncId\": 4,\"command\" : \"memberInfo\",\"subCommand\":\"update\",\"content\":{\"target\":" + group + ",\"memberId\":" + qq + ",\"info\" : {\"name\":" + name + "}}}";
	if (logined) {
		wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::send_app(string group, string code) {
	string packet_Json = "{\"syncId\": 5,\"command\" : \"sendGroupMessage\",\"subCommand\" : null,\"content\" : {\"target\":" + group + ",\"messageChain\" : [{\"type\": \"App\",\"content\" :" + code + "}]}}";
	if (logined) {
		wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::SendPacket(string packet) {
	wsc->sendTextMsg(packet);
}
