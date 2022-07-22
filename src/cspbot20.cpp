#include "cspbot20.h"
#include "stdafx.h"
#include "helper.h"
#include "global.h"
#include "logger.h"
#include "ws_client.h"
#include "regular_edit.h"
#include "plugins.h"
#include <QInputDialog>
#include "Event.h"
#include "message_box.h"
#include "server.h"
#include "config_reader.h"

///////////////////////////////////////////// Export /////////////////////////////////////////////
//插入机器人日志
void CSPBot::insertLog(QString a) {
	ui.botconsole->setReadOnly(false);
	ui.botconsole->append(a);
	ui.botconsole->moveCursor(QTextCursor::End);
	ui.botconsole->setReadOnly(true);
};

//启动机器人日志
void CSPBot::startLogger() {
	//////// Logger ////////
	LoggerReader* loggerReader = new LoggerReader(this);
	connect(loggerReader, SIGNAL(updateLog(QString)), this, SLOT(insertLog(QString)));
	loggerReader->start();
}

//保存控制台日志
void CSPBot::slotSaveConsole() {
	if (ui.botconsole->toPlainText() == "") {
		msgbox::ShowInfo("控制台日志为空");
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存当前日志"), "", tr("日志文件(*.txt)"));
	if (fileName == "") {
		return;
	}
	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		msgbox::ShowError("日志文件保存失败!");
	} else {
		QTextStream stream(&file);
		stream << ui.botconsole->toPlainText();
		stream.flush();
		file.close();
	}
}

//清空控制台日志
void CSPBot::slotClearConsole() { ui.botconsole->setText(""); }

//更新上下行
void CSPBot::slotUpdateSendRecive(int send, int recive) {
	string sendString = std::to_string(send);
	string reciveString = std::to_string(recive);
	if (send > 99) {
		sendString = "99+";
	}
	if (recive > 99) {
		reciveString = "99+";
	}
	string reFormat = fmt::format("消息:{}发;{}收", sendString, reciveString);
	ui.websocketMsg->setText(QString::fromStdString(reFormat));
}

//更新连接时间
void CSPBot::slotConnected(mTime getTime) { mGetTime = getTime; }

