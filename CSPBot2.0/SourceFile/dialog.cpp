// self
#include <helper.h>
#include <dialog.h>
//third-party
#include <QGraphicsDropShadowEffect>
#include <qevent.h>

CDialog::CDialog(diaLogStatus status, QWidget* parent){
	dialogUi.setupUi(this);
	sStatus = status; //存储状态
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
	shadow_effect->setBlurRadius(15);
	dialogUi.background->setGraphicsEffect(shadow_effect);
	changeIconStatus(); //改变图标
	QApplication::beep();
	//绑定事件
	connect(dialogUi.pushButton, &QPushButton::clicked, this, &QDialog::close);
}

CDialog::~CDialog() {
	
}

//设置标题
void CDialog::setTitle(QString title) {
	dialogUi.title->setText(title);
	this->setWindowTitle(title);
}

//设置内容
void CDialog::setContent(QString content) {
	dialogUi.content->setText(content);
}

//设置Icon状态
void CDialog::changeIconStatus() {
	dialogUi.infoStatus->setHidden(true);
	dialogUi.errorStatus->setHidden(true);
	dialogUi.warnStatus->setHidden(true);
	switch (sStatus) {
	case info:
		dialogUi.infoStatus->setHidden(false);
		break;
	case warn:
		dialogUi.warnStatus->setHidden(false);
		break;
	case error:
		dialogUi.errorStatus->setHidden(false);
		break;
	default:
		dialogUi.content->setGeometry(QRect(10, 40, 361, 121));
		break;
	}
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//标题栏的长度
const static int pos_min_x = 0;
const static int pos_max_x = 381;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//自己实现的窗口拖动操作
void CDialog::mousePressEvent(QMouseEvent* event) {
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
void CDialog::mouseMoveEvent(QMouseEvent* event) {
	if (isMousePressed == true) {
		QPoint movePot = event->globalPos() - mousePosition;
		move(movePot);
	}
}
void CDialog::mouseReleaseEvent(QMouseEvent* event) {
	isMousePressed = false;
}
