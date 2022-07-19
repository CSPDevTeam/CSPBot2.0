#pragma once
// third-party
#include "../lua/lua.hpp"
#include <LuaBridge/LuaBridge.h>

lua_State* InitLua();

inline lua_State* g_lua_State = InitLua();

class LuaValue {
public:
	LuaValue() : thiz(g_lua_State) {}
	LuaValue(const char* str) : thiz(g_lua_State, str) {}
	LuaValue(const luabridge::LuaRef& ref) : thiz(ref) {}
	~LuaValue(){};
	luabridge::LuaRef& raw() { return thiz; }
	size_t length() { return thiz.length(); }
	template <typename... Arguments>
	LuaValue operator()(Arguments&&... arguments) {
		return thiz(std::forward<Arguments>(arguments)...);
	};
	LuaValue operator[](const string& key) {
		return thiz[key];
	}
	LuaValue operator[](const char* key) {
		return thiz[key];
	}
	LuaValue operator[](size_t key) {
		return thiz[key];
	}
	operator bool() { return thiz; }

private:
	luabridge::LuaRef thiz;
};
