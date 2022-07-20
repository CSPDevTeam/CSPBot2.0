// self
#include <helper.h>
#include <dialog.h>
//third-party
#include <QGraphicsDropShadowEffect>
#include <qevent.h>

using namespace std;

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
	//阴影设置
	vector<QWidget*> buttons = {
		dialogUi.pushButton,
		dialogUi.content,
	};
	for (auto bt : buttons) {
		setGraphics(bt);
	}
	//绑定事件
	connect(dialogUi.pushButton, &QPushButton::clicked, this, &QDialog::close);
}

CDialog::~CDialog() {
	
}

void CDialog::setGraphics(QWidget* bt) {
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(8);
	bt->setGraphicsEffect(shadow_effect);
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
		setGraphics(dialogUi.infoStatus);
		break;
	case warn:
		dialogUi.warnStatus->setHidden(false);
		setGraphics(dialogUi.warnStatus);
		break;
	case error:
		dialogUi.errorStatus->setHidden(false);
		setGraphics(dialogUi.errorStatus);
		break;
	default:
		dialogUi.content->setGeometry(QRect(10, 40, 361, 121));
		break;
	}
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//标题栏的长度
constexpr auto pos_min_x = 0;
constexpr auto pos_max_x = 381;
constexpr auto pos_min_y = 0;
constexpr auto pos_max_y = 30;
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
