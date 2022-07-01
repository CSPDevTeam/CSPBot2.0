#include "cspbot20.h"
#include "stdafx.h"
#include "global.h"
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include "websocketClient.h"

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
std::string version = "2.0.0";
CSPBot* window;
Server* server;
Mirai* mirai;
WsClient* wsc;
MySysInfo* mysysinfo = new MySysInfo();
int configVersion = 3;
std::queue<string> q;

string getConfig(string key){
    auto config = YAML::LoadFile("config/config.yml");
    return config[key].as<string>();
};

bool connectMirai();


///////////////////////////////////////////// Main /////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CSPBot* window = new CSPBot;
    window->show();
    
    return a.exec();
}
