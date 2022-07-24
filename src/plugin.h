#pragma once
#include <map>
#include <Windows.h>
#include <vector>
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using std::string;
using std::vector;

//自定义类
class Logger;

#ifdef CSPBot_EXPORTS
#define CSPAPI __declspec(dllexport)
enum class EventCode;
#else
#define CSPAPI __declspec(dllimport)
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
#endif

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

//命令回调
typedef void(*CommandCallback)(std::vector<std::string> args);
//事件监听
typedef void (*EventCallback)(std::unordered_map<std::string,std::string> data);

//导出Logger
class CSPAPI cspLogger;

//向外导出
CSPAPI bool runCommand(const string& cmd);
CSPAPI void SendGroupMsg(const string& group, const string& msg);
CSPAPI void SendAllGroupMsg(const string& msg);
CSPAPI void RecallMsg(const string& target);
CSPAPI void SendApp(const string& group, const string& code);
CSPAPI void SendPacket(const string& packet);
CSPAPI bool SetListener(EventCode evc, EventCallback func);
CSPAPI std::string MotdJE(const string& host);
CSPAPI std::string MotdBE(const string& host);
CSPAPI button_Type ShowTipWindow(window_Type type, const string& title, const string& content, const vector<button_Type>& buttonType);
CSPAPI bool RegisterCommand(const string& cmd, CommandCallback cbf);
CSPAPI json QueryInfo(const string& type, const string& arg);
CSPAPI bool UnbindXbox(string qq);
CSPAPI bool BindXbox(string name, string qq);
CSPAPI std::vector<std::string> GetGroup();
CSPAPI std::vector<std::string> GetAdmin();

//导出Logger
class cspLogger {
public:
	cspLogger(string name);
	template <typename... Args>
	CSPAPI void info(const char* msg, Args&&... args);
	template <typename... Args>
	CSPAPI void warn(const char* msg, Args&&... args);
	template <typename... Args>
	CSPAPI void error(const char* msg, Args&&... args);
	template <typename... Args>
	CSPAPI void debug(const char* msg, Args&&... args);

private:
	string _modelName;
	Logger* _logger;
};