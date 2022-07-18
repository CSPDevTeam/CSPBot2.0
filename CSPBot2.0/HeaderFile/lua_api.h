#pragma once
#include "../lua/lua.hpp"
#include <LuaBridge/LuaBridge.h>

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
