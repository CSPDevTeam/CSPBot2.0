#pragma once
#include "global.h"

namespace plugin {
static bool load_plugin(string pluginFilePath, bool isHotLoad);                                                   //加载插件
static bool load_all_plugin();                                                                                    //加载所有插件
CSPAPI Plugin* getPlugin(string name);                                                                            //获取插件
CSPAPI Plugin* getPlugin(HMODULE handle);                                                                         //获取插件
CSPAPI bool registerPlugin(HMODULE handle, string name, string desc, Version version, map<string, string> others);//注册插件
void initPluginModel();                                                                                           //加载Model;
}// namespace plugin
