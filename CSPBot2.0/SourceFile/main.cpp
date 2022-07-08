#include "cspbot20.h"
#include "stdafx.h"
#include "global.h"
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include "websocketClient.h"
#include <QQueue>

#pragma comment(lib, "dbghelp.lib")

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
std::string version = "2.0.1";
CSPBot* window;
Server* server;
Mirai* mirai;
WsClient* wsc;
CommandAPI* commandApi;
MySysInfo* mysysinfo = new MySysInfo();
int configVersion = 4;
QQueue<QString> q;

string getConfig(string key){
    auto config = YAML::LoadFile("config/config.yml");
    return config[key].as<string>();
};

bool checkConfigVersion() {
    auto config = YAML::LoadFile("config/config.yml");
	if(config["Version"].as<int>() < configVersion) {
		return false;
	}
    return true;
}

void InitPython(); //初始化Python解释器
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException); //开启CrashLogger

///////////////////////////////////////////// Main /////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");//GB2312也可以
    QTextCodec::setCodecForLocale(codec);//2
    //展示ssl版本
    qDebug() << QSslSocket::sslLibraryBuildVersionString();

    //注册异常捕获函数
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

    // set printf and fprintf out immediately
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
	

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
    //检测文件版本
    if (!checkConfigVersion()) {
        QMessageBox::critical(window, u8"严重错误", u8"配置文件版本过低,请检查",
            QMessageBox::Yes, QMessageBox::Yes);
        return 1;
    }
	
	//展示窗口
    window->show();
    window->publicStartLogger();
    InitPython();

    //未安装字体提示
    if (!hasFont) {
        QMessageBox::information(window, u8"提示", u8"缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体",
            QMessageBox::Yes, QMessageBox::Yes);
    }
    return a.exec();
}
