#pragma once
#include <yaml-cpp/yaml.h>
#include <Nlohmann/json.hpp>
using json = nlohmann::json;

inline json yaml2json(const YAML::Node& root) {
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

inline json yaml2json(const std::string& str) {
	YAML::Node root;
	try {
		root = YAML::Load(str);
	}
	catch (const std::exception&) {
		return json{};
	}
	return yaml2json(root);
}