#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include "logger.h"

namespace plugin {
	class plugin_manager {
	public:
		static bool load_plugin(string pluginFilePath,  bool isHotLoad); //加载插件
		static bool load_all_plugin(); //加载所有插件
		static Plugin* getPlugin(std::string name); //获取插件
		static Plugin* getPlugin(HMODULE handle); //获取插件
		static bool registerPlugin(HMODULE handle, std::string name, std::string desc, Version version, std::map<std::string, std::string> others); //注册插件
	};
	void initPluginModel(); //加载Model;
} // namespace plugin
