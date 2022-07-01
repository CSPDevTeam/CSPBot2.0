#include "cspbot20.h"
#include "stdafx.h"
#include "helper.h"
#include "global.h"
#include "logger.h"
#include "websocketClient.h"
#include "regularEdit.h"

using namespace std;

///////////////////////////////////////////// Global /////////////////////////////////////////////
//关闭动画Animation
QGraphicsOpacityEffect* c_pOpacity;
QPropertyAnimation* c_pAnimation;

///////////////////////////////////////////// Export /////////////////////////////////////////////
//插入机器人日志
void CSPBot::insertLog(QString a) {;
    ui.botconsole->setReadOnly(false);
    ui.botconsole->append(a);
    ui.botconsole->setReadOnly(true);
};

bool connectMirai();

///////////////////////////////////////////// Main /////////////////////////////////////////////
CSPBot::CSPBot(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
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
    ui.background->setGraphicsEffect(shadow_effect);

    //滚动条
    vector<QScrollBar*> bars = { ui.ServerLog->verticalScrollBar(),ui.botconsole->verticalScrollBar() };
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
        ui.runCmd
    };
    for (auto bt : buttons) {
        setGraphics(bt);
    }

    //动画窗口
    c_pOpacity = new QGraphicsOpacityEffect(this);
    c_pAnimation = new QPropertyAnimation(this);

    //////// Basic ////////
    //绑定版本号到标签
    ui.version->setText("V" + Helper::stdString2QString(version));

    //////// Bind ////////
    //注册并绑定
    qRegisterMetaType<StringMap>("StringMap");
    //翻页按钮
    connect(ui.mainPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.playerPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.regularPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.pluginPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.logPage, SIGNAL(clicked()), this, SLOT(switchPage()));

    //功能性按钮
    connect(ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(ui.min, SIGNAL(clicked()), this, SLOT(on_actionMinimize_triggered()));

    //绑定事件
    connect(c_pAnimation, &QPropertyAnimation::finished, this, &CSPBot::close);
    connect(this,SIGNAL(signalStartServer()), this, SLOT(startServer()));
    
    //Server类按钮
    connect(ui.start, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(ui.stop, SIGNAL(clicked()), this, SLOT(stopServer()));
    connect(ui.forceStop, SIGNAL(clicked()), this, SLOT(forceStopServer()));
    connect(ui.clear, SIGNAL(clicked()), this, SLOT(clear_console()));
    connect(ui.ServerCmd, SIGNAL(clicked()), this, SLOT(startCmd())); //绑定启动cmd
    connect(ui.runCmd, SIGNAL(clicked()), this, SLOT(insertCmd())); //绑定运行命令

    //绑定快捷键
    connect(this, SIGNAL(runCommand()), this, SLOT(insertCmd())); //绑定回车输入命令
    connect(this, SIGNAL(runCmd()), ui.ServerCmd, SLOT(click())); //绑定启动cmd


    //////// Mirai ////////
    mirai = new Mirai();
    connect(mirai, SIGNAL(setUserImages(QString, QString)), this, SLOT(setUserImage(QString, QString)));


    //////// Logger ////////
    LoggerReader* loggerReader = new LoggerReader(this);
    connect(loggerReader, SIGNAL(updateLog(QString)), this, SLOT(insertLog(QString)));
    loggerReader->start();
    connectMirai();

    /////// Other /////////
    ui.inputCmd->setEnabled(false);
    ui.runCmd->setEnabled(false);
    ui.ServerLog->setEnabled(false);

    /////// Test ////////
    Regular re = { "","",Console,group,true };
    regularEdit* resub = new regularEdit(re,this);
    resub->show();

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
        "border-image:url(:/a/Images/a/1.png);"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/4.png);"
        "subcontrol-position:bottom;"
        "}"
        "QScrollBar::sub-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/2.png);"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
        "{"
        "background:rgba(0,0,0,10%);"
        "border-radius:4px;"
        "}"
    );
}

QPixmap PixmapToRound(QPixmap& src, int radius);

void CSPBot::setUserImage(QString qqNum, QString qqNick) {
    QString szUrl = "https://q.qlogo.cn/g?b=qq&nk=" + qqNum + "&s=640";
    QUrl url(szUrl);
    QNetworkAccessManager manager;
    QEventLoop loop;
    qDebug() << "Reading picture form " << url;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    //请求结束并下载完成后，退出子事件循环
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //开启子事件循环
    loop.exec();
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    ui.userImage->setPixmap(PixmapToRound(pixmap, 45)); // 你在QLabel显示图片
    ui.user->setText(qqNick);
}


QPixmap PixmapToRound(QPixmap& src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QSize size(src.width(), src.height());
    QSize size2(radius * 2, radius * 2);
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);//消锯齿
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.translate(0, 0);
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawEllipse(0, 0, size.width(), size.height());
    src.setMask(mask);
    QPixmap image = src.scaled(size2);
    return image;
}

