#pragma once
#include "global.h"
#include "ws_client.h"
#include <fmt/format.h>

string GetConfig(const string& key);

class Logger {
public:
	Logger(const string& title) : title_(title) {}

	template <typename... Args>
	void info(const string& msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 2) {
				pushToQueue("<font color = \"#008000\">" + getTime() + " I/" + title_ + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 2) {
				pushToQueue("<font color = \"#008000\">" + getTime() + " I/" + title_ + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	void error(const string& msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 4) {
				pushToQueue("<font color = \"#FF0000\">" + getTime() + " E/" + title_ + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 4) {
				pushToQueue("<font color = \"#FF0000\">" + getTime() + " E/" + title_ + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	void warn(const string& msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 3) {
				pushToQueue("<font color = \"#FFCC66\">" + getTime() + " W/" + title_ + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 3) {
				pushToQueue("<font color = \"#FFCC66\">" + getTime() + " W/" + title_ + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	void debug(const string& msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 1) {
				pushToQueue("<font color = \"#6699FF\">" + getTime() + " D/" + title_ + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 1) {
				pushToQueue("<font color = \"#6699FF\">" + getTime() + " D/" + title_ + ": " + msg + "\n</font>");
			}
		}
	};

	//获取格式化时间
	static string getTime() {
		time_t tt = time(NULL);
		struct tm* t = localtime(&tt);
		string s = std::to_string(t->tm_sec);
		if (s.length() == 1) {
			s = "0" + s;
		}
		string timeString = fmt::format("{}-{}-{} {}:{}:{}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, s);
		return timeString;
	};

private:
	int getLevel() {
		string level = GetConfig("LoggerLevel");
		if (level == "!failed!") {
			level = "debug";
		}
		if (level == "info") {
			return 2;
		}
		else if (level == "warn") {
			return 3;
		}
		else if (level == "error") {
			return 4;
		}
		else {
			return 1;
		}
	};

	void pushToQueue(const string& log) {
		g_queue.push(log); //推送到Queue
	};

	string title_ = "";
};

namespace fmtConsole {
string getCPUUsed();
string FmtConsoleRegular(string cmd);
string FmtGroupRegular(messagePacket message, string cmd);
QString getColoredLine(string line);
} // namespace fmtConsole

class LoggerReader : public QThread {
	Q_OBJECT
protected:
	void run();
signals:
	void updateLog(QString log); //输出日志
public:
	LoggerReader(QObject* parent = NULL){};
	~LoggerReader(){};
};