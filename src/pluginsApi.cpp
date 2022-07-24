#include "pluginManager.h"
#include "global.h"
#include "server.h"
#include "config_reader.h"
#include "helper.h"
#include "logger.h"
#include "plugin.h"
#include "message_box.h"

#define PLUGIN_PATH "plugins\\"

//######################### Server #########################

bool runCommand(const string& cmd) {
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
	} else if (type == "sendAllGroup") {
		string msg = qm.at("msg");
		g_mirai->sendAllGroupMsg(msg, false);
	} else if (type == "recallMsg") {
		string target = qm.at("target");
		g_mirai->recallMsg(target, false);
	} else if (type == "App") {
		string group = qm.at("group");
		string code = qm.at("code");
		g_mirai->send_app(group, code);
	} else if (type == "sendPacket") {
		string code = qm.at("packet");
		g_mirai->SendPacket(code);
	}
	return true;
}
// Mirai API
void SendGroupMsg(const string& group, const string& msg) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("msg", msg);
	ThreadMirai("sendGroup", data);
}

void SendAllGroupMsg(const string& msg) {
	std::unordered_map<string, string> data;
	data.emplace("msg", msg);
	ThreadMirai("sendAllGroup", data);
}

void RecallMsg(const string& target) {
	std::unordered_map<string, string> data;
	data.emplace("target", target);
	ThreadMirai("recallMsg", data);
}

void SendApp(const string& group, const string& code) {
	std::unordered_map<string, string> data;
	data.emplace("group", group);
	data.emplace("code", code);
	ThreadMirai("App", data);
}

void SendPacket(const string& packet) {
	std::unordered_map<string, string> data;
	data.emplace("packet", packet);
	ThreadMirai("sendPacket", data);
}

//######################### Listener #########################

bool SetListener(EventCode evc, EventCallback func) {
	//添加回调函数
	g_cb_functions[evc].push_back(func);
	return true;
}

//######################### Motd #########################

string MotdJE(const string& host) {
	auto regex = QRegularExpression("(\\w.+):(\\w+)");
	auto match = regex.match(QString::fromStdString(host));
	if (match.hasMatch()) {
		return Motd::motdje(host);
	} else {
		return "{}";
	}
}

string MotdBE(const string& host) {
	auto regex = QRegularExpression("(\\w.+):(\\w+)");
	auto matchs = regex.match(QString::fromStdString(host));
	if (matchs.hasMatch()) {
		return Motd::motdbe(host);
	} else {
		return "{}";
	}
}

//######################### Window  #########################

//构造弹窗
button_Type ShowTipWindow(window_Type type, const string& title, const string& content, const vector<button_Type>& buttonType) {
	QFlags<QMessageBox::StandardButton> btn;
	for (button_Type b : buttonType) {
		QMessageBox::StandardButton s_btn = QMessageBox::StandardButton(b);

		btn = btn | s_btn;
	}
	//展示窗口
	QMessageBox::StandardButton Choosedbtn;
	switch (type) {
		case information:
			Choosedbtn = QMessageBox::information(
				g_main_window,
				QString::fromStdString(title),
				QString::fromStdString(content),
				btn);
			break;
		case question:
			Choosedbtn = QMessageBox::question(
				g_main_window,
				QString::fromStdString(title),
				QString::fromStdString(content),
				btn);
			break;
		case warning:
			Choosedbtn = QMessageBox::warning(
				g_main_window,
				QString::fromStdString(title),
				QString::fromStdString(content),
				btn);
			break;
		case critical:
			Choosedbtn = QMessageBox::critical(
				g_main_window,
				QString::fromStdString(title),
				QString::fromStdString(content),
				btn);
			break;
		default:
			break;
	}


	//返回值
	return button_Type(Choosedbtn);
}

//######################### Command #########################

bool RegisterCommand(const string& cmd, CommandCallback cbf) {
	if (g_command.find(cmd) != g_command.end() &&
		cmd != "bind" &&
		cmd != "unbind" &&
		cmd != "motdbe" &&
		cmd != "motdje" &&
		cmd != "start" &&
		cmd != "stop") {
		return false;
		throw std::runtime_error("Invalid command:" + cmd);
	}
	g_command[cmd].push_back(cbf);
	return true;
}

//######################### Player #########################
json QueryInfo(const string& type, const string& arg) {
	if (type != "qq" && type != "xuid" && type != "player") {
		throw std::runtime_error("Invalid type:" + type);
	}

	auto queryData = Bind::queryXboxID(type, arg);
	return yaml2json(queryData).dump();
}

bool UnbindXbox(string qq) {
	return Bind::unbind(qq);
}

bool BindXbox(string name, string qq) {
	return Bind::bind(qq, name);
}

//######################### Info #########################
vector<string> GetGroup() {
	vector<string> group;
	for (auto i : g_config["group"]) {
		group.push_back(i.as<string>());
	}
	return group;
}

vector<string> GetAdmin() {
	vector<string> admin;
	for (auto i : g_config["admin"]) {
		admin.push_back(i.as<string>());
	}
	return admin;
}

//Logger
cspLogger::cspLogger(string name) {
	_modelName = name;
	_logger = new Logger("[" + _modelName + "]");
}

template <typename... Args>
void cspLogger::info(const char* msg, Args&&... args) {
	_logger->info(msg, std::forward<Args>(args)...);
}

template <typename... Args>
void cspLogger::warn(const char* msg, Args&&... args) {
	_logger->warn(msg, std::forward<Args>(args)...);
}

template <typename... Args>
void cspLogger::error(const char* msg, Args&&... args) {
	_logger->error(msg, std::forward<Args>(args)...);
}

template <typename... Args>
void cspLogger::debug(const char* msg, Args&&... args) {
	_logger->debug(msg, std::forward<Args>(args)...);
}
