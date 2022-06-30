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
	void insertBDSLog(QString log); //≤Â»ÎBDS»’÷æ
public:
	Server(QObject* parent = NULL) {};
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
};
