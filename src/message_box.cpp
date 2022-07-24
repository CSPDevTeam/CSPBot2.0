#include "global.h"
#include "message_box.h"
#include "cspbot20.h"
#include <QString>

//显示Error信息
void msgbox::ShowError(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Critical);
	msgBox.setText(msg);
	msgBox.exec();
	msgBox.deleteLater();
}

//显示Information信息
void msgbox::ShowInfo(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Information);
	msgBox.setText(msg);
	msgBox.exec();
	msgBox.deleteLater();
}

//显示Warning信息
void msgbox::ShowWarn(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Warning);
	msgBox.setText(msg);
	msgBox.exec();
	msgBox.deleteLater();
}

//显示Question信息
void msgbox::ShowQuestion(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Question);
	msgBox.setText(msg);
	msgBox.exec();
	msgBox.deleteLater();
}
