#include "helper.h"
#include "config_reader.h"
#include <fstream>

//字符串替换
string helper::replace(const string& strSrc, const string& oldStr, const string& newStr, size_t count) {
	string strRet = strSrc;
	size_t pos = 0;
	int l_count = 0;
	if (-1 == count) // replace all
		count = strRet.size();
	while ((pos = strRet.find(oldStr, pos)) != string::npos) {
		strRet.replace(pos, oldStr.size(), newStr);
		if (++l_count >= count)
			break;
		pos += newStr.size();
	}
	return strRet;
}

//切割字符串
vector<string> helper::split(const string& str, const string& pattern) {
	vector<string> res;
	if (str == "")
		return res;
	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos) {
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(pattern);
	}

	return res;
}

//检测string是否是UTF-8
bool helper::is_str_utf8(const char* str) {
	unsigned int nBytes = 0;
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				}
				else if (chr >= 0xF8) {
					nBytes = 5;
				}
				else if (chr >= 0xF0) {
					nBytes = 4;
				}
				else if (chr >= 0xE0) {
					nBytes = 3;
				}
				else if (chr >= 0xC0) {
					nBytes = 2;
				}
				else {
					return false;
				}
				nBytes--;
			}
		}
		else {
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			nBytes--;
		}
	}
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) {
		return true;
	}
	return true;
}

// GBK与UTF-8互转
string helper::GbkToUtf8(const char* src_str) {
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, static_cast<size_t>(len) + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, static_cast<size_t>(len) + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr)
		delete[] wstr;
	if (str)
		delete[] str;
	return strTemp;
}

string helper::Utf8ToGbk(const char* src_str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, static_cast<size_t>(len) * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, static_cast<size_t>(len) + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK)
		delete[] wszGBK;
	if (szGBK)
		delete[] szGBK;
	return strTemp;
}

string Motd::motdbe(string host) {
	auto func = (funcPtrBE)GetProcAddress(h, "MotdBEJson");
	if (NULL == h || INVALID_HANDLE_VALUE == h) {
		return "{\"status\":\"error\"}";
	}
	char* st1 = const_cast<char*>(host.c_str());
	return func(st1);
}

string Motd::motdje(string host) {
	auto func = (funcPtrJE)GetProcAddress(h, "MotdJEJson");
	if (NULL == h || INVALID_HANDLE_VALUE == h) {
		return "{\"status\":\"error\"}";
	}
	char* st1 = const_cast<char*>(host.c_str());
	return func(st1);
}

bool Bind::bind(string qq, string name) {
	vector<string> player_list;
	vector<string> qq_list;
	for (YAML::Node i : g_player.raw()) {
		player_list.push_back(i["playerName"].as<string>());
		qq_list.push_back(i["qq"].as<string>());
	}

	//检测有没有已绑定的账号
	if (find(player_list.begin(), player_list.end(), name) == player_list.end() &&
		find(qq_list.begin(), qq_list.end(), qq) == qq_list.end()) {
		std::ofstream fout("data/player.yml");
		YAML::Node pl;
		pl["playerName"] = name;
		pl["qq"] = qq;
		pl["xuid"] = "";
		g_player.raw().push_back(pl);
		fout << g_player.raw();
	}
	else {
		return false;
	}
	return true;
}

bool Bind::unbind(string qq) {
	vector<string> qq_list;
	for (YAML::Node i : g_player.raw()) {
		qq_list.push_back(i["qq"].as<string>());
	}

	//检测有没有已绑定的账号
	if (find(qq_list.begin(), qq_list.end(), qq) != qq_list.end()) {
		std::ofstream fout("data/player.yml");
		auto it = find(qq_list.begin(), qq_list.end(), qq);
		if (it != qq_list.end()) {
			int index = it - qq_list.begin();
			g_player.raw().remove(index);
		}
		fout << g_player.raw();
	}
	else {
		return false;
	}
	return true;
}

//绑定Xuid
bool Bind::bindXuid(string name, string xuid) {
	for (auto i : g_player.raw()) {
		if (i["playerName"].as<string>() == name) {
			std::ofstream fout("data/player.yml");
			i["xuid"] = xuid;
			fout << g_player.raw();
			return true;
		}
	}
	return false;
}

//查询信息
YAML::Node Bind::queryXboxID(string type, string arg) {
	//迭代搜索
	for (auto i : g_player.raw()) {
		if (type == "qq") {
			if (i["qq"].as<string>() == arg) {
				return i;
			}
		}
		else if (type == "player") {
			if (i["playerName"].as<string>() == arg) {
				return i;
			}
		}
		else if (type == "xuid") {
			if (i["xuid"].as<string>() == arg) {
				return i;
			}
		}
	}
	return {};
}
