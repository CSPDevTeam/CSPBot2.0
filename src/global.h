#pragma once
#pragma execution_character_set("utf-8")
#include "version.h"
#include <queue>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <map>
#include "pluginInfo.h"

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

inline constexpr auto g_VERSION = TO_VERSION_STRING(PLUGIN_VERSION_MAJOR.PLUGIN_VERSION_MINOR.PLUGIN_VERSION_REVISION);
inline constexpr auto g_config_version = 4;
inline CSPBot* g_main_window;
inline Server* g_server;
inline Mirai* g_mirai;
inline WsClient* g_wsc;
inline CommandAPI* g_cmd_api;
inline CSPAPI queue<string> g_queue;

inline unordered_map<EventCode, vector<EventCallback>> g_cb_functions;
inline unordered_map<string, vector<CommandCallback>> g_command;
inline unordered_map<string,Plugin> g_plugins;
inline unordered_map<EventCode, bool> g_enable_events;

extern Logger g_logger;
extern Logger g_server_logger;
extern Logger g_mirai_logger;
extern Logger g_plugin_logger;


extern CSPAPI ConfigReader g_config;
extern ConfigReader g_player;
extern ConfigReader g_event;
extern ConfigReader g_regular;

// clang-format off
#define CSP_TRY try {
#define CSP_CTACH } catch(const std::exception& e) { ; }
// clang-format on