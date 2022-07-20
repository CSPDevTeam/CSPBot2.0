#pragma once
#pragma execution_character_set("utf-8")
#include "version.h"
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

using std::queue;
using std::string;
using std::vector;
using std::unordered_map;
namespace fs = std::filesystem;

class CSPBot;
class Server;
class Mirai;
class WsClient;
class CommandAPI;
class Logger;
struct Plugin {
	string name;
	string description;
	string author;
	string version;
};
enum class EventCode;

inline constexpr auto g_VERSION = TO_VERSION_STRING(PLUGIN_VERSION_MAJOR.PLUGIN_VERSION_MINOR.PLUGIN_VERSION_REVISION);
inline constexpr auto g_config_version = 4;
inline CSPBot* g_main_window;
inline Server* g_server;
inline Mirai* g_mirai;
inline WsClient* g_wsc;
inline CommandAPI* g_cmd_api;
inline queue<string> g_queue;

inline struct lua_State* g_lua_State = nullptr;

inline unordered_map<EventCode, vector<string>> g_cb_functions;
inline unordered_map<string, string> command;
inline vector<Plugin> g_plugins;
inline unordered_map<EventCode, bool> enableEvent;

extern Logger logger;
extern Logger serverLogger;
extern Logger mirai_logger;

// clang-format off
#define CSP_TRY try {
#define CSP_CTACH } catch(const std::exception& e) { ; }
// clang-format on