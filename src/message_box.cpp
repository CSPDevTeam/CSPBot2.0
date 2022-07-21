#include "global.h"
#include <qstring.h>
#include "cspbot20.h"
#include "message_box.h"

//显示Error信息
void msgbox::ShowError(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Critical);
	msgBox.setText(msg);
	msgBox.exec();
}

//显示Information信息
void msgbox::ShowHint(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Information);
	msgBox.setText(msg);
	msgBox.exec();
	
}

//显示Warning信息
void msgbox::ShowWarn(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Warning);
	msgBox.setText(msg);
	msgBox.exec();
}

//显示Question信息
void msgbox::ShowQuestion(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Question);
	msgBox.setText(msg);
	msgBox.exec();
}
