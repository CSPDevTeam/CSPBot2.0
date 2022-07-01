#pragma once
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
//#include "CPython.h"

enum stopType;

class Server :public QThread
{
	Q_OBJECT
protected:
	void run();
signals:
	void insertBDSLog(QString log); //插入BDS日志
	void OtherCallback(QString listener, StringMap args = {}); //Callback
	void changeStatus(bool a); //更改状态
	void chLabel(QString title, QString content); //更改标签
	void chenableForce(bool a); //更改强制停止状态

public:
	Server(int mode=0,QObject* parent = NULL) {
		startMode = mode;
	};
	~Server() {};
	bool started = false;
	bool normalStop = false;
	stopType TypeOfStop;
	bool createServer();
	bool forceStopServer();
	bool stopServer();
	bool sendCmd(std::string cmd);
	bool getPoll();
	BOOL bRet;
	char ReadBuff[8192];
	DWORD ReadNum = 0;
	PROCESS_INFORMATION pi;
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	DEBUG_EVENT de;
private:
	void catchInfo(QString line); //抓取信息
	void selfCatchLine(QString line); //抓取命令
	int startMode = 0;

private slots:
	inline void slotInsertBDSLog(QString log) {
		emit insertBDSLog(log);
	}; //插入BDS日志
	inline void slotOtherCallback(QString listener, StringMap args = {}) {
		emit OtherCallback(listener, args);
	}; //Callback
	inline void slotChangeStatus(bool a) {
		emit changeStatus(a);
	}; //更改状态
	inline void slotChLabel(QString title, QString content) {
		emit chLabel(title, content);
	}; //更改标签
	inline void slotChenableForce(bool a) {
		emit chenableForce(a);
	}; //更改强制停止状态
};

class ServerPoll :public QThread
{
	Q_OBJECT
protected:
	void run();
public:
	ServerPoll(QObject* parent = NULL) {};
	~ServerPoll() {};
signals:
	void stoped();
	void insertBDSLog(QString log); //插入BDS日志
	void OtherCallback(QString listener, StringMap args = {}); //Callback
	void changeStatus(bool a); //更改状态
	void chLabel(QString title, QString content); //更改标签
	void chenableForce(bool a); //更改强制停止状态

};
