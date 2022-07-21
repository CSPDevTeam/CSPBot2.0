#include "plugins.h"
#include "global.h"
#include "server.h"
#include "config_reader.h"
#include "helper.h"
#include "logger.h"

#define PLUGIN_PATH "plugins\\"

int versionPacket = 1; // api版本
// Buttons
enum SelfStandardButton {
	NoButton = 0,
	Ok = 1024,
	Save,
	SaveAll,
	Open,
	Yes,
	YesToAll,
	No,
	NoToAll,
	Abort,
	Retry,
	Ignore,
	Close,
	Cancel,
	Discard,
	Help,
	Apply,
	Reset,
	RestoreDefaults,
};

//######################### Server #########################

bool RunCommand(const string& cmd, lua_State* L) {
	return g_server->sendCmd(cmd + '\n');
}

//######################### Mirai #########################
// Mirai发信息主API
bool ThreadMirai(string cbe, StringMap qm) {
	string type = cbe;
	if (type == "sendGroup") {
		string group = qm.at("group");
		string msg = qm.at("msg");
		g_mirai->sendGroupMsg(group, msg, false);
	}
	else if (type == "sendAllGroup") {
		string msg = qm.at("msg");
		g_mirai->sendAllGroupMsg(msg, false);
	}
	else if (type == "recallMsg") {
		string target = qm.at("target");
		g_mirai->recallMsg(target, false);
	}
	else if (type == "App") {
		string group = qm.at("group");
		string code = qm.at("code");
		g_mirai->send_app(group, code);
	}
	else if (type == "sendPacket") {
		string code = qm.at("packet");
		g_mirai->SendPacket(code);
	}
	return true;
}
// Mirai API
void SendGroupMsg(const string& group, const string& msg, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("msg", msg);
	ThreadMirai("sendGroup", data);
}

void SendAllGroupMsg(const string& msg, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("msg", msg);
	ThreadMirai("sendAllGroup", data);
}

void RecallMsg(const string& target, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("target", target);
	ThreadMirai("recallMsg", data);
}

void SendApp(const string& group, const string& code, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("code", code);
	ThreadMirai("App", data);
}

void SendPacket(const string& packet, lua_State* L) {
	std::unordered_map<string, string> data;
	data.emplace("packet", packet);
	ThreadMirai("sendPacket", data);
}

//######################### Listener #########################

bool SetListener(const string& eventName, const luabridge::LuaRef& func, lua_State* L) {
	auto event_code = magic_enum::enum_cast<EventCode>(eventName);
	if (!event_code)
		throw std::invalid_argument("Invalid event name " + eventName);

	//添加回调函数
	g_cb_functions[event_code.value()].push_back(func);
	return true;
}

//######################### Motd #########################

string MotdJE(const string& host, lua_State* L) {
	auto regex = QRegularExpression("(\\w.+):(\\w+)");
	auto match = regex.match(QString::fromStdString(host));
	if (match.hasMatch()) {
		return Motd::motdje(host);
	}
	else {
		return "{}";
	}
}
string MotdBE(const string& host, lua_State* L) {
	auto regex = QRegularExpression("(\\w.+):(\\w+)");
	auto matchs = regex.match(QString::fromStdString(host));
	if (matchs.hasMatch()) {
		return Motd::motdbe(host);
	}
	else {
		return "{}";
	}
}

//######################### Window  #########################

string QButtonToString(QMessageBox::StandardButton c) {
	if (c == QMessageBox::NoButton) {
		return "NoButton";
	}
	else if (c == QMessageBox::Ok) {
		return "Ok";
	}
	else if (c == QMessageBox::Save) {
		return "Save";
	}
	else if (c == QMessageBox::SaveAll) {
		return "SaveAll";
	}
	else if (c == QMessageBox::Open) {
		return "Open";
	}
	else if (c == QMessageBox::Yes) {
		return "Yes";
	}
	else if (c == QMessageBox::YesToAll) {
		return "YesToAll";
	}
	else if (c == QMessageBox::No) {
		return "No";
	}
	else if (c == QMessageBox::NoToAll) {
		return "NoToAll";
	}
	else if (c == QMessageBox::Abort) {
		return "Abort";
	}
	else if (c == QMessageBox::Retry) {
		return "Retry";
	}
	else if (c == QMessageBox::Ignore) {
		return "Ignore";
	}
	else if (c == QMessageBox::Close) {
		return "Close";
	}
	else if (c == QMessageBox::Cancel) {
		return "Cancel";
	}
	else if (c == QMessageBox::Discard) {
		return "Discard";
	}
	else if (c == QMessageBox::Help) {
		return "Help";
	}
	else if (c == QMessageBox::Apply) {
		return "Apply";
	}
	else if (c == QMessageBox::Reset) {
		return "Reset";
	}
	else if (c == QMessageBox::RestoreDefaults) {
		return "RestoreDefaults";
	}
	else {
		return "";
	}
}

