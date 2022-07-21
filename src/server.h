#pragma once
#include "global.h"
#include "cspbot20.h"

enum stopType;

class Server : public QObject {
	Q_OBJECT
signals:
	void insertBDSLog(QString log);							   //插入BDS日志
	void OtherCallback(QString listener, StringMap args = {}); // Callback
	void changeStatus(bool a);								   //更改状态
	void chLabel(QString title, QString content);			   //更改标签
	void chenableForce(bool a);								   //更改强制停止状态

public:
	Server(int mode = 0, QObject* parent = NULL) {
		startMode = mode;
	};
	~Server(){};

	bool createServer();
	bool forceStopServer();
	bool stopServer();
	bool sendCmd(std::string cmd);
	bool getPoll();
	void run();

	//一些关于服务器的变量
	bool getNormalStop();
	bool getStarted();

private:
	void catchInfo(QString line);	  //抓取信息
	void selfCatchLine(QString line); //抓取命令
	void formatBDSLog(QString s_qLine); //处理BDS发来的信息
	int startMode = 0;				  //开启模式
	QProcess* myChildProcess;		  //子进程指针
	bool started = false;			  //是否开启
	bool normalStop = false;		  //正常停止
	stopType TypeOfStop;			  //停止状态

private slots:

	void receiver();					 //收到日志
	void progressFinished(int exitCode); //进程结束
};

class ServerPoll : public QThread {
	Q_OBJECT
protected:
	//void run();

public:
	ServerPoll(QObject* parent = NULL){};
	~ServerPoll(){};
signals:
	void stoped();
	void insertBDSLog(QString log);							   //插入BDS日志
	void OtherCallback(QString listener, StringMap args = {}); // Callback
	void changeStatus(bool a);								   //更改状态
	void chLabel(QString title, QString content);			   //更改标签
	void chenableForce(bool a);								   //更改强制停止状态
};
