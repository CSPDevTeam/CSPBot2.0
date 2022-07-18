#pragma once
#include <queue>
#include "qstring.h"
#include <QQueue>

class CSPBot;
extern CSPBot* window;

class Server;
extern Server* server;

class Mirai;
extern Mirai* mirai;

class MySysInfo;
extern MySysInfo* mysysinfo;

class WsClient;
extern WsClient* wsc;

class CommandAPI;
extern CommandAPI* commandApi;


extern QQueue<QString> q;

extern std::string version;
extern int configVersion;
