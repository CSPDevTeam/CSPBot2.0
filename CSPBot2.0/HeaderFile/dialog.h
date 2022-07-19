#pragma once
// third-party
#include <ui_dialog.h>

//枚举状态
enum diaLogStatus {
	info,
	warn,
	error,
	null
};

class CDialog : public QDialog {
	Q_OBJECT
public:
	CDialog(diaLogStatus status,QWidget* parent = Q_NULLPTR);
	~CDialog();
	
	void setTitle(QString title); //设置标题
	void setContent(QString content); //设置内容

private:
	void changeIconStatus(); //设置图标状态
	diaLogStatus sStatus; //图标状态
	Ui::Dialog dialogUi; //UI指针
};