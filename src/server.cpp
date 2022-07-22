#include "server.h"
#include "global.h"
#include "cspbot20.h"
#include "logger.h"
#include "config_reader.h"
//#include <regex>

enum stopType : int {
	normal,
	force,
	accident,
} stoptype;

bool Server::createServer() {
	myChildProcess = new QProcess(this);

	//绑定事件
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(readyReadStandardError()), this, SLOT(receiver()));
	QObject::connect(myChildProcess, SIGNAL(finished(int)), this, SLOT(progressFinished(int)));

	myChildProcess->setProcessChannelMode(QProcess::MergedChannels);
	QString runProgress;
	if (startMode == 0) {
		runProgress = "runner.bat";
	}
	else if (startMode == 1) {
		runProgress = "cmd.exe";
	}

	myChildProcess->start(runProgress);
	if (myChildProcess->waitForStarted()) {
		g_server_logger.info("服务器启动成功,PID为:{}", myChildProcess->processId());
		emit changeStatus(true);
		emit chenableForce(true);
		started = true;
	}
	else {
		g_server_logger.error("服务器启动失败,原因:{}", myChildProcess->errorString().toStdString());
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
void Server::formatBDSLog(QString s_line) {
	s_line = s_line.toUtf8();
	//去掉Color
	QRegularExpression pattern("\033\\[(.+?)m");
	auto match = pattern.match(s_line);
	QString s_line_colorless = s_line.replace(pattern, "");
	
	//分割字符串
	QStringList s_colorless_list = s_line_colorless.split("\n");
	QStringList s_color_list = s_line.split("\n");
	
	//对控制台输出色彩
	for (QString i : s_color_list) {
		i = i.replace("\n", "");
		i = i.replace("\r", "");
		if (i != "") {
			QString coloredLine = fmtConsole::getColoredLine(i);
			if (coloredLine != "") {
				emit insertBDSLog(coloredLine);
			}
		}
	}

	//去掉颜色进行回调
	for (QString i : s_colorless_list) {
		i = i.replace("\n", "");
		i = i.replace("\r", "");
		if (i != "") {
			catchInfo(i);
			selfCatchLine(i);

			// Callback
			StringMap p;
			p.emplace("line", i.toStdString());
			emit OtherCallback("onConsoleUpdate", p);
		}
	}
}

//获取日志输出
void Server::receiver() {
	char output[2049];
	while (myChildProcess->readLine(output, 2048)) {
		formatBDSLog(output);
	}
}

//获取NormalStop变量
bool Server::getNormalStop() {
	return normalStop;
};

//获取Started变量
bool Server::getStarted() {
	return started;
};

//进程结束
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

//抓取内置正则
void Server::catchInfo(QString line) {
	QRegularExpression world("worlds\\/(.+)\\/db");
	QRegularExpression version("Version\\s(.+)");
	QRegularExpression pid("PID\\s(.+)[?]");
	QRegularExpression join("Player\\sconnected:\\s(.+),\\sxuid:\\s(.+)");
	QRegularExpression difficulty("Difficulty:\\s(.+)\\s(.+)");
	auto world_matchs = world.match(line);
	auto version_matchs = version.match(line);
	auto pid_matchs = pid.match(line);
	auto join_matchs = join.match(line);
	auto difficulty_matchs = difficulty.match(line);
	if (world_matchs.hasMatch()) {
		emit chLabel("world", world_matchs.captured(1));
	}
	else if (version_matchs.hasMatch()) {
		string version_raw = version_matchs.captured(1).toStdString();
		if (version_raw.find('(') != string::npos) {		
			auto version_R_matchs = QRegularExpression("(.+)\\(").match(QString::fromStdString(version_raw));			
			if (version_R_matchs.hasMatch()) {
				version_raw = version_R_matchs.captured(1).toStdString();
			}
		}
		string version_string = version_raw;
		emit chLabel("g_VERSION", QString::fromStdString(version_string));
	}
	else if (pid_matchs.hasMatch()) {
		emit chLabel("pid", "[CSPBot] 提示:已有一个PID为" +pid_matchs.captured(1)+ "的相同目录进程，是否结束进程?（确认请输入y,取消请输入n)");
	}
	else if (join_matchs.hasMatch()) {
		Bind::bindXuid(join_matchs.captured(1).toStdString(), join_matchs.captured(2).toStdString());
	}
	else if (difficulty_matchs.hasMatch()) {
		emit chLabel("difficult", difficulty_matchs.captured(1));
	}
}

YAML::Node getRegular() {
	return g_regular.raw();
}

void Server::selfCatchLine(QString line) {
	YAML::Node regular = getRegular();

	//读取正则组
	for (YAML::Node i : regular) {
		string Regular = i["Regular"].as<string>();
		string Action = i["Action"].as<string>();
		string From = i["From"].as<string>();
		bool Permissions = i["Permissions"].as<bool>();

		auto regex = QRegularExpression(QString::fromStdString(Regular));
		auto matchs = regex.match(line);
		//执行操作
		if (matchs.hasMatch() && From == "console") {
			string Action_type = Action.substr(0, 2);
			int num = 0;
			for (auto& replace : matchs.capturedTexts()) {
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