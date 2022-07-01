#pragma once

#include "helper.h"
#include <QObject>
#include <QtWebSockets>
#include <QDebug>
#include <QUrl>
#include <LightWSClient/WebSocketClient.h>

using namespace cyanray;

class WsClient
{
private:
	WebSocketClient ws;
	bool connected = false;
public:
	explicit WsClient();
	//API
	bool sendTextMsg(string msg);
	bool connect(string url);
	bool close();
	bool shutdown();
	int getStatus();
};



std::string getConfig(std::string key);

class Mirai :public QObject
{
	Q_OBJECT
public:
	explicit Mirai();
	bool login();
	void sendGroupMsg(std::string group, std::string msg, bool callback = true);
	void sendAllGroupMsg(std::string msg, bool callback = true);
	void recallMsg(std::string target, bool callback = true);
	void botProfile();
	void changeName(std::string qq, std::string group, std::string name);
	void send_app(std::string group, std::string code);
	void SendPacket(std::string packet);
	bool logined = false;

	//Callback Event
	void onText(WebSocketClient& client, string msg);
	void onError(WebSocketClient& client, string msg);
	void onLost(WebSocketClient& client, int code);
signals:
	void setUserImages(QString qqNum, QString qqNick); //¸ü¸ÄUserImages
	void OtherCallback(QString listener, std::unordered_map<string, string> args={});
private:
	void selfGroupCatchLine(Message message);
	//WsClient* wsc;
};