///////////////////////////////////////////// Basic /////////////////////////////////////////////
void CSPBot::switchPage() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());//得到按下的按钮的指针
    QPushButton* btns[5] = { ui.mainPage,ui.playerPage,ui.pluginPage,ui.regularPage,ui.logPage };
    for (auto i : btns) {
        i->setStyleSheet("QPushButton{background-color:#f0f3f8;color:#666666;border-radius:10px;qproperty-iconSize: 32px 32px;}QPushButton:hover{background-color:#e0e6ee;border-radius:10px;};");
    }
    if (button == ui.mainPage) {
        ui.main->setCurrentIndex(0);
    }
    else if (button == ui.playerPage) {
        ui.main->setCurrentIndex(1);
    }
    else if (button == ui.regularPage) {
        ui.main->setCurrentIndex(2);
    }
    else if (button == ui.pluginPage) {
        ui.main->setCurrentIndex(3);
    }
    else if (button == ui.logPage) {
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
void CSPBot::mousePressEvent(QMouseEvent* event)
{
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
void CSPBot::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePressed == true)
    {
        QPoint movePot = event->globalPos() - mousePosition;
        move(movePot);
    }
}
void CSPBot::mouseReleaseEvent(QMouseEvent* event)
{
    isMousePressed = false;
}

void CSPBot::on_actionMinimize_triggered()
{
    showMinimized();
    //最大化 showMaximized()；
}
void CSPBot::on_actionClose_triggered()
{
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
    if (server != nullptr && server->started) {
        auto temp = QMessageBox::warning(
            this,
            u8"警告",
            u8"服务器还在运行，你是否要关闭?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (temp == QMessageBox::Yes)
        {
            server->forceStopServer();
            /*Callbacker cbe(EventCode::onStop);
            if (cbe.callback()) {
                event->accept();
            }
            else {
                event->ignore();
            }*/
        }
        else
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////// Server /////////////////////////////////////////////
//构造Server
void CSPBot::buildServer(int mode) {
    server = new Server(mode,this);
    //绑定检测器
    connect(server, SIGNAL(insertBDSLog(QString)), this, SLOT(slotInsertBDSLog(QString)));
    connect(server, SIGNAL(OtherCallback(QString,StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
    connect(server, SIGNAL(chenableForce(bool)), this, SLOT(slotChenableForce(bool)));
    connect(server, SIGNAL(chLabel(QString, QString)), this, SLOT(slotChLabel(QString, QString)));
    connect(server, SIGNAL(changeStatus(bool)), this, SLOT(slotChangeStatus(bool)));
    //启动Server线程
    server->start();
}

//插入BDS日志
void CSPBot::slotInsertBDSLog(QString log) {
    ui.ServerLog->setReadOnly(false);
    ui.ServerLog->append(log);
    ui.ServerLog->setReadOnly(true);
}; 

//Callback
void CSPBot::slotOtherCallback(QString listener, StringMap args) {
    
}; 

//更改状态
void CSPBot::slotChangeStatus(bool a) {
    if (a) {
        ui.ServerStatus->setText(u8"状态: 已启动");
    }
    else {
        ui.ServerStatus->setText(u8"状态: 未启动");
    }
}; 

//更改标签
void CSPBot::slotChLabel(QString title, QString content) {
    if (title == "world") {
        ui.ServerWorld->setText(u8"世界:" + content);
    }
    else if (title == "version") {
        ui.ServerVersion->setText(u8"版本:" + content);
    }
    else if (title == "difficult") {
        ui.ServerDifficult->setText(u8"难度:" + content);
    }
}; 

//更改强制停止状态
void CSPBot::slotChenableForce(bool a) {
    ui.forceStop->setEnabled(a);
    if (a) {
        /*ui.change->setText(u8"停止");*/
        ui.stop->setEnabled(true);
        ui.start->setEnabled(false);
        ui.inputCmd->setEnabled(true);
        ui.runCmd->setEnabled(true);
    }
    else {
        /*ui.change->setText(u8"启动");*/
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
    slotInsertBDSLog(u8"[CSPBot] 已向进程发出启动命令");
    ui.start->setEnabled(false);
}

//开启Cmd
void CSPBot::startCmd() {
    buildServer(1);
    ui.stop->setEnabled(true);
    slotInsertBDSLog(u8"<font style=\"color:#FFCC66;\">!!![WARNNING] [CSPBot] 您已进入<font color=\"#008000\">CMD</font>调试模式. [WARNNING]!!!</font>");
    slotInsertBDSLog(u8"[CSPBot] 已向进程发出启动命令");
    ui.start->setEnabled(false);
}

//关闭服务器
void CSPBot::stopServer() {
    server->stopServer();
    slotInsertBDSLog(u8"[CSPBot] 已向进程发出终止命令");
    ui.stop->setEnabled(false);
    ui.start->setEnabled(true);
}

//强制停止进程
void CSPBot::forceStopServer() {
    auto temp = QMessageBox::warning(this, u8"警告", u8"服务器还在运行，你是否要强制停止?", QMessageBox::Yes | QMessageBox::No);
    if (temp == QMessageBox::Yes)
    {
        server->forceStopServer();
        ui.ServerLog->append(u8"[CSPBot] 已向进程发出强制终止命令");
    }
}

//清空日志
void CSPBot::clear_console() {
    ui.ServerLog->setText("");
}

//插入命令
void CSPBot::insertCmd() {
    try {
        string cmd = Helper::QString2stdString(ui.inputCmd->text());
        if (cmd != "") {
            ui.inputCmd->setText("");
            server->sendCmd(cmd + "\n");
        }
    }
    catch (...) {

    }
}

///////////////////////////////////////////// Keyboard /////////////////////////////////////////////
void CSPBot::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && e->key() == Qt::Key_C)
    {
        emit runCmd();
    }
    else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        emit runCommand();
    }
}