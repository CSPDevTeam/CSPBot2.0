#pragma once
#pragma execution_character_set("utf-8")
#include "version.h"
#include <queue>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <Windows.h>
#include <map>
#include "plugin.h"

using std::queue;
using std::string;
using std::string_view;
using std::vector;
using std::unordered_map;
namespace fs = std::filesystem;

class CSPBot;
class Server;
class Mirai;
class WsClient;
class CommandAPI;
class Logger;
class ConfigReader;

enum class EventCode;

// Type
struct Version {
	enum Status {
		Dev,
		Beta,
		Release
	};

	int major;
	int minor;
	int revision;
	Status status;

	CSPAPI explicit Version(int major = 0, int minor = 0, int revision = 0, Status status = Status::Release);

	CSPAPI bool operator<(Version b);
	CSPAPI bool operator==(Version b);

	CSPAPI std::string toString(bool needStatus = false);
	CSPAPI static Version parse(const std::string& str);
};

struct Plugin {
	std::string name;
	std::string desc; // `introduction` before
	Version version;
	std::map<std::string, std::string> others; // `otherInformation` before

	std::string filePath;
	HMODULE handle;
	// Call a Function by Symbol String
	template <typename ReturnType = void, typename... Args>
	inline ReturnType callFunction(const char* functionName, Args... args) {
		void* address = GetProcAddress(handle, functionName);
		if (!address)
			return ReturnType();
		return reinterpret_cast<ReturnType (*)(Args...)>(address)(std::forward<Args>(args)...);
	}
};

inline constexpr auto g_VERSION = TO_VERSION_STRING(PLUGIN_VERSION_MAJOR.PLUGIN_VERSION_MINOR.PLUGIN_VERSION_REVISION);
inline constexpr auto g_config_version = 4;
inline CSPBot* g_main_window;
inline Server* g_server;
inline Mirai* g_mirai;
inline WsClient* g_wsc;
inline CommandAPI* g_cmd_api;
inline queue<string> g_queue;

inline unordered_map<EventCode, vector<EventCallback>> g_cb_functions;
inline unordered_map<string, vector<CommandCallback>> g_command;
inline unordered_map<string,Plugin> g_plugins;
inline unordered_map<EventCode, bool> g_enable_events;

extern Logger g_logger;
extern Logger g_server_logger;
extern Logger g_mirai_logger;
extern Logger g_plugin_logger;


extern ConfigReader g_config;
extern ConfigReader g_player;
extern ConfigReader g_event;
extern ConfigReader g_regular;

// clang-format off
#define CSP_TRY try {
#define CSP_CTACH } catch(const std::exception& e) { ; }
// clang-format on