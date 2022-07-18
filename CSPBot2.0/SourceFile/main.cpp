#include "cspbot20.h"
#include "stdafx.h"
#include "global.h"
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include "websocketClient.h"
#include <QQueue>
#include "Version.h"
#include "framework.h"
#include "logger.h"
#include <plugins.h>
#include <fstream>

#pragma comment(lib, "dbghelp.lib")

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
std::string version = TO_VERSION_STRING(PLUGIN_VERSION_MAJOR.PLUGIN_VERSION_MINOR.PLUGIN_VERSION_REVISION);
CSPBot* window;
Server* server;
Mirai* mirai;
WsClient* wsc;
CommandAPI* commandApi;
MySysInfo* mysysinfo = new MySysInfo();
int configVersion = 4;
QQueue<QString> q;
Logger logger("CSPBot");

string getConfig(string key) {
	std::ifstream fin("config/config.yml", ios::in);
	if (!fin.is_open()) {
		return "!failed!";
	}
	auto config = YAML::Load(fin);
	return config[key].as<string>();
};

int checkConfigVersion() {
	std::ifstream fin("config/config.yml", ios::in);
	if (!fin.is_open()) {
		return 2;
	}
	auto config = YAML::Load(fin);
	if (config["Version"].as<int>() < configVersion) {
		return 1;
	}
	return 0;
}

string checkConfigfull() {
	vector<string> fileList = {
		"config/config.yml",
		"data/event.yml",
		"data/player.yml",
		"data/regular.yml",
	};
	
	for (string i : fileList) {
		std::ifstream fin(i, ios::in);
		if (!fin.is_open()) {
			return i;
		}
		fin.close();
	}
	
	return "success.";
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException); //开启CrashLogger

///////////////////////////////////////////// Main /////////////////////////////////////////////

int main(int argc, char* argv[]) {
	PluginManager::LoadPlugin();
	PluginManager::registerPlugin("test");
	QApplication a(argc, argv);
	QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // GB2312也可以
	QTextCodec::setCodecForLocale(codec);				   // 2
	//展示ssl版本
	qDebug() << QSslSocket::sslLibraryBuildVersionString();

	//设置缩放
	qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

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
	//展示窗口
	window->show();
	window->publicStartLogger();

	//检测文件完整性
	string f = checkConfigfull();
	if (f != "success.") {
		QMessageBox::critical(window, "严重错误", "配置文件不完整\n缺少:" + Helper::stdString2QString(f) + "文件", QMessageBox::Yes, QMessageBox::Yes);
		return 1;
	}
	
	//检测文件版本
	switch (checkConfigVersion()) {
	case 1:
		QMessageBox::critical(window, "严重错误", "配置文件版本过低,请检查", QMessageBox::Yes, QMessageBox::Yes);
		return 1;
	case 2:
		QMessageBox::critical(window, "严重错误", "无法初始化配置，请检查config/config.yml文件是否正常", QMessageBox::Yes, QMessageBox::Yes);
		return 1;
	default:
		break;
	}
	


	//未安装字体提示
	if (!hasFont) {
		QMessageBox::information(window, "提示", "缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体", QMessageBox::Yes, QMessageBox::Yes);
	}
	return a.exec();
}
