#include "global.h"
#include <qstring.h>
#include "cspbot20.h"
#include "dialog.h"
#include "message_box.h"

void msgbox::ShowError(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::error, g_main_window);
	//s_dialogWindow->setTitle("严重错误");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::critical(g_main_window, "严重错误", msg, QMessageBox::Yes);
}

void msgbox::ShowHint(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::info, g_main_window);
	//s_dialogWindow->setTitle("提示");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::information(g_main_window, "提示", msg, QMessageBox::Yes);
}

void msgbox::ShowWarn(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::warn, g_main_window);
	//s_dialogWindow->setTitle("警告");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::warning(g_main_window, "警告", msg, QMessageBox::Yes);
}
