#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include "logger.h"
#include "global.h"
#include "pluginInfo.h"

namespace plugin {
	static bool load_plugin(string pluginFilePath,  bool isHotLoad); //加载插件
	static bool load_all_plugin(); //加载所有插件
	CSPAPI Plugin* getPlugin(std::string name); //获取插件
	CSPAPI Plugin* getPlugin(HMODULE handle); //获取插件
	CSPAPI bool registerPlugin(HMODULE handle, std::string name, std::string desc, Version version, std::map<std::string, std::string> others); //注册插件
	void initPluginModel(); //加载Model;
} // namespace plugin
