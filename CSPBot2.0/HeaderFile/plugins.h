﻿#pragma once
// self
#include "logger.h"
// third-party
#include "../lua/lua.hpp"
#include <LuaBridge/LuaBridge.h>

using luabridge::LuaRef;

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

// class Callbacker {
// public:
//	Callbacker(EventCode t) : type_(t), arg_(luabridge::newTable(g_lua_State)) {}
//	~Callbacker() {}
//
//	//事件回调
//	bool callback() {
//		bool pass = true;
//		for (auto& cb : g_cb_functions[type_]) {
//			if (g_cb_functions[type_].size() > 0) {
//				try {
//					if (arg_.length() > 0) {
//						pass = cb(arg_) != false;
//					}
//					else {
//						pass = cb() != false;
//					}
//				}
//				catch (const std::exception& e) {
//					logger.error(e.what());
//				}
//			}
//		}
//
//		return pass;
//	}
//
//	Callbacker& insert(const char* key, const string& item) {
//		arg_[key] = item.c_str();
//		return *this;
//	}
//
// private:
//	EventCode type_;
//	LuaValue arg_;
// };

lua_State* InitLua();

bool LoadPlugin();