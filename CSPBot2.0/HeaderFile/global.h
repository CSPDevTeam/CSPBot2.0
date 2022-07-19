#pragma once
#pragma execution_character_set("utf-8")
// self
#include "version.h"
// third-party
#include <qstring.h>
#include <QQueue>

class CSPBot;
class Server;
class Mirai;
class WsClient;
class CommandAPI;

namespace {
inline constexpr auto g_VERSION = TO_VERSION_STRING(PLUGIN_VERSION_MAJOR.PLUGIN_VERSION_MINOR.PLUGIN_VERSION_REVISION);
inline constexpr auto g_config_version = 4;
inline CSPBot* g_main_window;
inline Server* g_server;
inline Mirai* g_mirai;
inline WsClient* g_wsc;
inline CommandAPI* g_cmd_api;
inline QQueue<QString> g_queue;
} // namespace
