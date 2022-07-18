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

enum stopType : int {
	normal,
	force,
	accident,
} stoptype;

void setUTF8() {
#ifdef _WIN32
	// SetConsoleOutputCP(65001);
	CONSOLE_FONT_INFOEX info = {0};
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = 16; // leave X as zero
	info.FontWeight = FW_NORMAL;
	// wcscpy(info.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
#endif
}

bool Server::createServer() {
	setUTF8();
	myChildProcess = new QProcess(this);

	//绑定事件
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardError()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(finished(int)), this, SLOT(progressFinished(int)));

	myChildProcess->setProcessChannelMode(QProcess::MergedChannels);
	string runProgress;
	if (startMode == 0) {
		runProgress = "cmd.exe /c chcp 65001&cd \"{}\"&\"{}\"";
		runProgress = fmt::format(runProgress, getConfig("progressPath"), getConfig("progressName"));
	}
	else if (startMode == 1) {
		runProgress = "cmd.exe";
	}

	myChildProcess->start(Helper::stdString2QString(runProgress));
	if (myChildProcess->waitForStarted()) {
		serverLogger.info(u8"服务器启动成功,PID为:{}", myChildProcess->processId());
		emit changeStatus(true);
		emit chenableForce(true);
		started = true;
	}
	else {
		serverLogger.error(u8"服务器启动失败,原因:{}", Helper::QString2stdString(myChildProcess->errorString()));
	}


	return true;
}

// Server主启动
void Server::run() {
	createServer();
	if (started) {
		emit OtherCallback("onServerStart");
	}
}

//强制停止
bool Server::forceStopServer() {
	myChildProcess->kill();
	return true;
}

//发送命令
bool Server::sendCmd(string cmd) {
	qDebug() << Helper::stdString2QString(cmd);

	//检测停止
	if (cmd == getConfig("stopCmd") + "\n") {
		normalStop = true;
	}

	// Callback
	std::unordered_map<string, string> p;
	p.emplace("cmd", cmd);
	emit OtherCallback("onSendCommand", p);
	myChildProcess->write(cmd.c_str());
	return true;
}

//软停止服务器
bool Server::stopServer() {
	Server::sendCmd(getConfig("stopCmd") + "\n");
	normalStop = true;
	return true;
}

//处理BDS消息
void Server::formatBDSLog(string line) {
	//去掉Color并分割
	regex pattern("\033\\[(.+?)m");
	string nocolor_line = regex_replace(line, pattern, "");
	vector<string> nocolor_words = Helper::split(nocolor_line, "\n");

	//色彩格式化
	vector<string> words = Helper::split(line, "\n");

	//对控制台输出色彩
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

	//去掉颜色进行回调
	for (string i : nocolor_words) {
		string _line = Helper::replace(i, "\n", "");
		_line = Helper::replace(_line, "\r", "");
		if (_line != "") {
			QString qline = Helper::stdString2QString(_line);
			catchInfo(qline);
			selfCatchLine(qline);

			// Callback
			std::unordered_map<string, string> p;
			p.emplace("line", _line);
			emit OtherCallback("onConsoleUpdate", p);
		}
	}
}

//获取日志输出
void Server::receiver() {
	char output[2049];
	while (myChildProcess->readLine(output, 2048)) {
		string line = output;
		formatBDSLog(line);
	}
	// emit insertBDSLog(Helper::stdString2QString(output));
	// formatBDSLog(output);
}


//获取NormalStop变量
bool Server::getNormalStop() {
	return normalStop;
};

//获取Started变量
bool Server::getStarted() {
	return started;
};

void Server::progressFinished(int exitCode) {
	server->started = false;
	emit insertBDSLog(
		u8"[CSPBot] 进程已终止. 结束代码:" + Helper::stdString2QString(to_string(exitCode)));
	emit OtherCallback("onServerStop");
	emit changeStatus(false);
	emit chenableForce(false);
	emit chLabel("world", "Unkown");
	emit chLabel("version", "Unkown");
	emit chLabel("difficult", "Unkown");

	// Callback
	if (server->getNormalStop()) {
		server->TypeOfStop = normal;
	}
	else {
		server->TypeOfStop = accident;
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
		// qDebug() << line << r << r_pos;
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