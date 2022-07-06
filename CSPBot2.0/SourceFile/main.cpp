#include "cspbot20.h"
#include "stdafx.h"
#include "global.h"
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include "websocketClient.h"
#include <QQueue>

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
std::string version = "2.0.1";
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

void InitPython(); //初始化Python解释器

///////////////////////////////////////////// Main /////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //展示ssl版本
    qDebug() << QSslSocket::sslLibraryBuildVersionString();

	//加载字体
    QFontDatabase database;
    bool hasFont = false;
    auto fontFamily = database.families();
    if (std::find(
        fontFamily.begin(),
        fontFamily.end(),
        "HarmonyOS Sans SC") != fontFamily.end()) {
        hasFont = true;
    };
    
	
	
    CSPBot* window = new CSPBot;
    window->show();
    window->publicStartLogger();
    InitPython();
    if (!hasFont) {
        QMessageBox::information(window, u8"提示", u8"缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体",
            QMessageBox::Yes, QMessageBox::Yes);
    }
    return a.exec();
}