QMessageBox::StandardButton StringToQButton(string c) {
	if (c == "NoButton") {
		return QMessageBox::NoButton;
	}
	else if (c == "Ok") {
		return QMessageBox::Ok;
	}
	else if (c == "Save") {
		return QMessageBox::Save;
	}
	else if (c == "SaveAll") {
		return QMessageBox::SaveAll;
	}
	else if (c == "Open") {
		return QMessageBox::Open;
	}
	else if (c == "Yes") {
		return QMessageBox::Yes;
	}
	else if (c == "YesToAll") {
		return QMessageBox::YesToAll;
	}
	else if (c == "No") {
		return QMessageBox::No;
	}
	else if (c == "NoToAll") {
		return QMessageBox::NoToAll;
	}
	else if (c == "Abort") {
		return QMessageBox::Abort;
	}
	else if (c == "Retry") {
		return QMessageBox::Retry;
	}
	else if (c == "Ignore") {
		return QMessageBox::Ignore;
	}
	else if (c == "Close") {
		return QMessageBox::Close;
	}
	else if (c == "Cancel") {
		return QMessageBox::Cancel;
	}
	else if (c == "Discard") {
		return QMessageBox::Discard;
	}
	else if (c == "Help") {
		return QMessageBox::Help;
	}
	else if (c == "Apply") {
		return QMessageBox::Apply;
	}
	else if (c == "Reset") {
		return QMessageBox::Reset;
	}
	else if (c == "RestoreDefaults") {
		return QMessageBox::RestoreDefaults;
	}
	else {
		return QMessageBox::FlagMask;
	}
}

//构造弹窗
string ShowTipWindow(const string& type, const string& title, const string& content, const luabridge::LuaRef& buttonType, lua_State* L) {
	QFlags<QMessageBox::StandardButton> btn;
	for (int i = 0; i < buttonType.length(); ++i) {
		string Btype = buttonType[i];
		auto event_code = StringToQButton(Btype);

		if (event_code == QMessageBox::FlagMask) {
			throw std::invalid_argument("Invalid StandardButton name " + Btype);
		}

		btn = btn | event_code;
	}
	//展示窗口
	QMessageBox::StandardButton Choosedbtn;
	if (type == "information") {
		Choosedbtn = QMessageBox::information(
			g_main_window,
			QString::fromStdString(title),
			QString::fromStdString(content),
			btn);
	}
	//询问
	else if (type == "question") {
		Choosedbtn = QMessageBox::question(
			g_main_window,
			QString::fromStdString(title),
			QString::fromStdString(content),
			btn);
	}
	//警告
	else if (type == "warning") {
		Choosedbtn = QMessageBox::warning(
			g_main_window,
			QString::fromStdString(title),
			QString::fromStdString(content),
			btn);
	}
	//错误
	else if (type == "critical") {
		Choosedbtn = QMessageBox::critical(
			g_main_window,
			QString::fromStdString(title),
			QString::fromStdString(content),
			btn);
	}
	//未知
	else {
		throw std::invalid_argument("Invalid TipWindowType name " + type);
		return "";
	}

	//返回值
	return QButtonToString(Choosedbtn);
}

//######################### Command #########################

bool RegisterCommand(const string& cmd, const luabridge::LuaRef& cbf, lua_State* L) {
	if (g_command.find(cmd) != g_command.end() &&
		cmd != "bind" &&
		cmd != "unbind" &&
		cmd != "motdbe" &&
		cmd != "motdje" &&
		cmd != "start" &&
		cmd != "stop") {
		return false;
		throw std::invalid_argument("Invalid command:" + cmd);
	}
	if (g_command.find(cmd) != g_command.end()) {
		return false;
		throw std::invalid_argument("Invalid command:" + cmd);
	}
	g_command.emplace(cmd, cbf);
	return true;
}

//######################### Player #########################
luabridge::LuaRef QueryInfo(const string& type, const string& arg, lua_State* L) {
	if (type != "qq" && type != "xuid" && type != "player") {
		throw std::invalid_argument("Invalid type:" + type);
	}

	auto queryData = Bind::queryXboxID(type, arg);
	string j = yaml2json(queryData).dump();
	return {L, j};
}

