#pragma once
#include "lua_api.h"
#include <string>
#include <logger.h>

extern Logger logger;

enum class EventCode {
	onServerStart,
	onServerStop,
	onSendCommand,
	onReceiveMsg,
	onReceivePacket,
	onStop,
	onLogin,
	onImport,
	onSendMsg,
	onRecall,
	onConnectError,
	onConnectLost,
	onConsoleUpdate,
	onBinded,
	onUnBinded,
};

inline std::unordered_map<EventCode, vector<LuaValue>> g_cb_functions;
inline std::unordered_map<string, LuaValue> command;
inline std::vector<struct Plugin> g_plugins;
inline std::unordered_map<EventCode, bool> enableEvent;

//事件回调，初始化对象将申请GIL
class Callbacker {
public:
	Callbacker(EventCode t) : type_(t), arg_(luabridge::newTable(g_lua_State)) {}
	~Callbacker() {}

	//事件回调
	inline bool callback() {
		bool pass = true;
		for (auto& cb : g_cb_functions[type_]) {
			if (g_cb_functions[type_].size() > 0) {
				try {
					if (arg_.length() > 0) {
						pass = cb(arg_) != false;
					}
					else {
						pass = cb() != false;
					}
				}
				catch (const std::exception& e) {
					logger.error(e.what());
				}
			}
		}

		return pass;
	}

	inline Callbacker& insert(const char* key, const string& item) {
		arg_[key] = item.c_str();
		return *this;
	}

private:
	EventCode type_;
	luabridge::LuaRef arg_;
};

struct Plugin {
	std::string name;
	std::string description;
	std::string author;
	std::string version;
};

struct PluginManager {
	static bool registerPlugin(const std::string& name, const std::string& description = "", const std::string& author = "Unknown", const std::string& version = "v1.0.0");
	static Plugin* getPlugin(const std::string& name);
	static bool hasPlugin(const std::string& name);
	static bool LoadPlugin();
};
