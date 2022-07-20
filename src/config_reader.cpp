#include "config_reader.h"
#include "message_box.h"
#include <qstring.h>

json yaml2json(const YAML::Node& root) {
	json j = nullptr;
	switch (root.Type()) {
	case YAML::NodeType::Null:
		break;
	case YAML::NodeType::Scalar:
		return root.as<std::string>();
	case YAML::NodeType::Sequence:
		for (const auto& node : root) {
			j.push_back(yaml2json(node));
		}
		break;
	case YAML::NodeType::Map:
		for (const auto& it : root) {
			j[it.first.as<std::string>()] = yaml2json(it.second);
		}
		break;
	default:
		break;
	}
	return j;
}

ConfigReader::ConfigReader() {}

ConfigReader::ConfigReader(const string& file) {
	readFile(file);
}

bool ConfigReader::readFile(const string& file) {
	try {
		root = YAML::LoadFile(string(file));
	}
	catch (const std::exception& e) {
		msgbox::ShowError(e.what());
		return false;
	}
	return true;
}

YAML::Node& ConfigReader::raw() {
	return root;
}

YAML::Node ConfigReader::operator[](const string& key) {
	return root[key];
}

YAML::Node ConfigReader::operator[](int key) {
	return root[key];
}
