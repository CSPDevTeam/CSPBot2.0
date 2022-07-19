#include <error.h>
#include <global.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <cspbot20.h>

void ShowError(const QString& msg) {
	QMessageBox::critical(g_main_window, "严重错误", msg, QMessageBox::Yes);
}

void ShowHint(const QString& msg) {
	QMessageBox::information(g_main_window, "提示", msg, QMessageBox::Yes);
}
