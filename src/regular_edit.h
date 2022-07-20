#pragma once
#include "helper.h"
#include <ui_regularEdit.h>

class RegularEdit : public QWidget {
	Q_OBJECT

public:
	RegularEdit(Regular regular, bool newRegular, QWidget* parent = Q_NULLPTR);

private slots:
	void on_actionClose_triggered(); //关闭窗口
	/////// Start Progress ////////
	void stratProgress();
	void saveRegular();
	void deleteRegular();

private:
	//////// Window ////////
	void mouseMoveEvent(QMouseEvent* event);
	// void setGraphics(QWidget* bt);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent*);
	QPoint mousePosition;
	bool isMousePressed;
	int startCount = 0;
	QButtonGroup* groupButton1;
	QButtonGroup* groupButton2;

	//////// Style /////////
	void setGraphics(QWidget* bt);

	//////// Setting ////////
	void setRegular();

	//存储
	Regular mRegular;
	bool mNewRegular;
	Ui::Regular regular_ui;
};