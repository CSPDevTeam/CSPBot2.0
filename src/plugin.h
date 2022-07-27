#pragma once
#include "plugin_manager.h"
#include "json.hpp"

using json = nlohmann::json;

//自定义类
class Logger;

//窗口类型
enum window_Type {
	information,
	question,
	warning,
	critical
};

//按钮类型
enum button_Type {
	NoButton = 0x00000000,
	Ok = 0x00000400,
	Save = 0x00000800,
	SaveAll = 0x00001000,
	Open = 0x00002000,
	Yes = 0x00004000,
	YesToAll = 0x00008000,
	No = 0x00010000,
	NoToAll = 0x00020000,
	Abort = 0x00040000,
	Retry = 0x00080000,
	Ignore = 0x00100000,
	Close = 0x00200000,
	Cancel = 0x00400000,
	Discard = 0x00800000,
	Help = 0x01000000,
	Apply = 0x02000000,
	Reset = 0x04000000,
	RestoreDefaults = 0x08000000,
};

//向外导出
CSPAPI bool runCommand(string cmd);
CSPAPI void SendGroupMsg(string group, string msg);
CSPAPI void SendAllGroupMsg(string msg);
CSPAPI void RecallMsg(string target);
CSPAPI void SendApp(string group, string code);
CSPAPI void SendPacket(string packet);
CSPAPI bool SetListener(EventCode evc, EventCallback func);
CSPAPI string MotdJE(string host);
CSPAPI string MotdBE(string host);
CSPAPI button_Type ShowTipWindow(window_Type type, string title, string content, const vector<button_Type>& buttonType);
CSPAPI bool RegisterCommand(string cmd, CommandCallback cbf);
CSPAPI json QueryInfo(string type, string arg);
CSPAPI bool UnbindXbox(string qq);
CSPAPI bool BindXbox(string name, string qq);
CSPAPI vector<string> GetGroup();
CSPAPI vector<string> GetAdmin();

// Get Current DLL's module handle
// (Must be header-only!)
HMODULE inline GetCurrentModule() {
	HMODULE hModule = nullptr;
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
	        (LPCWSTR)GetCurrentModule,
	        &hModule)) {
		return hModule;
	}
	return nullptr;
}

//注册插件
inline bool regPlugin(string name, string desc, Version version, map<string, string> others) {
	return plugin::registerPlugin(GetCurrentModule(), name, desc, version, others);
}

//导出Logger
class CSPAPI cspLogger {
public:
	cspLogger(string name);
	void info(const char* msg);
	void warn(const char* msg);
	void error(const char* msg);
	void debug(const char* msg);

private:
	Logger* _logger;
};