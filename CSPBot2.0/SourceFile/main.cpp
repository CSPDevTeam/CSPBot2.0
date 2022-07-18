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
	try {
		auto config = YAML::LoadFile("config/config.yml");
		return config[key].as<string>();
	}
	catch (const std::exception& e) {
		//logger.error(e.what());
	}
};

bool checkConfigVersion() {
	try {
		auto config = YAML::LoadFile("config/config.yml");
		if (config["Version"].as<int>() < configVersion) {
			return false;
		}
		return true;
	}
	catch (const std::exception& e) {
		//logger.error(e.what());
	}
}

// void InitPython();											  //初始化Python解释器
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException); //开启CrashLogger

///////////////////////////////////////////// Main /////////////////////////////////////////////
void warnInfo() {
	QMessageBox::warning(window, "注意", "未检测到安装Python\nCSPBot插件模块将不会运行", QMessageBox::Yes, QMessageBox::Yes);
	logger.warn("未检测到安装Python,CSPBot插件模块将不会运行");
}

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
	//检测文件版本
	if (!checkConfigVersion()) {
		QMessageBox::critical(window, "严重错误", "配置文件版本过低,请检查", QMessageBox::Yes, QMessageBox::Yes);
		return 1;
	}

	//展示窗口
	window->show();
	window->publicStartLogger();
	// tryInitPython();


	//未安装字体提示
	if (!hasFont) {
		QMessageBox::information(window, "提示", "缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体", QMessageBox::Yes, QMessageBox::Yes);
	}
	return a.exec();
}
