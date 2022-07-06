#include <string>
#include <windows.h> 
#include "global.h"
#include "cspbot20.h"
#include "qthread.h"
#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <stdio.h>
#include "logger.h"
#include "websocketClient.h"
#include "helper.h"
#include "server.h"
#include <qprocess.h>

using namespace std;
Logger serverLogger("Server");

enum stopType :int {
	normal,
	force,
	accident,
} stoptype;

bool Server::createServer()
{
	STARTUPINFO si = { 0 };
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = 0;
	BOOL bRet = CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0);
	BOOL bWet = CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &sa, 0);
	if (bRet != TRUE) {
		return false;
	}
	HANDLE hTemp = GetStdHandle(STD_OUTPUT_HANDLE);
	SetStdHandle(STD_OUTPUT_HANDLE, g_hChildStd_OUT_Wr);
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdInput = g_hChildStd_IN_Rd;
	si.hStdError = g_hChildStd_OUT_Wr;
	si.hStdOutput = g_hChildStd_OUT_Wr;
	std:string s;

	//判断启动模式
	if (startMode == 0) {
		//s = "runner.bat";
		s = "cmd.exe /c cd {}&{}";
		s = fmt::format(s, getConfig("progressPath"), getConfig("progressName"));
	}
	else if (startMode == 1) {
		s = "cmd.exe";
	}

	wstring widstr = std::wstring(s.begin(), s.end());
	LPWSTR path = (LPWSTR)widstr.c_str();
	bRet = CreateProcess(NULL, path, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &this->pi);
	SetStdHandle(STD_OUTPUT_HANDLE, hTemp);
	if (bRet != TRUE)
	{
		return false;
	}
	CloseHandle(g_hChildStd_OUT_Wr);
	this->ReadNum = ReadNum;
	this->started = true;
	cin.get();
	serverLogger.info(u8"服务器启动成功,PID为:" + std::to_string(pi.dwProcessId));

	return true;
}

void selfCatchLine(QString line);

