#include <plugins.h>
#include <logger.h>
#include <vector>
#include <set>
#include <unordered_map>

#define PLUGIN_PATH	   "plugins\\"

using namespace std;
namespace fs = filesystem;

bool PluginManager::registerPlugin(const std::string& name, const std::string& description, const std::string& author, const std::string& version) {
	if (hasPlugin(name))
		return false;
	g_plugins.push_back({name, description, author, version});
	return true;
}

bool PluginManager::hasPlugin(const std::string& name) {
	return getPlugin(name) != nullptr;
}

Plugin* PluginManager::getPlugin(const std::string& name) {
	Plugin* res = nullptr;
	for (auto& value : g_plugins) {
		if (value.name == name) {
			res = &value;
		}
	}
	return res;
}

bool PluginManager::LoadPlugin() {
	//加载文件
	try {
		logger.info("Start Loading Plugins...");
		for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
			string filename = info.path().filename().u8string();
			if (info.path().extension() == ".lua") {
				string name(info.path().stem().u8string());

				//忽略以'_'开头的文件
				if (name.front() == '_') {
					logger.warn("Ignoring {}", name);
					continue;
				}
				else {
					logger.info("Loading Plugin {}", name);
					auto m = luaL_dofile(g_lua_State, name.c_str());
					if (m) {
					}
					else {
						logger.error("Fail to load the plugin {}!", name);
					}
					Callbacker cbe(EventCode::onImport);
					cbe.insert("name", name);
					cbe.callback();
					logger.info("Plugin {} Loaded.", name);
				}
			}
		}
		logger.info("All Plugins Loaded.");
	}
	catch (const exception& e) {
		logger.error(e.what());
	}
	return true;
}
