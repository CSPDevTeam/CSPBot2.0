#include <cspbot20.h>
#include <stdafx.h>
#include <global.h>
#include <mysysinfo/mysysinfo.h>
#include <QtWidgets/QApplication>
#include <websocketClient.h>
#include <framework.h>
#include <logger.h>
#include <plugins.h>
#include <fstream>
#include <global.h>
#include <messageBox.h>

#pragma comment(lib, "dbghelp.lib")

using namespace std;
///////////////////////////////////////////// Global /////////////////////////////////////////////
Logger logger("CSPBot");
Logger serverLogger("Server");
Logger mirai_logger("Mirai");

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
	if (config["Version"].as<int>() < g_config_version) {
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
	QApplication a(argc, argv);
	QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // GB2312也可以
	QTextCodec::setCodecForLocale(codec);
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
	auto fontFamily = database.families();
	bool hasFont = fontFamily.contains("HarmonyOS Sans SC");

	g_main_window = new CSPBot;
	//展示窗口
	g_main_window->show();
	g_main_window->publicStartLogger();

	try {
		//检测文件完整性
		string f = checkConfigfull();
		if (f != "success.") {
			messageBox::ShowError(QString::fromStdString("配置文件不完整\n缺少:") + f.c_str() + "文件");
			return 1;
		}
	}
	catch (const std::exception& e) {
		messageBox::ShowError(e.what());
	}

	//检测文件版本
	switch (checkConfigVersion()) {
	case 1:
		messageBox::ShowError("配置文件版本过低,请检查");
		return 1;
	case 2:
		messageBox::ShowError("无法初始化配置，请检查config/config.yml文件是否正常");
		return 1;
	default:
		break;
	}

	g_lua_State = InitLua();
	LoadPlugin();

	//未安装字体提示
	if (!hasFont) {
		QMessageBox::information(g_main_window, "提示", "缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体", QMessageBox::Yes, QMessageBox::Yes);
	}
	return a.exec();
}
