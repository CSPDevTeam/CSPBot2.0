#include "regular_edit.h"
#include "config_reader.h"
#include "message_box.h"
#include <QtWidgets>
#include <QGraphicsDropShadowEffect>
#include <fstream>

RegularEdit::RegularEdit(Regular regular, bool newRegular, QWidget* parent)
	: QWidget(parent) {
	regular_ui.setupUi(this);
	mRegular = regular;
	mNewRegular = newRegular;

	//////// Style ////////
	//窗口样式
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	//设置无边框
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	//设置窗口阴影
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(10);
	regular_ui.background->setGraphicsEffect(shadow_effect);

	setGraphics(regular_ui.save);
	setGraphics(regular_ui.cancel);
	setGraphics(regular_ui.regularEdit);
	setGraphics(regular_ui.remove);
	
	//分组
	groupButton1 = new QButtonGroup(this);
	groupButton1->addButton(regular_ui.console, 0);
	groupButton1->addButton(regular_ui.group, 1);
	// ui->apple_radioButton->setChecked(true);
	setRegular();
	if (mNewRegular) {
		regular_ui.remove->setEnabled(false);
	}

	///////// Bind //////////
	connect(regular_ui.regularTest, SIGNAL(clicked()), this, SLOT(stratProgress()));
	connect(regular_ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
	connect(regular_ui.cancel, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
	connect(regular_ui.save, SIGNAL(clicked()), this, SLOT(saveRegular()));
	connect(regular_ui.remove, SIGNAL(clicked()), this, SLOT(deleteRegular()));
}

void RegularEdit::saveRegular() {
	if (mNewRegular == false) {
		//转换成存储的Regular
		string fmRegular = mRegular.regular.toStdString();
		string fmAction = mRegular.action.toStdString();
		string fmFrom;
		bool fmPermission = mRegular.permission;

		//转换来源
		switch (mRegular.from) {
		case group:
			fmFrom = "group";
			break;
		case console:
			fmFrom = "console";
			break;
		}

		//获取原对象
		bool removed = false;
		for (int i = 0; i < g_regular.raw().size(); i++) {
			if (g_regular[i]["Regular"].as<string>() == fmRegular &&
				g_regular[i]["Action"].as<string>() == fmAction &&
				g_regular[i]["From"].as<string>() == fmFrom &&
				g_regular[i]["Permissions"].as<bool>() == fmPermission) {
				g_regular.raw().remove(i);
				removed = true;
				break;
			}
		}

		if (!removed) {
			msgbox::ShowWarn("无法正常删除原正则，请手动删除");
		}
	}

	//写入新对象
	//转换成存储的Regular
	string tmRegular = regular_ui.regularEdit->text().toStdString();
	string tmAction = regular_ui.actionEdit->text().toStdString();
	string tmFrom;
	bool tmPermission = regular_ui.checkBox->checkState();

	//转换来源
	switch (groupButton1->checkedId()) {
	case 0:
		tmFrom = "console";
		break;
	case 1:
		tmFrom = "group";
		break;
	}
	YAML::Node newRegular;
	newRegular["Regular"] = tmRegular;
	newRegular["Action"] = tmAction;
	newRegular["From"] = tmFrom;
	newRegular["Permissions"] = tmPermission;
	g_regular.raw().push_back(newRegular);

	//写入文件
	std::ofstream fout("data/regular.yml");
	fout << g_regular.raw();
	fout.close();

	this->close();
}

void RegularEdit::deleteRegular() {
	//转换成存储的Regular
	string fmRegular = mRegular.regular.toStdString();
	string fmAction = mRegular.action.toStdString();
	string fmFrom;
	bool fmPermission = mRegular.permission;

	//转换来源
	switch (mRegular.from) {
	case group:
		fmFrom = "group";
		break;
	case console:
		fmFrom = "console";
		break;
	}

	//获取原对象
	bool removed = false;
	for (int i = 0; i < g_regular.raw().size(); i++) {
		if (g_regular[i]["Regular"].as<string>() == fmRegular &&
			g_regular[i]["Action"].as<string>() == fmAction &&
			g_regular[i]["From"].as<string>() == fmFrom &&
			g_regular[i]["Permissions"].as<bool>() == fmPermission) {
			g_regular.raw().remove(i);
			removed = true;
			break;
		}
	}

	if (!removed) {
		msgbox::ShowWarn("无法正常删除原正则，请手动删除");
	}

	//写入文件
	std::ofstream fout("data/regular.yml");
	fout << g_regular.raw();
	fout.close();

	this->close();
}

void RegularEdit::setRegular() {
	//设置正则
	regular_ui.regularEdit->setText(mRegular.regular);
	regular_ui.actionEdit->setText(mRegular.action);
	switch (mRegular.from) {
	case console:
		regular_ui.console->setChecked(true);
		break;
	case group:
		regular_ui.group->setChecked(true);
		break;
	}
	//设置是否需要权限
	regular_ui.checkBox->setChecked(mRegular.permission);
}

void RegularEdit::stratProgress() {
	QProcess process(this);
	process.startDetached("regularTester.exe");
}

///////////////////////////////////////////// Style /////////////////////////////////////////////
void RegularEdit::setGraphics(QWidget* bt) {
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(8);
	bt->setGraphicsEffect(shadow_effect);
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//标题栏的长度
const static int pos_min_x = 0;
const static int pos_max_x = 500;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//自己实现的窗口拖动操作
void RegularEdit::mousePressEvent(QMouseEvent* event) {
	mousePosition = event->pos();
	//只对标题栏范围内的鼠标事件进行处理
	if (mousePosition.x() <= pos_min_x)
		return;
	if (mousePosition.x() >= pos_max_x)
		return;
	if (mousePosition.y() <= pos_min_y)
		return;
	if (mousePosition.y() >= pos_max_y)
		return;
	isMousePressed = true;
}
void RegularEdit::mouseMoveEvent(QMouseEvent* event) {
	if (isMousePressed == true) {
		QPoint movePot = event->globalPos() - mousePosition;
		move(movePot);
	}
}
void RegularEdit::mouseReleaseEvent(QMouseEvent* event) {
	isMousePressed = false;
}

void RegularEdit::on_actionClose_triggered() {
	close();
}