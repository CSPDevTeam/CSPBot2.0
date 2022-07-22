#include "global.h"
#include "cspbot20.h"
#include "config_reader.h"
#include "logger.h"
#include "message_box.h"
#include "plugins.h"

#pragma comment(lib, "dbghelp.lib")
#ifdef _DEBUG
#pragma comment(lib, "lib/libcrypto_d.lib")
#pragma comment(lib, "lib/libssl_d.lib")
#pragma comment(lib, "lib/vld.lib")
#include <vld.h>
#else
#pragma comment(lib, "lib/libcrypto.lib")
#pragma comment(lib, "lib/libssl.lib")
#endif

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

void CheckConfigVersion() {
	if (g_config["Version"].as<int>() < g_config_version)
		msgbox::ShowError("配置文件版本过低,请检查");
}

bool InitConfig() {
	bool s_config_ret = g_config.readFile("config/config.yml");
	bool s_player_ret = g_player.readFile("data/player.yml");
	bool s_event_ret = g_event.readFile("data/event.yml");
	bool s_regular_ret = g_regular.readFile("data/regular.yml");
	return s_config_ret && s_player_ret && s_event_ret && s_regular_ret;
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException); //开启CrashLogger

///////////////////////////////////////////// Main /////////////////////////////////////////////
int main(int argc, char* argv[]) {
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	QApplication a(argc, argv);
	
	//展示ssl版本
	//qDebug() << QSslSocket::sslLibraryBuildVersionString();

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
		msgbox::ShowInfo("缺少字体文件，可能会影响您使用CSPBot\n请根据文档来安装字体");

	if (!InitConfig()) {
		return 1;
	}

	g_main_window = new CSPBot;
	//展示窗口
	g_main_window->show();
	g_main_window->publicStartLogger();

	//检测文件版本
	CheckConfigVersion();

	return a.exec();
}
namespace std {
//template <class _First, class... _Rest>
//struct _Enforce_same {
//	static_assert(conjunction_v<is_same<_First, _Rest>...>,
//		"N4687 26.3.7.2 [array.cons]/2: "
//		"Requires: (is_same_v<T, U> && ...) is true. Otherwise the program is ill-formed.");
//	using type = _First;
//};
//
//template <class _First, class... _Rest>
//array(_First, _Rest...) -> array<typename _Enforce_same<_First, _Rest...>::type, 1 + sizeof...(_Rest)>;
}