bool UnbindXbox(string qq, lua_State* L) {
	return Bind::unbind(qq);
}

bool BindXbox(string name, string qq, lua_State* L) {
	return Bind::bind(qq, name);
}

//######################### Info #########################
luabridge::LuaRef GetGroup(lua_State* L) {
	luabridge::LuaRef groupList = luabridge::newTable(L);
	// bool inGroup = false;
	for (auto i : g_config["group"]) {
		groupList.append(i.as<string>());
	}
	return groupList;
}

luabridge::LuaRef GetAdmin(lua_State* L) {
	luabridge::LuaRef groupList = luabridge::newTable(L);
	bool inGroup = false;
	for (auto i : g_config["admin"]) {
		groupList.append(i.as<string>());
	}
	return groupList;
}

void EnableListener(EventCode evc) {
	g_enable_events.emplace(evc, true);
}

Plugin* GetPlugin(const string& name) {
	Plugin* res = nullptr;
	for (auto& value : g_plugins) {
		if (value.name == name) {
			res = &value;
		}
	}
	return res;
}

bool HasPlugin(const string& name) {
	return GetPlugin(name) != nullptr;
}

bool RegisterPlugin(const string& name, const string& description, const string& author, const string& version, lua_State* L) {
	if (HasPlugin(name))
		return false;
	g_plugins.push_back({name, description, author, version});
	return true;
}

bool LoadPlugin() {
	//加载文件
	try {
		g_logger.info("Start Loading Plugins...");
		if (!fs::exists(PLUGIN_PATH))
			fs::create_directories(PLUGIN_PATH);
		for (auto& info : fs::directory_iterator(PLUGIN_PATH)) {
			if (info.path().extension() == ".lua") {
				string name(info.path().stem().u8string());

				//忽略以'_'开头的文件
				if (name.front() == '_') {
					g_logger.warn("Ignoring {}", name);
					continue;
				}
				else {
					g_logger.info("Loading Plugin {}", name);
					auto m = luaL_dofile(g_lua_State, name.c_str());
					if (m) {
					}
					else {
						g_logger.error("Fail to load the plugin {}!", name);
					}
					// Callbacker cbe(EventCode::onImport);
					// cbe.insert("name", name);
					// cbe.callback();
					g_logger.info("Plugin {} Loaded.", name);
				}
			}
		}
		g_logger.info("All Plugins Loaded.");
	}
	catch (const std::exception& e) {
		g_logger.error(e.what());
	}
	return true;
}

lua_State* InitLua() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luabridge::getGlobalNamespace(L)
		.beginClass<Logger>("Logger")
		.addConstructor<void (*)(string)>()
		.addFunction("info", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->info(msg); }))
		.addFunction("debug", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->debug(msg); }))
		.addFunction("error", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->error(msg); }))
		.addFunction("error", std::function([](Logger* thiz, const string& msg, lua_State* L) { thiz->warn(msg); }))
		.endClass()
		.addFunction("GetVersion", std::function([](lua_State* L) { return g_VERSION; }))
		.addFunction("SendGroupMsg", &SendGroupMsg)
		.addFunction("SendAllGroupMsg", &SendAllGroupMsg)
		.addFunction("RecallMsg", &RecallMsg)
		.addFunction("SendApp", &SendApp)
		.addFunction("SendPacket", &SendPacket)
		.addFunction("SetListener", &SetListener)
		.addFunction("MotdBE", &MotdBE)
		.addFunction("MotdJE", &MotdJE)
		.addFunction("Tip", &ShowTipWindow)
		.addFunction("GetAPIVersion", std::function([](lua_State* L) { return versionPacket; }))
		.addFunction("RegisterCommand", &RegisterCommand)

		.addFunction("RunCommand", &RunCommand)
		.addFunction("GetServerStatus", std::function([](lua_State* L) { return g_server->getStarted(); }))

		.addFunction("QueryInfo", &QueryInfo)
		.addFunction("Unbind", &UnbindXbox)
		.addFunction("Bind", &BindXbox)

		.addFunction("GetAdmin", &GetAdmin)
		.addFunction("GetGroup", &GetGroup)

		.addFunction("RegisterPlugin", &RegisterPlugin);

	return L;
}

void DeinitLua(lua_State* L) {
	lua_close(L);
}
