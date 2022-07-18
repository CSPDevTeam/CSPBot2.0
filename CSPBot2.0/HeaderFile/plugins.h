#pragma once
#include "../lua/lua.hpp"
#include <LuaBridge/LuaBridge.h>

#include <string>
#include <logger.h>

inline Logger pluginLogger("PluginModle");

/*struct BotVersion {
	int major;
	int minor;
	int revision;
};*/

struct Plugin {
	std::string name;
	std::string info;
	std::string author;
	std::string version;
	luabridge::LuaRef m;
};

class PluginManager {
public:
	static bool registerPlugin(luabridge::LuaRef handle, std::string name, std::string info = "", std::string autor = "Unkown", std::string version = "v1.0.0");
	static Plugin* getPlugin(std::string name);
	static Plugin* getPlugin(luabridge::LuaRef handler);
	static bool hasPlugin(std::string name);
	static bool LoadPlugin();

private:
};

enum EventCode {
	onServerStart,	 // OK
	onServerStop,	 // OK
	onSendCommand,	 // OK
	onReceiveMsg,	 // OK
	onReceivePacket, // OK
	onStop,			 // OK
	onLogin,		 // OK
	onImport,		 // OK
	onSendMsg,		 // OK
	onRecall,		 // OK
	onConnectError,	 // OK
	onConnectLost,	 // OK
	onConsoleUpdate, // OK
	onBinded,		 // OK
	onUnBinded,      // OK
};

lua_State* InitLua();
inline lua_State* g_lua_State = InitLua();

class LuaValue {
public:
	LuaValue() : thiz(g_lua_State) {}
	LuaValue(const luabridge::LuaRef& ref) : thiz(ref) {}
	~LuaValue(){};
	template <typename... Arguments>
	LuaValue operator()(Arguments&&... arguments) {
		return thiz(std::forward<Arguments>(arguments)...);
	};
	operator bool() { return thiz; }

private:
	luabridge::LuaRef thiz;
};
inline std::unordered_map<EventCode, vector<LuaValue>> g_cb_functions;
inline std::unordered_map<string, LuaValue> command;
inline std::unordered_map<std::string, Plugin> plugins;
inline std::unordered_map<EventCode, bool> enableEvent;

void EnableListener(EventCode evc);
lua_State* InitLua();
void DeinitLua(lua_State* L);
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
					pluginLogger.error(e.what());
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
