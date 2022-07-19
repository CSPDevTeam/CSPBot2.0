﻿#pragma once
// self
#include <helper.h>
#include <logger.h>
#include <plugins.h>
// third-party
#include <yaml-cpp/yaml.h>
#include <magic_enum.hpp>

class EventCallbacker {
public:
	EventCallbacker(EventCode t) : type_(t), arg_() {}
	~EventCallbacker() {}

	//事件回调
	inline void callback() {
		std::vector<std::string> ev = getEvent(type_);
		for (std::string i : ev) {
			i = fmtConsole::FmtConsoleRegular(i);
			string Action_type = i.substr(0, 2);
			for (string j : arg_) {
				i = helper::replace(i, "$" + std::to_string(num), j);
				num++;
			}
			if (Action_type == "<<") {
				string cmd = i.erase(0, 2);
				g_server->sendCmd(cmd + "\n");
			}
			else if (Action_type == ">>") {
				string cmd = i.erase(0, 2);
				g_mirai->sendAllGroupMsg(cmd);
			}
			else {
				g_cmd_api->CustomCmd(i, "");
			}
		}
	}

	//获取事件
	inline std::vector<std::string> getEvent(EventCode ec) {
		auto yf = YAML::LoadFile("data/event.yml");
		std::vector<std::string> eventList = {};
		auto event_code = magic_enum::enum_name<EventCode>(ec);
		std::string code = std::string(event_code);
		for (auto i : yf[code]) {
			eventList.push_back(i.as<std::string>());
		}
		return eventList;
	}

	inline EventCallbacker& insert(const std::string& item) {
		arg_.push_back(item);
		return *this;
	}

private:
	EventCode type_;
	std::vector<std::string> arg_;
	int num = 1;
};
