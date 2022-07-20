#include "global.h"
#include "cspbot20.h"
#include "config_reader.h"
#include "logger.h"
#include "message_box.h"
#include "plugins.h"

#pragma comment(lib, "dbghelp.lib")
// 引入库
#ifdef _DEBUG
#pragma comment(lib, "lib/libcrypto_d.lib")
#pragma comment(lib, "lib/libssl_d.lib")
#pragma comment(lib, "lib/LightWSClient_d.lib")
#else
#pragma comment(lib, "lib/libcrypto.lib")
#pragma comment(lib, "lib/libssl.lib")
#pragma comment(lib, "lib/LightWSClient.lib")
#endif
#pragma warning(disable : 4996)

///////////////////////////////////////////// Global /////////////////////////////////////////////
Logger g_logger("CSPBot");
Logger g_server_logger("Server");
Logger g_mirai_logger("Mirai");

ConfigReader g_config;
ConfigReader g_player;
ConfigReader g_event;
ConfigReader g_regular;

string GetConfig(const string& key) {
	return g_config[key].as<string>();
};

int checkConfigVersion() {
	if (g_config["Version"].as<int>() < g_config_version) {
		return 1;
	}
	return 0;
}

void InitConfig() {
	if (!fs::exists("config/config.yml"))
		msgbox::ShowError("config/config.yml not found");
	if (!fs::exists("data/player.yml"))
		msgbox::ShowError("data/player.yml not found");
	if (!fs::exists("data/event.yml"))
		msgbox::ShowError("data/event.yml not found");
	if (!fs::exists("data/regular.yml"))
		msgbox::ShowError("data/regular.yml not found");

	g_config.readFile("config/config.yml");
	g_player.readFile("data/player.yml");
	g_event.readFile("data/event.yml");
	g_regular.readFile("data/regular.yml");
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
	if (!fontFamily.contains("HarmonyOS Sans SC"))
		msgbox::ShowHint("缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体");

	InitConfig();

	g_main_window = new CSPBot;
	//展示窗口
	g_main_window->show();
	g_main_window->publicStartLogger();

	//检测文件版本
	switch (checkConfigVersion()) {
	case 1:
		msgbox::ShowError("配置文件版本过低,请检查");
		return 1;
	case 2:
		msgbox::ShowError("无法初始化配置，请检查config/config.yml文件是否正常");
		return 1;
	default:
		break;
	}

	g_lua_State = InitLua();
	LoadPlugin();

	return a.exec();
}
