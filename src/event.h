#pragma once
#include "global.h"

enum class EventCode {
	onServerStart,
	onServerStop,
	onSendCommand,
	onReceiveMsg,
	onReceivePacket,
	onStop,
	onLogin,
	onImport,
	onSendMsg,
	onRecall,
	onConnectError,
	onConnectLost,
	onConsoleUpdate,
	onBinded,
	onUnBinded,
};

class EventCallbacker {
public:
	EventCallbacker(EventCode t) : type_(t), arg_() {}
	~EventCallbacker() {}

	void callback();
	vector<string> getEvent(EventCode ec);
	EventCallbacker& insert(const string& item);

private:
	EventCode type_;
	vector<string> arg_;
	int num = 1;
};
