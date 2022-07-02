#include "cspbot20.h"
#include "stdafx.h"
#include "global.h"
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include "websocketClient.h"
#include <QQueue>

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
std::string version = "2.0.0";
CSPBot* window;
Server* server;
Mirai* mirai;
WsClient* wsc;
CommandAPI* commandApi;
MySysInfo* mysysinfo = new MySysInfo();
int configVersion = 3;
QQueue<QString> q;

string getConfig(string key){
    auto config = YAML::LoadFile("config/config.yml");
    return config[key].as<string>();
};

void InitPython(); //³õÊ¼»¯Python½âÊÍÆ÷

///////////////////////////////////////////// Main /////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CSPBot* window = new CSPBot;
    window->show();
    window->publicStartLogger();
    InitPython();
    return a.exec();
}
