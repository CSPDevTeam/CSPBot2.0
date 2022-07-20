#include "event.h"
#include "helper.h"
#include "logger.h"
#include "server.h"
#include "config_reader.h"

void EventCallbacker::callback() {
	vector<string> ev = getEvent(type_);
	for (string i : ev) {
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

vector<string> EventCallbacker::getEvent(EventCode ec) {
	ConfigReader event_data("data/event.yml");
	vector<string> eventList;
	auto code = magic_enum::enum_name<EventCode>(ec);
	for (auto i : event_data[string(code)]) {
		eventList.push_back(i.as<string>());
	}
	return eventList;
}

EventCallbacker& EventCallbacker::insert(const string& item) {
	arg_.push_back(item);
	return *this;
}
