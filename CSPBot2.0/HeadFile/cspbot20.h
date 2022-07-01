#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cspbot20.h"
#include <string>
#include "helper.h"
#include <QQueue>

//Type
typedef std::unordered_map<std::string, std::string> StringMap;

class CSPBot : public QMainWindow
{
    Q_OBJECT

///////////////////////////////////////////// Public /////////////////////////////////////////////
public:
    CSPBot(QWidget *parent = Q_NULLPTR);


///////////////////////////////////////////// Signals /////////////////////////////////////////////
signals:
    void ilog(); //Logger 槽信号
    void runCmd(); //启动Cmd
    void runCommand(); //运行命令
    void signalStartServer(); //开启服务器

///////////////////////////////////////////// Slot /////////////////////////////////////////////
private slots:
    void on_actionMinimize_triggered();//最小化窗口
    void on_actionClose_triggered();//关闭窗口
    void switchPage();//切换函数
    void insertLog(QString a);//Logger 槽函数
    void setUserImage(QString qqNum, QString qqNick); //设置头像

    /////////////// Server //////////////////
    //ServerRebackSlot
    void slotOtherCallback(QString listener, StringMap args);
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
    //void PacketCallback(std::string a);
    //void CommandCallback(std::string a, std::vector<std::string> b);
    

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

    //////// UI ////////
    Ui::Form ui;
};
