#include "server.h"
#include "global.h"
#include "cspbot20.h"
#include "logger.h"
#include "config_reader.h"
#include <regex>

//#include <windows.h>
//#include <qthread.h>
//#include <iostream>
//#include <tchar.h>
//#include <stdio.h>
//#include <string>
//#include <regex>
//#include <yaml-cpp/yaml.h>
//#include <stdio.h>
//#include "logger.h"
//#include "helper.h"
//#include <qprocess.h>

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
	// setUTF8();
	myChildProcess = new QProcess(this);

	//绑定事件
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardError()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(finished(int)), this, SLOT(progressFinished(int)));

	myChildProcess->setProcessChannelMode(QProcess::MergedChannels);
	string runProgress;
	if (startMode == 0) {
		runProgress = "cmd.exe /c chcp 65001&cd \"{}\"&\"{}\"";
		runProgress = fmt::format(runProgress, GetConfig("progressPath"), GetConfig("progressName"));
	}
	else if (startMode == 1) {
		runProgress = "cmd.exe";
	}

	myChildProcess->start(QString::fromStdString(runProgress));
	if (myChildProcess->waitForStarted()) {
		serverLogger.info("服务器启动成功,PID为:{}", myChildProcess->processId());
		emit changeStatus(true);
		emit chenableForce(true);
		started = true;
	}
	else {
		serverLogger.error("服务器启动失败,原因:{}", myChildProcess->errorString().toStdString());
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
	QProcess::startDetached("taskkill -t  -f /pid " + QString::number(myChildProcess->processId()));
	return true;
}

//发送命令
bool Server::sendCmd(string cmd) {
	qDebug() << QString::fromStdString(cmd);

	//检测停止
	if (cmd == GetConfig("stopCmd") + "\n") {
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
	string stopCmd = GetConfig("stopCmd");
	if (stopCmd == "!failed!") {
		stopCmd = "stop";
	}
	Server::sendCmd(stopCmd + "\n");
	normalStop = true;
	return true;
}

//处理BDS消息
void Server::formatBDSLog(string line) {
	//去掉Color并分割
	std::regex pattern("\033\\[(.+?)m");
	string nocolor_line = regex_replace(line, pattern, "");
	vector<string> nocolor_words = helper::split(nocolor_line, "\n");

	//色彩格式化
	vector<string> words = helper::split(line, "\n");

	//对控制台输出色彩
	for (string i : words) {
		string _line = helper::replace(i, "\n", "");
		_line = helper::replace(_line, "\r", "");
		if (_line != "") {
			QString qline = QString::fromStdString(_line);
			QString coloredLine = fmtConsole::getColoredLine(_line);
			if (coloredLine != "") {
				emit insertBDSLog(coloredLine);
			}
		}
	}

	//去掉颜色进行回调
	for (string i : nocolor_words) {
		string _line = helper::replace(i, "\n", "");
		_line = helper::replace(_line, "\r", "");
		if (_line != "") {
			QString qline = QString::fromStdString(_line);
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
	// emit insertBDSLog(output).c_str();
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
	g_server->started = false;
	emit insertBDSLog("[CSPBot] 进程已终止. 结束代码:" + QString::number(exitCode));
	emit OtherCallback("onServerStop");
	emit changeStatus(false);
	emit chenableForce(false);
	emit chLabel("world", "Unkown");
	emit chLabel("g_VERSION", "Unkown");
	emit chLabel("difficult", "Unkown");

	// Callback
	if (g_server->getNormalStop()) {
		g_server->TypeOfStop = normal;
	}
	else {
		g_server->TypeOfStop = accident;
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
		string version_raw = version.cap(1).toStdString();
		if (version_raw.find('(') != string::npos) {
			QRegExp version_R("(.+)\\(");
			int version_R_pos = version_R.indexIn(QString::fromStdString(version_raw));
			if (version_R_pos > -1) {
				version_raw = version_R.cap(1).toStdString();
			}
		}
		string version_string = version_raw;
		emit chLabel("g_VERSION", QString::fromStdString(version_string));
	}
	else if (pid_pos > -1) {
		QString msg = "[CSPBot] 提示:已有一个PID为" + PID.cap(1) + "的相同目录进程，是否结束进程?（确认请输入y,取消请输入n)";
		emit insertBDSLog(msg);
	}
	else if (difficult_pos > -1) {
		emit chLabel("difficult", Difficult.cap(2));
	}
	else if (join_pos > -1) {
		Bind::bindXuid(Join.cap(1).toStdString(), Join.cap(2).toStdString());
	}
}

YAML::Node getRegular() {
	ConfigReader regular("data/regular.yml");
	return regular.raw();
}

void Server::selfCatchLine(QString line) {
	YAML::Node regular = getRegular();

	//读取正则组
	for (YAML::Node i : regular) {
		string Regular = i["Regular"].as<string>();
		string Action = i["Action"].as<string>();
		string From = i["From"].as<string>();
		bool Permissions = i["Permissions"].as<bool>();

		QRegExp r(QString::fromStdString(Regular));
		int r_pos = r.indexIn(line);
		// qDebug() << line << r << r_pos;
		//执行操作
		if (r_pos > -1 && From == "console") {
			string Action_type = Action.substr(0, 2);
			int num = 0;
			for (auto& replace : r.capturedTexts()) {
				Action = helper::replace(Action, "$" + std::to_string(num), replace.toStdString());
				num++;
			}
			if (Action_type == "<<") {
				string cmd = Action.erase(0, 2);
				cmd = fmtConsole::FmtConsoleRegular(cmd);
				g_server->sendCmd(cmd + "\n");
			}
			else if (Action_type == ">>") {
				string cmd = Action.erase(0, 2);
				cmd = fmtConsole::FmtConsoleRegular(cmd);
				g_mirai->sendAllGroupMsg(cmd);
			}
			else {
				string ac = fmtConsole::FmtConsoleRegular(Action);
				g_cmd_api->CustomCmd(ac, "");
			}
		}
	}
}