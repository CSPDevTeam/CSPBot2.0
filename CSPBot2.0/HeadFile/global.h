#pragma once
#include <queue>

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

extern std::queue<std::string> q;

extern std::string version;
extern int configVersion;
