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

//消息类型
unordered_map<string, string> messageType = {
	{"Image",u8" [图片]"},
	{"Quote",u8" [回复的消息]"},
	{"AtAll",u8" [@全体成员]"},
	{"Face",u8" [QQ表情]"},
	{"FlashImage",u8" [闪照]"},
	{"Voice",u8" [语音]"},
	{"Xml",u8" [XML消息]"},
	{"Json",u8" [JSON消息]"},
	{"App",u8" [小程序]"},
	{"Poke",u8" [戳一戳]"},
	{"Dice",u8" [未知消息]"},
	{"ForwardMessage",u8" [转发的消息]"}			
};

string getConfig(string key);
//构建Client
Logger mirai_logger("Mirai");

//正则组结构体
enum regularAction { Console, Group, Command };
enum regularFrom { group, console };
struct Regular {
	QString regular;
	string action;
	regularAction type;
	regularFrom from;
	bool permission;
};

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

//WebSocketClinet::WebSocketClinet(QObject* parent)
//	: QObject(parent){
//	m_pWebSocket = new QWebSocket();
//	connect(
//		m_pWebSocket, SIGNAL(connected()), 
//		this, SLOT(slotConnected())
//	);
//	connect(
//		m_pWebSocket, SIGNAL(disconnected()),
//		this, SLOT(slotDisconnected())
//	);
//	connect(
//		m_pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//		this, SLOT(slotError(QAbstractSocket::SocketError))
//	);
//}
//
//WebSocketClinet::~WebSocketClinet(){
//	if (m_pWebSocket != 0){
//		m_pWebSocket->deleteLater();
//		m_pWebSocket = 0;
//	}
//}
//// 连接websocket服务器的URL
//bool WebSocketClinet::connectUrl(QString url){
//	m_url = QUrl(url);
//	m_pWebSocket->open(m_url);
//	return true;
//}
//// 关闭websocket
//void WebSocketClinet::close(){
//	m_pWebSocket->close();
//}
//// 发送Text类型的消息
//void WebSocketClinet::sendTextMsg(const QString& message){
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__ , __LINE__,__FUNCTION__);
//		return;
//	}
//	//mirai_logger.debug("Send: {}", message);
//	m_pWebSocket->sendTextMessage(message);
//}
//// 发送Text类型的消息
//void WebSocketClinet::sendTextMsg(const string& message){
//	QString m_message = Helper::stdString2QString(message);
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__, __LINE__, __FUNCTION__);
//		return;
//	}
//	//mirai_logger.debug("Send: {}", message);
//	m_pWebSocket->sendTextMessage(m_message);
//}
//// 发送Binary类型的消息
//void WebSocketClinet::sendBinaryMsg(const QByteArray& data){
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__, __LINE__, __FUNCTION__);
//		return;
//	}
//	m_pWebSocket->sendBinaryMessage(data);
//}
//// 返回服务器连接状态
//bool WebSocketClinet::getConStatus(){
//	return m_bConnected;
//}
//// 连接成功
//void WebSocketClinet::slotConnected(){
//	mirai_logger.info(u8"Websocket连接成功.");
//	m_bConnected = true;
//	connect(m_pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(slotRecvTextMsg(QString)));
//	connect(m_pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotRecvBinaryMsg(QByteArray)));
//}
//// 断开连接
//void WebSocketClinet::slotDisconnected(){
//	mirai_logger.error(u8"Websocket已断开连接"); 
//	reconnect();
//}
//// 接受字符数据
//void WebSocketClinet::slotRecvTextMsg(QString message){
//	emit sigRecvTextMsg(message);
//}
//// 接受二进制数据
//void WebSocketClinet::slotRecvBinaryMsg(QByteArray message){
//	mirai_logger.debug("slotRecvBinaryMsg: {}", message);
//}
//// 响应报错
//void WebSocketClinet::slotError(QAbstractSocket::SocketError error){
//	mirai_logger.error("errorCode:{} error:{}", (int)error, Helper::QString2stdString(m_pWebSocket->errorString()));
//}
//// 断开重连
//void WebSocketClinet::reconnect() { 
//	mirai_logger.warn(u8"Websocket重连中..."); 
//	m_pWebSocket->abort(); 
//	m_pWebSocket->open(m_url);
//}

