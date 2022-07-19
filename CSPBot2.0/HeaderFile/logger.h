#pragma once
// self
#include "cspbot20.h"
#include "server.h"
#include "global.h"
#include "helper.h"
#include "websocketClient.h"
// third-party
#include <FMT/chrono.h>

string getConfig(string key);

class Logger {
public:
	inline Logger(string moduleName) {
		Module = moduleName;
	}

	template <typename... Args>
	inline void info(string msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 2) {
				pushToQueue(
					"<font color = \"#008000\">" + getTime() + " I/" + Module + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 2) {
				pushToQueue(
					"<font color = \"#008000\">" + getTime() + " I/" + Module + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	inline void error(string msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 4) {
				pushToQueue(
					"<font color = \"#FF0000\">" + getTime() + " E/" + Module + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 4) {
				pushToQueue(
					"<font color = \"#FF0000\">" + getTime() + " E/" + Module + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	inline void warn(string msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 3) {
				pushToQueue(
					"<font color = \"#FFCC66\">" + getTime() + " W/" + Module + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 3) {
				pushToQueue(
					"<font color = \"#FFCC66\">" + getTime() + " W/" + Module + ": " + msg + "\n</font>");
			}
		}
	};
	template <typename... Args>
	inline void debug(string msg, const Args&... args) {
		try {
			string str = fmt::format(msg, args...);
			if (getLevel() <= 1) {
				pushToQueue(
					"<font color = \"#6699FF\">" + getTime() + " D/" + Module + ": " + str + "\n</font>");
			}
		}
		catch (...) {
			if (getLevel() <= 1) {
				pushToQueue(
					"<font color = \"#6699FF\">" + getTime() + " D/" + Module + ": " + msg + "\n</font>");
			}
		}
	};

	//获取格式化时间
	inline static string getTime() {
		time_t tt = time(NULL);
		struct tm* t = localtime(&tt);
		std::ostringstream buffer;
		string s = std::to_string(t->tm_sec);
		if (s.length() == 1) {
			s = "0" + s;
		}
		string timeString = fmt::format("{}-{}-{} {}:{}:{}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, s);
		return timeString;
	};

private:
	inline int getLevel() {
		string level = getConfig("LoggerLevel");
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

	inline void pushToQueue(const string& log) {
		g_queue.push(log);
	};

	string Module = "";
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