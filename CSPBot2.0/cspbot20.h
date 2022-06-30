#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cspbot20.h"
#include <string>
#include "helper.h"
#include <QQueue>


class CSPBot : public QMainWindow
{
    Q_OBJECT

///////////////////////////////////////////// Public /////////////////////////////////////////////
public:
    CSPBot(QWidget *parent = Q_NULLPTR);

    //CallBack
    inline void OtherCallback(std::string name, std::unordered_map<std::string, std::string> a = {}) {};
    inline void chenableForce(bool a) {};
    inline void ipipelog(std::string a) {};
    
    inline void PacketCallback(std::string a) {};
    inline void chLabel(std::string a, std::string b) {};
    inline void CommandCallback(std::string a, std::vector<std::string> b) {};
    //槽信号使用
    inline void changeStatus(bool a) {};

///////////////////////////////////////////// Signals /////////////////////////////////////////////
signals:
    void ilog(); //Logger 槽信号

///////////////////////////////////////////// Slot /////////////////////////////////////////////
private slots:
    void on_actionMinimize_triggered();//最小化窗口
    void on_actionClose_triggered();//关闭窗口
    void switchPage();//切换函数
    void insertLog(QString a);//Logger 槽函数
    void setUserImage(QString qqNum, QString qqNick); //设置头像

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
    void closeEvent(QCloseEvent* event);
    QPoint mousePosition;
    bool isMousePressed;
    int startCount = 0;

    //////// UI ////////
    Ui::Form ui;
};
