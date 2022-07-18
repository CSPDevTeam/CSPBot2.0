#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include "cspbot20.h"
#include "server.h"
#include "global.h"
#include <filesystem>
#include <FMT/chrono.h>
#include <FMT/color.h>
#include <FMT/core.h>
#include <FMT/os.h>
#include <FMT/printf.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <utility>
#include <magic_enum.hpp>
#include "helper.h"
#include "websocketClient.h"

std::string getConfig(std::string key);
class Logger {
public:
	inline Logger(std::string moduleName) {
		Module = moduleName;
	}

	template <typename... Args>
	inline void info(std::string msg, const Args&... args) {
		try {
			std::string str = fmt::format(msg, args...);
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
	inline void error(std::string msg, const Args&... args) {
		try {
			std::string str = fmt::format(msg, args...);
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
	inline void warn(std::string msg, const Args&... args) {
		try {
			std::string str = fmt::format(msg, args...);
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
	inline void debug(std::string msg, const Args&... args) {
		try {
			std::string str = fmt::format(msg, args...);
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
	inline static std::string getTime() {
		time_t tt	 = time(NULL);
		struct tm* t = localtime(&tt);
		std::ostringstream buffer;
		std::string s = std::to_string(t->tm_sec);
		if (s.length() == 1) {
			s = "0" + s;
		}
		std::string timeString = fmt::format("{}-{}-{} {}:{}:{}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, s);
		return timeString;
	};

private:
	inline int getLevel() {
		std::string level = getConfig("LoggerLevel");
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

	inline void pushToQueue(std::string log) {
		q.enqueue(Helper::stdString2QString(log));
	};

	std::string Module = "";
};

namespace fmtConsole {
std::string getCPUUsed();
std::string FmtConsoleRegular(std::string cmd);
std::string FmtGroupRegular(
	messagePacket message,
	std::string cmd);
QString getColoredLine(std::string line);
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