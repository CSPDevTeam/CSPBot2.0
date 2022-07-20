#pragma once
#include <ctime>
#include <optional>
#include <sstream>

#include "cspbot20.h"
#include "helper.h"
#include "ws_client.h"
#include "plugins.h"
#include "helper.h"
#include "server.h"
#include "global.h"

using namespace std;

string fmtMotdBE(string msgJson, string returnMsg);
string fmtMotdJE(string msgJson, string returnMsg);

vector<string> CommandAPI::SplitCommand(const std::string& paras) {
	if (paras.empty())
		return vector<string>();

	vector<string> res;
	string now, strInQuote = "";
	istringstream strIn(paras);
	while (strIn >> now) {
		if (!strInQuote.empty()) {
			strInQuote = strInQuote + " " + now;
			if (now.back() == '\"') {
				strInQuote.pop_back();
				res.push_back(strInQuote.erase(0, 1));
				strInQuote = "";
			}
		}
		else {
			if (now.front() == '\"') {
				if (now.back() == '\"') {
					now = now.substr(1, now.size() - 2);
					res.push_back(now);
				}
				else
					strInQuote = now;
			}
			else
				res.push_back(now);
		}
	}
	if (!strInQuote.empty()) {
		istringstream leftIn(strInQuote);
		while (leftIn >> now)
			res.push_back(now);
	}
	for (int i = 0; i < res.size(); i++) {
		res[i] = helper::replace(res[i], "\"", "");
	}
	return res;
}

void CommandAPI::CustomCmd(string cmd, string group) {
	vector<string> sp = SplitCommand(cmd);
	string Action_Type = sp[0];
	if (Action_Type == "bind") {
		if (sp.size() > 2) {
			bool ret = Bind::bind(sp[1], sp[2]);
			if (ret) {
				emit Callback("onBinded", {});
			}
		}
		else {
			g_mirai->sendGroupMsg(group, "参数错误!");
		}
	}
	else if (Action_Type == "unbind") {
		if (sp.size() > 1) {
			bool ret = Bind::unbind(sp[1]);
			if (ret) {
				emit Callback("onUnBinded", {});
			}
		}
		else {
			g_mirai->sendGroupMsg(group, "参数错误!");
		}
	}
	else if (Action_Type == "motdbe") {
		if (sp.size() > 2) {
			QRegExp r("(\\w.+):(\\w+)");
			int r_pos = r.indexIn(QString::fromStdString(sp[1]));
			if (r_pos > -1) {
				string motd_respone = Motd::motdbe(sp[1]);
				string fmt_respone;
				fmt_respone = fmtMotdBE(motd_respone, sp[2]);
				g_mirai->sendGroupMsg(group, fmt_respone);
			}
			else {
				g_mirai->sendGroupMsg(group, "Motd地址错误!");
			}
		}
		else {
			g_mirai->sendGroupMsg(group, "参数错误!");
		}
	}
	else if (Action_Type == "motdje") {
		if (sp.size() > 2) {
			QRegExp r("(\\w.+):(\\w+)");
			int r_pos = r.indexIn(QString::fromStdString(sp[1]));
			if (r_pos > -1) {
				string motd_respone = Motd::motdje(sp[1]);
				string fmt_respone;
				fmt_respone = fmtMotdJE(motd_respone, sp[2]);
				g_mirai->sendGroupMsg(group, fmt_respone);
			}
			else {
				g_mirai->sendGroupMsg(group, "Motd地址错误!");
			}
		}
		else {
			g_mirai->sendGroupMsg(group, "参数错误!");
		}
	}
	else if (Action_Type == "start") {
		g_mirai->sendGroupMsg(group, "正在开启服务器...");
		emit signalStartServer();
	}
	else if (Action_Type == "stop") {
		if (g_server->getStarted()) {
			g_server->stopServer();
		}
		else if (g_server->getStarted() != false && group != "0") {
			g_mirai->sendGroupMsg(group, "服务器不在运行中");
		}
	}
	else if (g_command.find(Action_Type) != g_command.end()) {
		StringVector args;
		int num = 0;
		for (auto& i : sp) {
			if (num != 0) {
				args.push_back(i);
			}
			num++;
		}
		emit signalCommandCallback(QString::fromStdString(Action_Type), args);
	}
}
