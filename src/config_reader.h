#pragma once
#include <yaml-cpp/yaml.h>
#include <json.hpp>

using nlohmann::json;
using std::string;

json yaml2json(const YAML::Node& root);

class ConfigReader {
public:
	ConfigReader();
	ConfigReader(const string& file);
	void readFile(const string& file);
	YAML::Node& raw();
	YAML::Node operator[](const string& key);
	YAML::Node operator[](int key);

private:
	YAML::Node root;
};