//手动连接Mirai
void CSPBot::slotConnectMirai() {
	if (g_mirai->logined == false) {
		string formatLog = fmt::format("<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), "正在连接Mirai...");
		insertLog(QString::fromStdString(formatLog));
		g_mirai->connectMirai();
	} else {
		string formatLog = fmt::format("<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), "现在已处于已连接状态.");
		insertLog(QString::fromStdString(formatLog));
	}
}

//手动断开Mirai
void CSPBot::slotDisConnectMirai() {
	if (g_mirai->logined == false) {
		string formatLog = fmt::format("<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), "现在未处于已连接状态.");
		insertLog(QString::fromStdString(formatLog));
	} else {
		string formatLog = fmt::format("<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), "正在断开Mirai...");
		insertLog(QString::fromStdString(formatLog));
		g_wsc->shutdown();
	}
}

void CSPBot::slotMiraiMessageBox() {
	if (GetConfig("connectUrl") != "!failed!") {
		msgbox::ShowError("无法连接到Mirai");
	}
}

///////////////////////////////////////////// Timer /////////////////////////////////////////////
void CSPBot::slotTimerFunc() {
	//////// Connect ////////
	mTime nowTime = time(NULL);
	int min;
	string minString;
	bool flag = true;
	if (mGetTime == 0) {
		min = 0;
	} else {
		int f = nowTime - mGetTime;
		if (f < 60) {
			min = f % 60;
			flag = false;
		} else {
			min = f / 60;
		}
	}
	minString = std::to_string(min);
	if (flag) {
		if (min > 99) {
			minString = "99+m";
		} else {
			minString = minString + "m";
		}
	} else {
		minString = minString + "s";
	}
	// qDebug() << QString::fromStdString(minString);
	string minFormat = fmt::format("连接时间:{}", minString);
	ui.websocketConnectedTime->setText(QString::fromStdString(minFormat));

	//////// Mirai ////////
	if (g_mirai->logined) {
		ui.websocketStatus->setText("状态: 已连接");
	} else {
		ui.websocketStatus->setText("状态: 未连接");
	}

	/////// Table ////////
	InitPlayerTableView();
	InitRegularTableView();
	InitPluginTableView();
}

///////////////////////////////////////////// Main /////////////////////////////////////////////
CSPBot::CSPBot(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//////// Style ////////
	//窗口样式
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	//设置无边框
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	;
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QString("CSPBot v") + g_VERSION);
	//设置窗口阴影
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(10);
	ui.background->setGraphicsEffect(shadow_effect);

	//滚动条
	vector<QScrollBar*> bars = {ui.ServerLog->verticalScrollBar(), ui.botconsole->verticalScrollBar(), ui.playerAdmin->verticalScrollBar(), ui.regularAdmin->verticalScrollBar(), ui.pluginAdmin->verticalScrollBar()};
	for (QScrollBar* bar : bars) {
		setAllScrollbar(bar);
	}

	//阴影设置
	vector<QWidget*> buttons = {
		ui.ServerLog,
		ui.controlWidget,
		ui.statusWidget,
		ui.playerAdmin,
		ui.regularAdmin,
		ui.pluginAdmin,
		ui.inputCmd,
		ui.runCmd,
		ui.consoleWidget,
		ui.websocketWidget,
		ui.regularWidget,
	};
	for (auto bt : buttons) {
		setGraphics(bt);
	}

	//动画窗口
	c_pOpacity = new QGraphicsOpacityEffect(this);
	c_pAnimation = new QPropertyAnimation(this);

	//////// Basic ////////
	//绑定版本号到标签
	ui.version->setText(QString("V") + g_VERSION);

	//////// Bind ////////
	//注册并绑定
	qRegisterMetaType<StringMap>("StringMap");
	qRegisterMetaType<mTime>("mTime");
	qRegisterMetaType<StringVector>("StringVector");

	//翻页按钮
	connect(ui.mainPage, &QPushButton::clicked, this, &CSPBot::switchPage);
	connect(ui.playerPage, &QPushButton::clicked, this, &CSPBot::switchPage);
	connect(ui.regularPage, &QPushButton::clicked, this, &CSPBot::switchPage);
	connect(ui.pluginPage, &QPushButton::clicked, this, &CSPBot::switchPage);
	connect(ui.logPage, &QPushButton::clicked, this, &CSPBot::switchPage);

	//功能性按钮
	connect(ui.close, &QPushButton::clicked, this, &CSPBot::on_actionClose_triggered);
	connect(ui.min, &QPushButton::clicked, this, &CSPBot::on_actionMinimize_triggered);
	connect(ui.about, &QPushButton::clicked, this, &CSPBot::showAbout);

	//绑定事件
	connect(c_pAnimation, &QPropertyAnimation::finished, this, &CSPBot::close);
	connect(this, &CSPBot::signalStartServer, this, &CSPBot::startServer);

	// Server类按钮
	connect(ui.start, &QPushButton::clicked, this, &CSPBot::startServer);
	connect(ui.stop, &QPushButton::clicked, this, &CSPBot::stopServer);
	connect(ui.forceStop, &QPushButton::clicked, this, &CSPBot::forceStopServer);
	connect(ui.clear, &QPushButton::clicked, this, &CSPBot::clear_console);
	connect(ui.ServerCmd, &QPushButton::clicked, this, &CSPBot::startCmd); //绑定启动cmd
	connect(ui.runCmd, &QPushButton::clicked, this, &CSPBot::insertCmd); //绑定运行命令
	connect(this, &CSPBot::signalStartLogger, this, &CSPBot::startLogger); //开启Logger服务

	//绑定快捷键
	connect(this, &CSPBot::runCommand, this, &CSPBot::insertCmd); //绑定回车输入命令
	connect(this, &CSPBot::runCmd, ui.ServerCmd, &QPushButton::click); //绑定启动cmd

	//机器人Console
	connect(ui.consoleSave, &QPushButton::clicked, this, &CSPBot::slotSaveConsole); //保存日志
	connect(ui.consoleClear, &QPushButton::clicked, this, &CSPBot::slotClearConsole); //清空控制台
	connect(ui.connect, &QPushButton::clicked, this, &CSPBot::slotConnectMirai); //连接Mirai
	connect(ui.disConnect, &QPushButton::clicked, this, &CSPBot::slotDisConnectMirai); //断开连接

	//表格
	connect(ui.regularAdmin, SIGNAL(pressed(QModelIndex)), this, SLOT(clickRegularTable(QModelIndex)));
	connect(ui.regularAdmin, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickedRegularTable(QModelIndex)));
	connect(ui.regularNew, &QPushButton::clicked, this, &CSPBot::newRegular); //新建正则按钮

	//////// Debug ////////
	connect(this, &CSPBot::signalDebug, this, &CSPBot::slotDebug);

	//////// Mirai ////////
	g_mirai = new Mirai();
	g_mirai->start();
	connect(g_mirai, SIGNAL(setUserImages(QString, QString)), this, SLOT(setUserImage(QString, QString)));
	connect(g_mirai, SIGNAL(updateSendRecive(int, int)), this, SLOT(slotUpdateSendRecive(int, int)));
	connect(g_mirai, SIGNAL(signalConnect(mTime)), this, SLOT(slotConnected(mTime)));
	connect(g_mirai, SIGNAL(OtherCallback(QString, StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
	connect(g_mirai, SIGNAL(signalMiraiMessageBox()), this, SLOT(slotMiraiMessageBox()));
	connect(g_mirai, SIGNAL(sendServerCommand(QString)), this, SLOT(slotSendCommand(QString)));
	connect(g_mirai, SIGNAL(packetCallback(QString)), this, SLOT(slotPacketCallback(QString)));

	/////// Other /////////
	ui.inputCmd->setEnabled(false);
	ui.runCmd->setEnabled(false);
	ui.stop->setEnabled(false);
	ui.forceStop->setEnabled(false);
	ui.ServerLog->setReadOnly(true);
	ui.botconsole->setReadOnly(true);
	g_cmd_api = new CommandAPI();
	connect(g_cmd_api, &CommandAPI::signalStartServer, this, &CSPBot::startServer);
	connect(g_cmd_api, SIGNAL(signalCommandCallback(QString, StringVector)), this, SLOT(slotCommandCallback(QString, StringVector)));
	connect(g_cmd_api, SIGNAL(Callback(QString, StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
	/////// timer /////////
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerFunc()));
	timer->start(1 * 1000);

	/////// Table /////////
	InitPlayerTableView();
	InitRegularTableView();
	InitPluginTableView();
}

///////////////////////////////////////////// Style /////////////////////////////////////////////
void CSPBot::setGraphics(QWidget* bt) {
	QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(0, 0);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(8);
	bt->setGraphicsEffect(shadow_effect);
}

void CSPBot::setAllScrollbar(QScrollBar* bar) {
	bar->setStyleSheet("QScrollBar:vertical"
					   "{"
					   "width:8px;"
					   "background:rgba(0,0,0,0%);"
					   "margin:0px,0px,0px,0px;"
					   "padding-top:9px;"
					   "padding-bottom:9px;"
					   "}"
					   "QScrollBar::handle:vertical"
					   "{"
					   "width:8px;"
					   "background:rgba(0,0,0,25%);"
					   " border-radius:4px;"
					   "min-height:20;"
					   "}"
					   "QScrollBar::handle:vertical:hover"
					   "{"
					   "width:8px;"
					   "background:rgba(0,0,0,50%);"
					   " border-radius:4px;"
					   "min-height:20;"
					   "}"
					   "QScrollBar::add-line:vertical"
					   "{"
					   "height:9px;width:8px;"
					   "border-image:url(:/a/Images/a/3.png);"
					   "subcontrol-position:bottom;"
					   "}"
					   "QScrollBar::sub-line:vertical"
					   "{"
					   "height:9px;width:8px;"
					   "border-image:url(:/a/image/a/1.png);"
					   "subcontrol-position:top;"
					   "}"
					   "QScrollBar::add-line:vertical:hover"
					   "{"
					   "height:9px;width:8px;"
					   "border-image:url(:/a/image/a/4.png);"
					   "subcontrol-position:bottom;"
					   "}"
					   "QScrollBar::sub-line:vertical:hover"
					   "{"
					   "height:9px;width:8px;"
					   "border-image:url(:/a/image/a/2.png);"
					   "subcontrol-position:top;"
					   "}"
					   "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
					   "{"
					   "background:rgba(0,0,0,10%);"
					   "border-radius:4px;"
					   "}");
}

QPixmap PixmapToRound(QPixmap& src, int radius);

void CSPBot::setUserImageError(QNetworkReply::NetworkError e) {
	auto error = magic_enum::enum_name<QNetworkReply::NetworkError>(e);
	Logger("Mirai").error("{}", error);
}

void CSPBot::setUserImage(QString qqNum, QString qqNick) {
	if (qqNum == "" || qqNick == "") {
		QPixmap pixmap = QPixmap();
		ui.userImage->setPixmap(pixmap);
		ui.user->setText("用户未登录");
		ui.userImage->setText("用户未登录");
		return;
	}
	QString szUrl = "https://q.qlogo.cn/g?b=qq&nk=" + qqNum + "&s=640";
	QUrl url(szUrl);
	QNetworkAccessManager manager;
	QEventLoop loop;
	qDebug() << "Reading picture form " << url;
	QNetworkReply* reply = manager.get(QNetworkRequest(url));
	//请求结束并下载完成后，退出子事件循环
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(setUserImageError(QNetworkReply::NetworkError)));
	//开启子事件循环
	loop.exec();
	QByteArray jpegData = reply->readAll();
	QPixmap pixmap;
	pixmap.loadFromData(jpegData);
	if (!pixmap.isNull()) {
		ui.userImage->setPixmap(PixmapToRound(pixmap, 45)); // 你在QLabel显示图片
	} else {
		ui.userImage->setText("图片加载错误");
	}

	ui.user->setText(qqNick);
}

QPixmap PixmapToRound(QPixmap& src, int radius) {
	if (src.isNull()) {
		return QPixmap();
	}

	QSize size(src.width(), src.height());
	QSize size2(radius * 2, radius * 2);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHints(QPainter::SmoothPixmapTransform); //消锯齿
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setRenderHints(QPainter::TextAntialiasing);
	painter.translate(0, 0);
	// painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawEllipse(0, 0, size.width(), size.height());
	src.setMask(mask);
	QPixmap image = src.scaled(size2);
	return image;
}

///////////////////////////////////////////// Basic /////////////////////////////////////////////
void CSPBot::switchPage() {
	QPushButton* button = qobject_cast<QPushButton*>(sender()); //得到按下的按钮的指针
	QPushButton* btns[5] = {ui.mainPage, ui.playerPage, ui.pluginPage, ui.regularPage, ui.logPage};
	for (auto i : btns) {
		i->setStyleSheet("QPushButton{background-color:#f0f3f8;color:#666666;border-radius:10px;qproperty-iconSize: 32px 32px;}QPushButton:hover{background-color:#e0e6ee;border-radius:10px;};");
	}
	if (button == ui.mainPage) {
		ui.main->setCurrentIndex(0);
	} else if (button == ui.playerPage) {
		ui.main->setCurrentIndex(1);
	} else if (button == ui.regularPage) {
		ui.main->setCurrentIndex(2);
	} else if (button == ui.pluginPage) {
		ui.main->setCurrentIndex(3);
	} else if (button == ui.logPage) {
		ui.main->setCurrentIndex(4);
	}
	button->setStyleSheet("background-color:#ccdff8;\ncolor:#666666;\nborder-radius:10px;\nqproperty-iconSize: 32px 32px;\n");
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//标题栏的长度
const static int pos_min_x = 0;
const static int pos_max_x = 1010;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//自己实现的窗口拖动操作
void CSPBot::mousePressEvent(QMouseEvent* event) {
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
void CSPBot::mouseMoveEvent(QMouseEvent* event) {
	if (isMousePressed == true) {
		QPoint movePot = event->globalPos() - mousePosition;
		move(movePot);
	}
}
void CSPBot::mouseReleaseEvent(QMouseEvent* event) { isMousePressed = false; }

void CSPBot::on_actionMinimize_triggered() {
	showMinimized();
	//最大化 showMaximized()；
}
void CSPBot::on_actionClose_triggered() {
	if (checkClose()) {
		c_pAnimation->setTargetObject(this);
		c_pOpacity->setOpacity(0);
		setGraphicsEffect(c_pOpacity);
		c_pAnimation->setTargetObject(c_pOpacity);
		c_pAnimation->setPropertyName("opacity");
		c_pAnimation->setStartValue(1);
		c_pAnimation->setEndValue(0);
		c_pAnimation->start();
	}
}

bool CSPBot::checkClose() {
	if (g_server != nullptr && g_server->getStarted()) {
		auto temp = QMessageBox::warning(this, "警告", "服务器还在运行，你是否要关闭?", QMessageBox::Yes | QMessageBox::No);
		if (temp == QMessageBox::Yes) {
			g_server->forceStopServer();
			// Callbacker cbe(EventCode::onStop);
			// if (cbe.callback()) {
			//	return true;
			// }
			// else {
			//	return false;
			// }
		} else {
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////// Server /////////////////////////////////////////////
//构造Server
void CSPBot::buildServer(int mode) {
	g_server = new Server(mode, this);
	//绑定检测器
	connect(g_server, SIGNAL(insertBDSLog(QString)), this, SLOT(slotInsertBDSLog(QString)));
	connect(g_server, SIGNAL(OtherCallback(QString, StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
	connect(g_server, SIGNAL(chenableForce(bool)), this, SLOT(slotChenableForce(bool)));
	connect(g_server, SIGNAL(chLabel(QString, QString)), this, SLOT(slotChLabel(QString, QString)));
	connect(g_server, SIGNAL(changeStatus(bool)), this, SLOT(slotChangeStatus(bool)));
	//启动Server
	g_server->run();
}

//插入BDS日志
void CSPBot::slotInsertBDSLog(QString log) {
	ui.ServerLog->setReadOnly(false);
	ui.ServerLog->append(log);
	ui.ServerLog->moveCursor(QTextCursor::End);
	ui.ServerLog->setReadOnly(true);
};

// Callback
bool CSPBot::slotOtherCallback(QString listener, StringMap args) {
	// Python Callbacker
	qDebug() << "CallBack:" << listener;
	string eventName = listener.toStdString();
	auto event_code = magic_enum::enum_cast<EventCode>(eventName);
	if (!event_code) {
		return false;
	}
	EventCode ct = event_code.value();
	// Callbacker cb(ct);
	// for (auto& i : args) {
	//	string key = i.first;
	//	string value = i.second;
	//	if (!helper::is_str_utf8(value.c_str())) {
	//		return false;
	//	}
	//	cb.insert(key.c_str(), value);
	// }
	// bool ret = cb.callback();
	bool ret = 0;
	// Event Callbacker
	EventCallbacker ecb(ct);
	std::vector<string> args_;
	for (auto& i : args) {
		ecb.insert(i.second);
	}
	ecb.callback();
	return ret;
};

void CSPBot::slotCommandCallback(QString cmd, StringVector fArgs) {
	// string type = cmd.toStdString();
	// if (g_command.find(type) != g_command.end()) {
	//	luabridge::LuaRef args = luabridge::newTable(g_lua_State);
	//	for (auto& i : fArgs) {
	//		args.append(i.c_str());
	//	}
	//	// command[type](args);
	// }
}

void CSPBot::slotPacketCallback(QString msg) {
	string msgJson = msg.toStdString();
	//转换为dict
	// Callbacker packetcbe(EventCode::onReceivePacket);
	//// TODO: convert to table
	// packetcbe.insert("msg", msgJson);
	// bool pakctecb = packetcbe.callback();
}

//更改状态
void CSPBot::slotChangeStatus(bool a) {
	if (a) {
		ui.ServerStatus->setText("状态: 已启动");
	} else {
		ui.ServerStatus->setText("状态: 未启动");
	}
};

//更改标签
void CSPBot::slotChLabel(QString title, QString content) {
	if (title == "world") {
		ui.ServerWorld->setText("世界:" + content);
	} else if (title == "g_VERSION") {
		ui.ServerVersion->setText("版本:" + content);
	} else if (title == "difficult") {
		ui.ServerDifficult->setText("难度:" + content);
	}
};

//更改强制停止状态
void CSPBot::slotChenableForce(bool a) {
	ui.forceStop->setEnabled(a);
	if (a) {
		/*ui.change->setText("停止");*/
		ui.stop->setEnabled(true);
		ui.start->setEnabled(false);
		ui.inputCmd->setEnabled(true);
		ui.runCmd->setEnabled(true);
	} else {
		/*ui.change->setText("启动");*/
		ui.stop->setEnabled(false);
		ui.start->setEnabled(true);
		ui.inputCmd->setEnabled(false);
		ui.runCmd->setEnabled(false);
	}
};

//开启服务器
void CSPBot::startServer() {
	buildServer(0);
	ui.stop->setEnabled(true);
	slotInsertBDSLog("[CSPBot] 已向进程发出启动命令");
	ui.start->setEnabled(false);
}

//开启Cmd
void CSPBot::startCmd() {
	buildServer(1);
	ui.stop->setEnabled(true);
	slotInsertBDSLog("<font style=\"color:#FFCC66;\">!!![WARNNING] [CSPBot] 您已进入<font color=\"#008000\">CMD</font>调试模式. [WARNNING]!!!</font>");
	slotInsertBDSLog("[CSPBot] 已向进程发出启动命令");
	ui.start->setEnabled(false);
}

//关闭服务器
void CSPBot::stopServer() {
	g_server->stopServer();
	slotInsertBDSLog("[CSPBot] 已向进程发出终止命令");
	// ui.stop->setEnabled(false);
	// ui.start->setEnabled(true);
}

//强制停止进程
void CSPBot::forceStopServer() {
	auto temp = QMessageBox::warning(this, "警告", "服务器还在运行，你是否要强制停止?", QMessageBox::Yes | QMessageBox::No);
	if (temp == QMessageBox::Yes) {
		g_server->forceStopServer();
		ui.ServerLog->append("[CSPBot] 已向进程发出强制终止命令");
	}
}

//清空日志
void CSPBot::clear_console() { ui.ServerLog->setText(""); }

//插入命令
void CSPBot::insertCmd() {
	try {
		string cmd = ui.inputCmd->text().toStdString();
		if (cmd != "") {
			ui.inputCmd->setText("");
			g_server->sendCmd(cmd + "\n");
		}
	} catch (...) {
	}
}

void CSPBot::slotSendCommand(QString cmd) { g_server->sendCmd(cmd.toStdString()); }

///////////////////////////////////////////// Debug /////////////////////////////////////////////
void CSPBot::slotDebug() {
	bool bRet = false;
	QString text = QInputDialog::getText(this, "CSPBot Debug", "Debug调试器用户请退出.", QLineEdit::Normal, "", &bRet);
	if (!bRet || text.isEmpty()) {
		return;
	}

	//判断需要什么功能
	if (text == "crash") {
		int* x = 0;
		*x = 1;
	} else if (text == "cmd") {
		emit runCommand();
	}
}

///////////////////////////////////////////// Keyboard /////////////////////////////////////////////
void CSPBot::keyPressEvent(QKeyEvent* e) {
	if (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && e->key() == Qt::Key_C) {
		emit runCmd();
	} else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
		emit runCommand();
	}
	// DEBUG调试器
	else if (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && e->key() == Qt::Key_D) {
		emit signalDebug();
	}
}

///////////////////////////////////////////// Table /////////////////////////////////////////////
void CSPBot::InitPlayerTableView() {
	try {
		int line_num = static_cast<int>(g_player.raw().size());
		QStringList strHeader;
		strHeader << "玩家名称"
				  << "玩家Xuid"
				  << "玩家QQ号";

		QStandardItemModel* m_model = new QStandardItemModel();
		m_model->setHorizontalHeaderLabels(strHeader);
		m_model->setColumnCount(strHeader.size());
		m_model->setRowCount(line_num);
		ui.playerAdmin->verticalHeader()->hide();
		ui.playerAdmin->setModel(m_model);

		//居中显示并设置文本
		int in = 0;
		for (YAML::Node i : g_player.raw()) {
			string playerName = i["playerName"].as<string>();
			string xuid = i["xuid"].as<string>();
			string qq = i["qq"].as<string>();
			QStandardItem* item1 = new QStandardItem(QString::fromStdString(playerName));
			item1->setTextAlignment(Qt::AlignCenter);
			QStandardItem* item2 = new QStandardItem(QString::fromStdString(xuid));
			item2->setTextAlignment(Qt::AlignCenter);
			QStandardItem* item3 = new QStandardItem(QString::fromStdString(qq));
			item3->setTextAlignment(Qt::AlignCenter);
			m_model->setItem(in, 0, item1);
			m_model->setItem(in, 1, item2);
			m_model->setItem(in, 2, item3);
			in++;
		}
		ui.playerAdmin->horizontalHeader()->setStretchLastSection(true);
		ui.playerAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		ui.playerAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
		// ui.playerAdmin->setFrameShape(QListWidget::NoFrame);
		ui.playerAdmin->setAlternatingRowColors(true);
		ui.playerAdmin->setShowGrid(false);
	} catch (...) {
	}
}

void CSPBot::InitRegularTableView() {
	try {
		int line_num = static_cast<int>(g_regular.raw().size());
		QStringList strHeader;
		strHeader << "正则"
				  << "来源"
				  << "执行"
				  << "权限";

		QStandardItemModel* m_model = new QStandardItemModel();
		//添加表头数据
		m_model->setHorizontalHeaderLabels(strHeader);
		//设置列数
		m_model->setColumnCount(strHeader.size());
		//设置行数
		m_model->setRowCount(line_num);

		//隐藏列表头
		ui.regularAdmin->verticalHeader()->hide();

		// setModel.
		ui.regularAdmin->setModel(m_model);
		//居中显示并设置文本
		int in = 0;
		for (YAML::Node i : g_regular.raw()) {
			string Regular = i["Regular"].as<string>();
			string Action = i["Action"].as<string>();
			string From = i["From"].as<string>();
			string Permissions;
			if (i["Permissions"].as<bool>()) {
				Permissions = "是";
			} else if (!i["Permissions"].as<bool>()) {
				Permissions = "否";
			}
			QStandardItem* item1 = new QStandardItem(QString::fromStdString(Regular));
			item1->setTextAlignment(Qt::AlignCenter);
			QStandardItem* item2 = new QStandardItem(QString::fromStdString(From));
			item2->setTextAlignment(Qt::AlignCenter);
			QStandardItem* item3 = new QStandardItem(QString::fromStdString(Action));
			item3->setTextAlignment(Qt::AlignCenter);
			QStandardItem* item4 = new QStandardItem(QString::fromStdString(Permissions));
			item4->setTextAlignment(Qt::AlignCenter);
			m_model->setItem(in, 0, item1);
			m_model->setItem(in, 1, item2);
			m_model->setItem(in, 2, item3);
			m_model->setItem(in, 3, item4);
			in++;
		}
		ui.regularAdmin->horizontalHeader()->setStretchLastSection(true);
		ui.regularAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		ui.regularAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
		// ui.playerAdmin->setFrameShape(QListWidget::NoFrame);
		ui.regularAdmin->setAlternatingRowColors(true);
		ui.regularAdmin->setShowGrid(false);
	} catch (...) {
	}
}

//选中自动选中该行
void CSPBot::clickRegularTable(QModelIndex index) { ui.regularAdmin->selectRow(index.row()); }

void CSPBot::doubleClickedRegularTable(QModelIndex index) {
	int row = ui.regularAdmin->currentIndex().row();
	auto modelViewOneUp = ui.regularAdmin->model();
	vector<string> regularData;
	/*
	enum regularAction { Console, Group, Command };
	enum regularFrom { group, console };
	struct Regular {
		QString regular;
		std::string action;
		regularAction type;
		regularFrom from;
		bool permission;
	};
	strHeader << "正则" << "来源" << "执行" << "权限";
	*/
	for (int i = 0; i < 5; i++) {
		QModelIndex index = modelViewOneUp->index(row, i);
		QString name = modelViewOneUp->data(index).toString();
		regularData.push_back(name.toStdString());
	}
	string mRegular = regularData[0];
	string mFrom = regularData[1];
	string mAction = regularData[2];
	string mPermission = regularData[3];

	//转换type
	string Action_type = mAction.substr(0, 2);
	regularAction regular_action;
	if (Action_type == "<<") {
		regular_action = regularAction::Console;
	} else if (Action_type == ">>") {
		regular_action = regularAction::Group;
	} else {
		regular_action = regularAction::Command;
	};

	//转换权限
	bool Permission = (mPermission == "是");

	//转换来源
	regularFrom regular_from;
	transform(mFrom.begin(), mFrom.end(), mFrom.begin(), ::tolower);
	if (mFrom == "group") {
		regular_from = regularFrom::group;
	} else {
		regular_from = regularFrom::console;
	};

	Regular regular = {QString::fromStdString(regularData[0]), QString::fromStdString(regularData[2]), regular_action, regular_from, Permission};
	RegularEdit* regEdit = new RegularEdit(regular, false, this);
	regEdit->show();
}

//新建正则
void CSPBot::newRegular() {
	Regular regular = {"", "", Console, console, false};
	RegularEdit* regEdit = new RegularEdit(regular, true, this);
	regEdit->show();
}

void CSPBot::InitPluginTableView() {
	try {
		int line_num = static_cast<int>(g_plugins.size());
		QStringList strHeader;
		strHeader << "插件"
				  << "介绍"
				  << "版本"
				  << "作者";

		QStandardItemModel* m_model = new QStandardItemModel();
		m_model->setHorizontalHeaderLabels(strHeader);
		m_model->setColumnCount(strHeader.size());
		m_model->setRowCount(line_num);
		ui.pluginAdmin->verticalHeader()->hide();
		ui.pluginAdmin->setModel(m_model);
		int in = 0;
		for (auto& x : g_plugins) {
			QStandardItem* item1 = new QStandardItem(QString::fromStdString(x.name));
			QStandardItem* item2 = new QStandardItem(QString::fromStdString(x.description));
			QStandardItem* item3 = new QStandardItem(QString::fromStdString(x.version));
			QStandardItem* item4 = new QStandardItem(QString::fromStdString(x.author));

			//居中文本
			item1->setTextAlignment(Qt::AlignCenter);
			item2->setTextAlignment(Qt::AlignCenter);
			item3->setTextAlignment(Qt::AlignCenter);
			item4->setTextAlignment(Qt::AlignCenter);

			m_model->setItem(in, 0, item1);
			m_model->setItem(in, 1, item2);
			m_model->setItem(in, 2, item3);
			m_model->setItem(in, 3, item4);
			in++;
		}
		ui.pluginAdmin->horizontalHeader()->setStretchLastSection(true);
		ui.pluginAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		ui.pluginAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
		ui.pluginAdmin->setAlternatingRowColors(true);
		ui.pluginAdmin->setShowGrid(false);
	} catch (...) {
	}
}

void CSPBot::showAbout() {
	string text = fmt::format("CSPbot2.0 由CSPDev开发\n版本号:{}\n本程序遵守GPL v3.0许可证，未经许可禁止倒卖，复制", g_VERSION);
	QApplication::beep();
	QMessageBox::about(this, "关于CSPbot v2", QString::fromStdString(text));
}