//Server主启动
void Server::run() {
	createServer();
	//绑定检测器
	ServerPoll* sp = new ServerPoll();
	connect(sp, SIGNAL(insertBDSLog(QString)), this, SLOT(slotInsertBDSLog(QString)));
	connect(sp, SIGNAL(OtherCallback(QString, StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
	connect(sp, SIGNAL(changeStatus(bool)), this, SLOT(slotChangeStatus(bool)));
	connect(sp, SIGNAL(chLabel(QString, QString)), this, SLOT(slotChLabel(QString, QString)));
	connect(sp, SIGNAL(chenableForce(bool)), this, SLOT(slotChenableForce(bool)));
	//启动检测器
	sp->start();
	if (started) {
		emit OtherCallback("onServerStart");
		while (ReadFile(this->g_hChildStd_OUT_Rd, this->ReadBuff, 8192, &this->ReadNum, NULL))
		{
			if (started) {
				ReadBuff[ReadNum] = '\0';
				std::string line = ReadBuff;
				if (!Helper::is_str_utf8(line.c_str())) {
					line = Helper::GbkToUtf8(line.c_str());
				}
				//qDebug() << stdString2QString(line);
				regex pattern("\033\\[(.+?)m");
				string nocolor_line = regex_replace(line, pattern, "");
				vector<string> words = Helper::split(line, "\n");
				vector<string> nocolor_words = Helper::split(nocolor_line, "\n");
				for (string i : words) {
					string _line = Helper::replace(i, "\n", "");
					_line = Helper::replace(_line, "\r", "");
					if (_line != "") {
						QString qline = Helper::stdString2QString(_line);
						QString coloredLine = fmtConsole::getColoredLine(_line);
						if (coloredLine != "") {
							emit insertBDSLog(coloredLine);
						}
					}
				}

				for (string i : nocolor_words) {
					string _line = Helper::replace(i, "\n", "");
					_line = Helper::replace(_line, "\r", "");
					if (_line != "") {
						QString qline = Helper::stdString2QString(_line);
						catchInfo(qline);
						selfCatchLine(qline);

						//Callback 
						std::unordered_map<string, string> p;
						p.emplace("line", _line);
						emit OtherCallback("onConsoleUpdate", p);
					}
				}
			}
			else {
				break;
			}
		}
	}

}

//强制停止
bool Server::forceStopServer() {
	try {
		QProcess process(window);
		QStringList argument;
		process.setProgram("taskkill");
		argument << "/F" << "/T" << "/PID" << Helper::stdString2QString(std::to_string(this->pi.dwProcessId));
		process.setArguments(argument);
		process.start();
		process.waitForStarted();
		process.waitForFinished();
		TypeOfStop = force;
		return true;
	}
	catch (...) {
		return false;
	}
}

//发送命令
bool Server::sendCmd(string cmd) {
	try {
		//检测停止
		if (cmd == getConfig("stopCmd") + "\n") {
			normalStop = true;
		}

		//Callback
		std::unordered_map<string, string> p;
		p.emplace("cmd", cmd);
		emit OtherCallback("onSendCommand", p);

		if (!WriteFile(this->g_hChildStd_IN_Wr, cmd.c_str(), cmd.length(), &this->ReadNum, NULL))
		{
			return false;
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

//软停止服务器
bool Server::stopServer() {
	try {
		Server::sendCmd(getConfig("stopCmd") + "\n");
		normalStop = true;
		return true;
	}
	catch (...) {
		return false;
	}

}

//获取子进程运行状态
bool Server::getPoll() {
	try {
		if (this != nullptr) {
			return WaitForSingleObject(pi.hProcess, 0);
		}
		else {
			return false;
		}
	}
	catch (...) {
		return false;
	}
}


void ServerPoll::run() {
	while (1) {
		bool poll = server->getPoll();
		if (!poll) {
			server->started = false;
			emit insertBDSLog(u8"[CSPBot] 进程已终止.");
			emit OtherCallback("onServerStop");
			emit changeStatus(false);
			emit chenableForce(false);
			emit chLabel("world", "Unkown");
			emit chLabel("version", "Unkown");
			emit chLabel("difficult", "Unkown");

			//Callback
			if (server->normalStop) {
				server->TypeOfStop = normal;
			}
			else {
				server->TypeOfStop = accident;
			}

			break;
		}
		else {
			emit changeStatus(true);
			emit chenableForce(true);
		}
		Sleep(1 * 1000);
	}
}

void Server::catchInfo(QString line) {
	QRegExp world("worlds\\/(.+)\\/db");
	QRegExp version("Version\\s(.+)");
	QRegExp PID("PID\\s(.+)[?]");
	QRegExp Join("Player\\sconnected:\\s(.+),\\sxuid:\\s(.+)");
	QRegExp Difficult("Difficulty:\\s(.+)\\s(.+)");
	int world_pos = world.indexIn(line);
	int version_pos = version.indexIn(line);
	int pid_pos = PID.indexIn(line);
	int join_pos = Join.indexIn(line);
	int difficult_pos = Difficult.indexIn(line);
	if (world_pos > -1) {
		emit chLabel("world", world.cap(1));
	}
	else if (version_pos > -1) {
		string version_raw = Helper::QString2stdString(version.cap(1));
		if (version_raw.find('(') != string::npos) {
			QRegExp version_R("(.+)\\(");
			int version_R_pos = version_R.indexIn(Helper::stdString2QString(version_raw));
			if (version_R_pos > -1) {
				version_raw = Helper::QString2stdString(version_R.cap(1));
			}
		}
		string version_string = version_raw;
		emit chLabel("version", Helper::stdString2QString(version_string));
	}
	else if (pid_pos > -1) {
		QString msg = Helper::stdString2QString(u8"[CSPBot] 提示:已有一个PID为") + PID.cap(1) + Helper::stdString2QString(u8"的相同目录进程，是否结束进程?（确认请输入y,取消请输入n)");
		emit insertBDSLog(msg);
	}
	else if (difficult_pos > -1) {
		emit chLabel("difficult", Difficult.cap(2));
	}
	else if (join_pos > -1) {
		Bind::bindXuid(Helper::QString2stdString(Join.cap(1)), Helper::QString2stdString(Join.cap(2)));
	}
}

YAML::Node getRegular() {
	std::ifstream fin("data/regular.yml");
	YAML::Node node = YAML::Load(fin); //读取regular配置文件
	return node;
}

void Server::selfCatchLine(QString line) {
	YAML::Node regular = getRegular();

	//读取正则组
	for (YAML::Node i : regular) {
		string Regular = i["Regular"].as<string>();
		string Action = i["Action"].as<string>();
		string From = i["From"].as<string>();
		bool Permissions = i["Permissions"].as<bool>();

		QRegExp r(Helper::stdString2QString(Regular));
		int r_pos = r.indexIn(line);
		//qDebug() << line << r << r_pos;
		//执行操作
		if (r_pos > -1 && From == "console") {
			string Action_type = Action.substr(0, 2);
			int num = 0;
			for (auto& replace : r.capturedTexts()) {
				Action = Helper::replace(Action, "$" + std::to_string(num), Helper::QString2stdString(replace));
				num++;
			}
			if (Action_type == "<<") {
				string cmd = Action.erase(0, 2);
				cmd = fmtConsole::FmtConsoleRegular(cmd);
				server->sendCmd(cmd + "\n");
			}
			else if (Action_type == ">>") {
				string cmd = Action.erase(0, 2);
				cmd = fmtConsole::FmtConsoleRegular(cmd);
				mirai->sendAllGroupMsg(cmd);
			}
			else {
				string ac = fmtConsole::FmtConsoleRegular(Action);
				commandApi->CustomCmd(ac, "");
			}
		}
	}
}