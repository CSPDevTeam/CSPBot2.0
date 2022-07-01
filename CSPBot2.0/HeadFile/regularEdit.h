#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_regularEdit.h"
#include "helper.h"
#include <QButtonGroup>

class regularEdit : public QWidget
{
    Q_OBJECT

public:
    regularEdit(Regular regular,QWidget* parent = Q_NULLPTR);

private slots:
    void on_actionClose_triggered();//¹Ø±Õ´°¿Ú
    /////// Start Progress ////////
    void stratProgress();

private:
    //////// Window ////////
    void mouseMoveEvent(QMouseEvent* event);
    //void setGraphics(QWidget* bt);
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

    

    //´æ´¢
    Regular mRegular;
    Ui::Regular regular_ui;
};