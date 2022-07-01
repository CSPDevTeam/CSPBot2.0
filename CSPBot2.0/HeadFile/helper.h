#pragma once
#include <string>
#include "qstring.h"
#include <stdlib.h>
#include <windows.h>
#include <fstream>


//////////////////////////////////////////// HeadFile ////////////////////////////////////////////
#include <yaml-cpp/yaml.h>
#include <Nlohmann/json.hpp>
using json = nlohmann::json;

namespace Helper{

	//将std::string转换为QString
	inline QString stdString2QString(std::string str) {
		QByteArray byteArray(str.c_str(), str.length());
		QString msg = byteArray;
		return msg;
	}

	//将QString转换为std::string
	inline std::string QString2stdString(QString str) {
		QByteArray bytes = str.toUtf8();
		std::string msg = bytes;
		return msg;
	}

	//字符串替换
	inline std::string replace(std::string strSrc,
		const std::string& oldStr, const std::string& newStr, int count = -1)
	{
		std::string strRet = strSrc;
		size_t pos = 0;
		int l_count = 0;
		if (-1 == count) // replace all
			count = strRet.size();
		while ((pos = strRet.find(oldStr, pos)) != std::string::npos)
		{
			strRet.replace(pos, oldStr.size(), newStr);
			if (++l_count >= count) break;
			pos += newStr.size();
		}
		return strRet;
	}

	//切割字符串
	inline std::vector<std::string> split(const std::string& str, const std::string& pattern)
	{
		std::vector<std::string> res;
		if (str == "")
			return res;
		std::string strs = str + pattern;
		size_t pos = strs.find(pattern);

		while (pos != strs.npos)
		{
			std::string temp = strs.substr(0, pos);
			res.push_back(temp);
			strs = strs.substr(pos + 1, strs.size());
			pos = strs.find(pattern);
		}

		return res;
	}

	//检测string是否是UTF-8
	inline bool is_str_utf8(const char* str)
	{
		unsigned int nBytes = 0;
		unsigned char chr = *str;
		bool bAllAscii = true;
		for (unsigned int i = 0; str[i] != '\0'; ++i) {
			chr = *(str + i);
			if (nBytes == 0 && (chr & 0x80) != 0) {bAllAscii = false;}
			if (nBytes == 0) {
				if (chr >= 0x80) {
					if (chr >= 0xFC && chr <= 0xFD) {nBytes = 6;}
					else if (chr >= 0xF8) {nBytes = 5;}
					else if (chr >= 0xF0) {nBytes = 4;}
					else if (chr >= 0xE0) {nBytes = 3;}
					else if (chr >= 0xC0) {nBytes = 2;}
					else {return false;}
					nBytes--;
				}
			}
			else {
				if ((chr & 0xC0) != 0x80) {return false;}
				nBytes--;
			}}
		if (nBytes != 0) {return false;}
		if (bAllAscii) {return true;}
		return true;
	}

	//GBK与UTF-8互转
	inline std::string GbkToUtf8(const char* src_str)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		std::string strTemp = str;
		if (wstr) delete[] wstr;
		if (str) delete[] str;
		return strTemp;
	}

	inline std::string Utf8ToGbk(const char* src_str)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
		wchar_t* wszGBK = new wchar_t[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char* szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		std::string strTemp(szGBK);
		if (wszGBK) delete[] wszGBK;
		if (szGBK) delete[] szGBK;
		return strTemp;
	}
}

namespace Basic {
	class Command {
	public:
		static std::vector<std::string> SplitCommand(const std::string& parent);
		static void CustomCmd(std::string cmd, std::string group);
	};
}

//消息结构体
struct Message {
	std::string group;
	std::string qqNum;
	std::string message;
	std::string qqNick;
};

namespace Motd {
	inline HMODULE h = LoadLibraryA("Motd.dll");
	typedef char* (*funcPtrBE)(char* Host);
	typedef char* (*funcPtrJE)(char* Host);

	inline std::string motdbe(std::string host) {
		auto func = (funcPtrBE)GetProcAddress(h, "MotdBEJson");
		if (NULL == h || INVALID_HANDLE_VALUE == h)
		{
			return "{\"status\":\"error\"}";
		}
		char* st1 = const_cast<char*>(host.c_str());
		return func(st1);
	}

	inline std::string motdje(std::string host) {
		auto func = (funcPtrJE)GetProcAddress(h, "MotdJEJson");
		if (NULL == h || INVALID_HANDLE_VALUE == h)
		{
			return "{\"status\":\"error\"}";
		}
		char* st1 = const_cast<char*>(host.c_str());
		return func(st1);
	}
}

namespace Bind {
	inline bool bind(std::string qq, std::string name) {
		YAML::Node player = YAML::LoadFile("data/player.yml");

		std::vector<std::string> player_list;
		std::vector<std::string> qq_list;
		for (YAML::Node i : player) {
			player_list.push_back(i["playerName"].as<std::string>());
			qq_list.push_back(i["qq"].as<std::string>());
		}

		//检测有没有已绑定的账号
		if (std::find(player_list.begin(), player_list.end(), name) == player_list.end() && \
			std::find(qq_list.begin(), qq_list.end(), qq) == qq_list.end()) {
			std::ofstream fout("data/player.yml");
			YAML::Node pl;
			pl["playerName"] = name;
			pl["qq"] = qq;
			pl["xuid"] = "";
			player.push_back(pl);
			fout << YAML::Dump(player);
		}
		else {
			return false;
		}
		return true;
	}

	inline bool unbind(std::string qq) {
		YAML::Node player = YAML::LoadFile("data/player.yml");
		std::vector<std::string> qq_list;
		for (YAML::Node i : player) {
			qq_list.push_back(i["qq"].as<std::string>());
		}

		//检测有没有已绑定的账号
		if (std::find(qq_list.begin(), qq_list.end(), qq) != qq_list.end()) {
			std::ofstream fout("data/player.yml");
			auto it = find(qq_list.begin(), qq_list.end(), qq);
			if (it != qq_list.end())
			{
				int index = it - qq_list.begin();
				player.remove(index);
			}
			fout << YAML::Dump(player);
		}
		else {
			return false;
		}
		return true;
	}

	//绑定Xuid
	inline bool bindXuid(std::string name, std::string xuid) {
		YAML::Node player = YAML::LoadFile("data/player.yml");
		for (YAML::Node i : player) {
			if (i["playerName"].as<std::string>() == name) {
				std::ofstream fout("data/player.yml");
				i["xuid"] = xuid;
				fout << YAML::Dump(player);
				return true;
			}
		}
		return false;
	}
}

//正则组结构体
enum regularAction { Console, Group, Command };
enum regularFrom { group, console };
struct Regular {
	QString regular;
	std::string action;
	regularAction type;
	regularFrom from;
	bool permission;
};