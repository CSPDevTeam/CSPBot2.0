#pragma once
#include "framework.h"
#include "global.h"
#include <qstring.h>

namespace YAML {
class Node;
}

namespace helper {

//字符串替换
string replace(const string& strSrc, const string& oldStr, const string& newStr, size_t count = -1);

//切割字符串
vector<string> split(const string& str, const string& pattern);

//检测string是否是UTF-8
bool is_str_utf8(const char* str);

// GBK与UTF-8互转
string GbkToUtf8(const char* src_str);

string Utf8ToGbk(const char* src_str);

// Str与WStr互转
std::wstring str2wstr(const string& str);
std::wstring str2wstr(const std::string& str, UINT codePage);
string wstr2str(const std::wstring& wstr);
std::string wstr2str(const std::wstring& wstr, UINT codePage);

} // namespace helper

namespace Motd {
inline HMODULE h = LoadLibraryA("dllLibs/Motd.dll");
typedef char* (*funcPtrBE)(char* Host);
typedef char* (*funcPtrJE)(char* Host);

string motdbe(string host);

string motdje(string host);
} // namespace Motd

namespace Bind {
bool bind(string qq, string name);

bool unbind(string qq);

//绑定Xuid
bool bindXuid(string name, string xuid);

//查询信息
YAML::Node queryXboxID(string type, string arg);
} // namespace Bind

//正则组结构体
enum regularAction {
	Console,
	Group,
	Command
};
enum regularFrom {
	group,
	console
};
struct Regular {
	QString regular;
	QString action;
	regularAction type;
	regularFrom from;
	bool permission;
};