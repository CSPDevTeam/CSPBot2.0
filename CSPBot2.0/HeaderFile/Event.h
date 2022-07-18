#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <magic_enum.hpp>

enum inLineEvent {
	onServerStart,	 // OK
	onServerStop,	 // OK
	onSendCommand,	 // OK
	onReceiveMsg,	 // OK
	onReceivePacket, // OK
	onStop,			 // OK
	onLogin,		 // OK
	onImport,		 // OK
	onSendMsg,		 // OK
	onRecall,		 // OK
	onConnectError,	 // OK
	onConnectLost,	 // OK
	onConsoleUpdate, // OK
	onBinded,
	onUnBinded,
};

class EventCallbacker {
public:
	EventCallbacker(inLineEvent t) : type_(t), arg_() {}
	~EventCallbacker() {}

	//事件回调
	inline void callback() {
		std::vector<std::string> ev = getEvent(type_);
	}

	//获取事件
	inline std::vector<std::string> getEvent(inLineEvent ec) {
		auto yf							   = YAML::LoadFile("data/event.yml");
		std::vector<std::string> eventList = {};
		auto event_code					   = magic_enum::enum_name<inLineEvent>(ec);
		std::string code				   = std::string(event_code);
		for (auto i : yf[code]) {
			eventList.push_back(i.as<std::string>());
		}
		return eventList;
	}

	inline EventCallbacker& insert(const char* key, const std::string& item) {
		arg_[key] = item;
		return *this;
	}

private:
	inLineEvent type_;
	std::unordered_map<std::string, std::string> arg_;
};
