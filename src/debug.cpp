#include <framework.h>
#include <DbgHelp.h>
#include <QApplication>
#include <QtWidgets/qmessagebox.h>
#include <FMT/chrono.h>
#include <logger.h>
#include <version.h>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

//获取格式化时间
string getTime() {
	time_t tt = time(NULL);
	struct tm* t = localtime(&tt);
	ostringstream buffer;
	string s = to_string(t->tm_sec);
	if (s.length() == 1) {
		s = "0" + s;
	}
	string timeString = fmt::format("{}-{}-{}_{}-{}-{}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, s);
	return timeString;
};

wchar_t* multiByteToWideChar(const string& pKey) {
	auto pCStrKey = pKey.c_str();
	//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, static_cast<int>(strlen(pCStrKey)) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	//第二次调用将单字节字符串转换成双字节字符串
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, static_cast<int>(strlen(pCStrKey)) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

// 保存程序异常崩溃的信息
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {
	if (!fs::exists("logs\\"))
		fs::create_directory("logs\\");
	//创建 Dump 文件
	string fileName = "logs/{}_CrashDump_{}.dmp";
	fileName = fmt::format(fileName, g_VERSION, getTime());
	wchar_t* wc = multiByteToWideChar(fileName);
	HANDLE hDumpFile = CreateFile(wc, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE) {
		// Dump 信息
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo{};
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;

		// 写入 dump 文件内容
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	}

	//弹出一个错误对话框
	QMessageBox msgBox;
	string text = "CSPBot出现严重错误，正在退出\n具体请查阅{}文件";
	text = fmt::format(text, fileName);
	msgBox.setWindowTitle("严重错误");
	msgBox.setText(QString::fromStdString(text));
	msgBox.exec();

	return EXCEPTION_EXECUTE_HANDLER;
}