#pragma once

#include <QtWidgets/QMainWindow>
#include "qnetworkreply.h"
#include "ui_cspbot20.h"
#include <string>
#include "helper.h"
#include <QQueue>

//Type
typedef std::unordered_map<std::string, std::string> StringMap;
typedef time_t mTime;
typedef std::vector<std::string> StringVector;

class CSPBot : public QMainWindow
{
    Q_OBJECT

///////////////////////////////////////////// Public /////////////////////////////////////////////
public:
    CSPBot(QWidget *parent = Q_NULLPTR);

    inline void publickStartSever(){
        emit signalStartServer();
    }

    inline void publicStartLogger() {
        emit signalStartLogger();
    }


///////////////////////////////////////////// Signals /////////////////////////////////////////////
signals:
    void ilog(); //Logger 槽信号
    void runCmd(); //启动Cmd
    void runCommand(); //运行命令
    void signalStartServer(); //开启服务器
    void signalStartLogger(); //开启Logger服务
    void signalDebug(); //开启Debug测试


///////////////////////////////////////////// Slot /////////////////////////////////////////////
private slots:
    void on_actionMinimize_triggered();//最小化窗口
    void on_actionClose_triggered();//关闭窗口
    void switchPage();//切换函数
    void insertLog(QString a);//Logger 槽函数
    void setUserImage(QString qqNum, QString qqNick); //设置头像
    void startLogger(); //启动日志
    void showAbout(); //展示About窗口

    /////////////// Server //////////////////
    //ServerRebackSlot
    void slotChenableForce(bool a);
    void slotInsertBDSLog(QString log);
    void slotChangeStatus(bool a);
    void slotChLabel(QString title, QString content);
    //ServerMainSlot
    void startServer(); //开启服务器
    void stopServer(); //停止服务器
    void forceStopServer(); //强制停止服务器
    void clear_console(); //清空BDS日志
    void startCmd(); //启动cmd
    void insertCmd(); //插入命令

    /////////////// Mirai //////////////////
    void slotConnectMirai(); //连接Mirai
    void slotDisConnectMirai(); //断开Mirai
    void slotMiraiMessageBox(); //Mirai消息框
    void setUserImageError(QNetworkReply::NetworkError e); //设置头像时出现ERROR
    void slotSendCommand(QString cmd); //Mirai传输命令
    void slotPacketCallback(QString msg); //Mirai收到包
    

    /////////////// Console //////////////////
    void slotSaveConsole(); //保存控制台文件
    void slotClearConsole(); //清空控制台
    void slotUpdateSendRecive(int send, int recive); //更新收发
    void slotConnected(mTime time); //更新时间

    /////////////// Global //////////////////
    void slotTimerFunc(); //Timer启动函数

    /////////////// Table //////////////////
    void clickRegularTable(QModelIndex index);
    void doubleClickedRegularTable(QModelIndex index);
    void newRegular(); //新建正则

    /////////////// Plugin //////////////////
    bool slotOtherCallback(QString listener, StringMap args);
    void slotCommandCallback(QString cmd, StringVector args);

    /////////////// Plugin //////////////////
    void slotDebug();
    

///////////////////////////////////////////// Private /////////////////////////////////////////////
private:
    //////// Basic ////////
    //添加阴影
    void setGraphics(QWidget* bt);

    //////// Window ////////
    void mouseMoveEvent(QMouseEvent* event);
    void setAllScrollbar(QScrollBar* bar);
    //void setGraphics(QWidget* bt);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent*);
    //关闭事件
    bool checkClose();
    QPoint mousePosition;
    bool isMousePressed;
    int startCount = 0;

    //////// Server ////////
    void buildServer(int mode=0);


    /////// Keyboard ///////
    void keyPressEvent(QKeyEvent* e);

    /////// Keyboard ///////
    mTime mGetTime;

    /////// Table ///////
    void InitPlayerTableView();
    void InitRegularTableView();
    void InitPluginTableView();
    

    //////// UI ////////
    Ui::Form ui;
};