WsClient::WsClient() {
	ws.OnTextReceived([]
	(WebSocketClient& client, string msg) {mirai->onText(client, msg);});
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
	if (this->connected) {
		try {
			ws.SendText(msg);
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

bool WsClient::shutdown() {
	if (this->connected) {
		try {
			ws.Shutdown();
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

Mirai::Mirai() {
	wsc = new WsClient();
}

void Mirai::selfGroupCatchLine(Message message) {
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
		if (Action_type == "<<") { regular_action = regularAction::Console; }
		else if (Action_type == ">>") { regular_action = regularAction::Group; }
		else { regular_action = regularAction::Command; };

		//转换来源
		regularFrom regular_from;
		transform(From.begin(), From.end(), From.begin(), ::toupper);
		if (From == "group") { regular_from = regularFrom::group; }
		else { regular_from = regularFrom::console; };
		Regular regular = { Helper::stdString2QString(mRegular),Action.erase(0, 2),regular_action,regular_from,Permissions };
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
				i.action = Helper::replace(i.action, "$" + std::to_string(num), Helper::QString2stdString(replace));
				num++;
			}
			//执行操作
			string cmd = fmtConsole::FmtGroupRegular(message.qqNum, message.qqNick, i.action);
			if (i.type == regularAction::Console) {
				server->sendCmd(cmd + "\n");
			}
			else if (i.type == regularAction::Group) {
				mirai->sendAllGroupMsg(cmd);
			}
			else {
				Basic::Command::CustomCmd(cmd, message.group);
			}
		}
	}
}

//Msg
void Mirai::onText(WebSocketClient& client, string msg) {
	json msg_json = json::parse(msg);
	string syncId = msg_json["syncId"].get<string>();
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
			string qq = std::to_string(msg_json["data"]["sender"]["id"].get<long long>());
			string qqnick = msg_json["data"]["sender"]["memberName"].get<string>();
			string group = std::to_string(msg_json["data"]["sender"]["group"]["id"].get<long long>());
			string msg = "";

			vector<string> allowGroup;
			std::ifstream fin("config/config.yml");
			YAML::Node config = YAML::Load(fin);
			for (auto i : config["group"]) {
				allowGroup.push_back(i.as<string>());
			}

			if (std::find(allowGroup.begin(), allowGroup.end(), group) != allowGroup.end()) {
				for (auto& i : msg_json["data"]["messageChain"]) {
					//分析消息
					if (i["type"] == "Plain") {
						msg += u8" " + i["text"].get<string>();
					}
					else if (i["type"] == "At") {
						msg += u8" " + i["display"].get<string>();
					}
					else if (i["type"] == "File") {
						msg += u8" [文件]" + i["name"].get<string>();
					}
					else if (i["type"] == "MusicShare") {
						msg += u8" [音乐分享]" + i["musicUrl"];
					}
					else if (i["type"] == "Source") {}
					else {
						if (std::find(messageType.begin(), messageType.end(), i["type"]) != messageType.end()) {
							msg += messageType[i["type"]];
						}
						else {
							msg += u8" [未知消息]";
						}
					}
				}
				msg = msg.erase(0, 1);
				//存储结构体
				Message message = { group,qq,msg,qqnick };

				//Callback
				std::unordered_map<string, string> args;
				args.emplace("group", group);
				args.emplace("msg", msg);
				args.emplace("qq", qq);
				args.emplace("qqnick", qqnick);
				emit OtherCallback("onReceiveMsg", args);
				selfGroupCatchLine(message);

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
	emit OtherCallback("onConnectError");
}

//响应连接丢失
void Mirai::onLost(WebSocketClient& client, int code) {
	mirai_logger.error("WebsocketClient Connect Lost,errorCode:{}", code);
	emit OtherCallback("onConnectLost");
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
	if (this->logined) {
		string mj = "{\"syncId\": 2, \"command\":\"sendGroupMessage\", \"subCommand\" : null,\
					\"content\": {\"target\":" + group + ", \"messageChain\": [{ \"type\":\"Plain\", \"text\" : \"" + msg + "\"}]}}";
		std::unordered_map<string, string> args;
		args.emplace("group", group);
		args.emplace("msg", msg);
		emit OtherCallback("onSendMsg", args);
		wsc->sendTextMsg(mj);
	}

}

void Mirai::recallMsg(string target, bool callback) {
	//recall
	if (logined) {
		string mj = "{\"syncId\": 3,\"command\" : \"recall\",\"subCommand\":null,\"content\":{\"target\":" + target + "}}";
		std::unordered_map<string, string> args;
		args.emplace("target", target);
		emit OtherCallback("onRecall", args);
		wsc->sendTextMsg(mj);
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