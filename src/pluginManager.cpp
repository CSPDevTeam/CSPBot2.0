#include "pluginManager.h"
#include "logger.h"
#include "helper.h"
#include <Windows.h>
#include <vector>
#include <Psapi.h>
#include "global.h"
#define PLUGIN_PATH "plugins\\"

using namespace std;

// Version
Version::Version(int major, int minor, int revision, Status status)
	: major(major), minor(minor), revision(revision), status(status) {
}

bool Version::operator<(Version b) {
	return major < b.major || (major == b.major && minor < b.minor) ||
		(major == b.major && minor == b.minor && revision < b.revision);
}

bool Version::operator==(Version b) {
	return major == b.major && minor == b.minor && revision == b.revision;
}

std::string Version::toString(bool needStatus) {
	string res = to_string(major) + "." + to_string(minor) + "." + to_string(revision);
	if (needStatus) {
		if (status == Status::Beta)
			res += " - Beta";
		else if (status == Status::Dev)
			res += " - Dev";
	}
	return res;
}

Version Version::parse(const std::string& str) {
	Version ver;
	std::string a = str;
	std::string status;
	size_t pos = 0;
	if ((pos = str.find_last_of('-')) != std::string::npos) {
		a = str.substr(0, pos);
		status = str.substr(pos + 1);
		std::transform(status.begin(), status.end(), status.begin(), ::tolower);
	}
	if (status == "beta")
		ver.status = Version::Status::Beta;
	else if (status == "dev" || status == "alpha")
		ver.status = Version::Status::Dev;
	else
		ver.status = Version::Status::Release;

	auto res = helper::split(a, ".");

	if (res.size() >= 1)
		ver.major = stoi(res[0]);
	if (res.size() >= 2)
		ver.minor = stoi(res[1]);
	if (res.size() >= 3)
		ver.revision = stoi(res[2]);

	return ver;
}

//获取插件路径
string GetModulePath(HMODULE handle) {
	wchar_t buf[MAX_PATH] = {0};
	GetModuleFileNameEx(GetCurrentProcess(), handle, buf, MAX_PATH);
	return helper::wstr2str(std::wstring(buf));
}

//插件管理

//获取插件
// Helper
Plugin* GetPlugin_Raw(std::string name) {
	for (auto& it : g_plugins) {
		if (it.second.name == name || (filesystem::path(helper::str2wstr(it.second.filePath))).filename().u8string() == name) {
			return &it.second;
		}
	}
	return nullptr;
}

Plugin* plugin::getPlugin(std::string name) {
	auto res = GetPlugin_Raw(name);
	if (res)
		return res;
	return nullptr;
}

Plugin* plugin::getPlugin(HMODULE handle) {
	if (!handle)
		return nullptr;

	for (auto& it : g_plugins) {
		if (it.second.handle == handle) {
			return &it.second;
		}
	}
	return nullptr;
}

//注册插件
bool plugin::registerPlugin(HMODULE handle, std::string name, std::string desc, Version version, std::map<std::string, std::string> others) {
	if (handle != nullptr) // DLL Plugin
	{
		if (getPlugin(handle) != nullptr) {
			for (auto& data : g_plugins) {
				return data.second.handle == handle;
			}
		} 
		else if (getPlugin(name) != nullptr) {
			return false; // 拒绝覆盖他人的数据
		}
	}

	Plugin plugin{name, desc, version, others};
	plugin.handle = handle;

	try {
		plugin.filePath = filesystem::path(helper::str2wstr(others.at("PluginFilePath"))).lexically_normal().u8string();
		others.erase("PluginFilePath");
	} catch (...) {
		if (handle)
			plugin.filePath = GetModulePath(handle);
	}

	g_plugins.emplace(name, plugin);
	return true;
}

bool plugin::load_plugin(string pluginFilePath, bool isHotLoad) {
	try {
		filesystem::path path(filesystem::path(pluginFilePath).lexically_normal());
		string file_name = path.filename().u8string();
		string file_ext = path.extension().u8string();
		
		if (file_ext != ".dll") {
			return false;
		}

		auto lib = LoadLibrary(helper::str2wstr(pluginFilePath).c_str());
		if (lib) {
			if (!registerPlugin(lib, file_name, file_name, Version(1, 0, 0), {})) {
				g_plugin_logger.error("Failed to register plugin {}!", pluginFilePath);
				if (getPlugin(file_name)) {
					g_plugin_logger.error("A plugin named {} has been registered", file_name);
				}
				return false;
			}
		} else {
			g_plugin_logger.error("Fail to load the plugin {}!", file_name);
			return false;
		}
		
		//调用onPostInit
		if (!getPlugin(lib)) {
			return false;
		}
		
		//Call onPostInit
		auto fn = GetProcAddress(getPlugin(lib)->handle, "onPostInit");
		if (fn) {
			try {
				((void (*)())fn)();
				return true;
			} 
			catch (std::exception& e) {
				g_plugin_logger.error("Plugin <{}> throws an std::exception in onPostInit", file_name);
				g_plugin_logger.error("Exception: {}", e.what());
				g_plugin_logger.error("Fail to init this plugin!");
				return false;
			} 
			catch (...) {
				g_plugin_logger.error("Plugin <{}> throws an exception in onPostInit", file_name);
				g_plugin_logger.error("Fail to init this plugin!");
				return false;
			}
		} 
		else{
			g_plugin_logger.error("Fail to get the initialize point of the plugin {}!", file_name);
			return false;
		}
		
		return true;
	}
	catch (const std::exception& e) {
		g_plugin_logger.error("Error occurred when loading plugin {}!", pluginFilePath);
		g_plugin_logger.error(e.what());
		return false;
	}
	catch (...) {
		g_plugin_logger.error("Error occurred when loading plugin {}!", pluginFilePath);
		return false;
	}
}

bool plugin::load_all_plugin() {
	if (!fs::exists(PLUGIN_PATH)) {
		fs::create_directory(PLUGIN_PATH);
	}
	g_plugin_logger.info("Plugins Modle started.");
	for (auto& file : fs::directory_iterator(PLUGIN_PATH)) {
		string file_name = file.path().filename().u8string();
		if (file.path().extension().u8string() == ".dll") {
			bool loaded = load_plugin(file.path().u8string(), false);
			if (loaded) {
				g_plugin_logger.info("Plugin {} Loaded.", file_name);
			}
		}
	}
	g_plugin_logger.info("Plugins Modle Loaded All Plugin.");
	return true;
}

void plugin::initPluginModel() {
	plugin::load_all_plugin();
}