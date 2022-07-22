#include "plugins.h"
#include "global.h"
#include "server.h"
#include "config_reader.h"
#include "helper.h"
#include "logger.h"
#include "message_box.h"

#define PLUGIN_PATH "plugins\\"

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

bool RunCommand(const string& cmd) {
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

bool SetListener(const string& eventName, const string& func) {
	auto event_code = magic_enum::enum_cast<EventCode>(eventName);
	if (!event_code)
		throw std::invalid_argument("Invalid event name " + eventName);

	//添加回调函数
	g_cb_functions[event_code.value()].push_back(func);
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

string QButtonToString(QMessageBox::StandardButton c) {
	if (c == QMessageBox::NoButton) {
		return "NoButton";
	} else if (c == QMessageBox::Ok) {
		return "Ok";
	} else if (c == QMessageBox::Save) {
		return "Save";
	} else if (c == QMessageBox::SaveAll) {
		return "SaveAll";
	} else if (c == QMessageBox::Open) {
		return "Open";
	} else if (c == QMessageBox::Yes) {
		return "Yes";
	} else if (c == QMessageBox::YesToAll) {
		return "YesToAll";
	} else if (c == QMessageBox::No) {
		return "No";
	} else if (c == QMessageBox::NoToAll) {
		return "NoToAll";
	} else if (c == QMessageBox::Abort) {
		return "Abort";
	} else if (c == QMessageBox::Retry) {
		return "Retry";
	} else if (c == QMessageBox::Ignore) {
		return "Ignore";
	} else if (c == QMessageBox::Close) {
		return "Close";
	} else if (c == QMessageBox::Cancel) {
		return "Cancel";
	} else if (c == QMessageBox::Discard) {
		return "Discard";
	} else if (c == QMessageBox::Help) {
		return "Help";
	} else if (c == QMessageBox::Apply) {
		return "Apply";
	} else if (c == QMessageBox::Reset) {
		return "Reset";
	} else if (c == QMessageBox::RestoreDefaults) {
		return "RestoreDefaults";
	} else {
		return "";
	}
}

QMessageBox::StandardButton StringToQButton(string c) {
	if (c == "NoButton") {
		return QMessageBox::NoButton;
	} else if (c == "Ok") {
		return QMessageBox::Ok;
	} else if (c == "Save") {
		return QMessageBox::Save;
	} else if (c == "SaveAll") {
		return QMessageBox::SaveAll;
	} else if (c == "Open") {
		return QMessageBox::Open;
	} else if (c == "Yes") {
		return QMessageBox::Yes;
	} else if (c == "YesToAll") {
		return QMessageBox::YesToAll;
	} else if (c == "No") {
		return QMessageBox::No;
	} else if (c == "NoToAll") {
		return QMessageBox::NoToAll;
	} else if (c == "Abort") {
		return QMessageBox::Abort;
	} else if (c == "Retry") {
		return QMessageBox::Retry;
	} else if (c == "Ignore") {
		return QMessageBox::Ignore;
	} else if (c == "Close") {
		return QMessageBox::Close;
	} else if (c == "Cancel") {
		return QMessageBox::Cancel;
	} else if (c == "Discard") {
		return QMessageBox::Discard;
	} else if (c == "Help") {
		return QMessageBox::Help;
	} else if (c == "Apply") {
		return QMessageBox::Apply;
	} else if (c == "Reset") {
		return QMessageBox::Reset;
	} else if (c == "RestoreDefaults") {
		return QMessageBox::RestoreDefaults;
	} else {
		return QMessageBox::FlagMask;
	}
}

//构造弹窗
string ShowTipWindow(const string& type, const string& title, const string& content, const vector<string>& buttonType) {
	QFlags<QMessageBox::StandardButton> btn;
	for (auto& b : buttonType) {
		auto event_code = StringToQButton(b);

		if (event_code == QMessageBox::FlagMask) {
			throw std::invalid_argument("Invalid StandardButton name " + b);
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

bool RegisterCommand(const string& cmd, const string& cbf) {
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
json QueryInfo(const string& type, const string& arg) {
	if (type != "qq" && type != "xuid" && type != "player") {
		throw std::invalid_argument("Invalid type:" + type);
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
auto GetGroup() {
	return g_config["group"];
}

auto GetAdmin() {
	return g_config["admin"];
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

bool RegisterPlugin(const string& name, const string& description, const string& author, const string& version) {
	if (HasPlugin(name))
		return false;
	g_plugins.push_back({name, description, author, version});
	return true;
}
