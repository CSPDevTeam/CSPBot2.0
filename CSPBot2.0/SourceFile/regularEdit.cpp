#include "regularEdit.h"
#include <QGraphicsDropShadowEffect>
#include <qevent.h>
#include "qdebug.h"
#include "qprocess.h"
#include <QtCore/qpropertyanimation.h>
#include <QtWidgets/qmessagebox.h>

using namespace std;
QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();

regularEdit::regularEdit(Regular regular,bool newRegular, QWidget* parent)
    : QWidget(parent)
{
    regular_ui.setupUi(this);
    mRegular = regular;
    mNewRegular = newRegular;

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
    regular_ui.background->setGraphicsEffect(shadow_effect);

    vector<QWidget*> widget = {
        regular_ui.save,
        regular_ui.cancel,
        regular_ui.regularTest,
        regular_ui.remove,
    };
    for (QWidget* i : widget) {
        setGraphics(i);
    }


    //分组
    groupButton1 = new QButtonGroup(this);
    groupButton1->addButton(regular_ui.console, 0);
    groupButton1->addButton(regular_ui.group, 1);
    //ui->apple_radioButton->setChecked(true);
    setRegular();
    if (mNewRegular) {
        regular_ui.remove->setEnabled(false);
    }

    ///////// Bind //////////
    connect(regular_ui.regularTest, SIGNAL(clicked()), this, SLOT(stratProgress()));
    connect(regular_ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(regular_ui.cancel, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(regular_ui.save, SIGNAL(clicked()), this, SLOT(saveRegular()));
    connect(regular_ui.remove, SIGNAL(clicked()), this, SLOT(deleteRegular()));
    
}

void regularEdit::saveRegular() {
    //读取Yaml
    YAML::Node regular = YAML::LoadFile("data/regular.yml");

    if (mNewRegular == false) {
        //转换成存储的Regular
        string fmRegular = Helper::QString2stdString(mRegular.regular);
        string fmAction = Helper::QString2stdString(mRegular.action);
        string fmFrom;
        bool fmPermission = mRegular.permission;
        //qDebug() << "fmRegular:" << fmRegular << "\nfmAction:" << fmAction << "\nfmFrom:" << fmFrom << "\nfmPer"

        //转换来源
        switch (mRegular.from)
        {
        case group:
            fmFrom = "group";
            break;
        case console:
            fmFrom = "console";
            break;
        }

        //获取原对象
        bool removed = false;
        for (int i = 0; i < regular.size(); i++) {
            if (regular[i]["Regular"].as<string>() == fmRegular && \
                regular[i]["Action"].as<string>() == fmAction && \
                regular[i]["From"].as<string>() == fmFrom && \
                regular[i]["Permissions"].as<bool>() == fmPermission) {
                regular.remove(i);
                removed = true;
                break;
            }
        }

        if (!removed) {
            QMessageBox::warning(
                this,
                u8"警告",
                u8"无法正常删除原正则，请手动删除",
                QMessageBox::Yes
            );
        }
    }
    
    //写入新对象
    //转换成存储的Regular
    string tmRegular = Helper::QString2stdString(regular_ui.regularEdit->text());
    string tmAction = Helper::QString2stdString(regular_ui.actionEdit->text());
    string tmFrom;
    bool tmPermission = regular_ui.checkBox->checkState();

    //转换来源
    switch (groupButton1->checkedId())
    {
    case 0:
        tmFrom = "console";
        break;
    case 1:
        tmFrom = "group";
        break;
    }
    YAML::Node newRegular;
    newRegular["Regular"] = tmRegular;
    newRegular["Action"] = tmAction;
    newRegular["From"] = tmFrom;
    newRegular["Permissions"] = tmPermission;
    regular.push_back(newRegular);

    //写入文件
    ofstream fout("data/regular.yml");
    fout << regular;
    fout.close();

    this->close();

    

}

void regularEdit::deleteRegular() {
    //读取Yaml
    YAML::Node regular = YAML::LoadFile("data/regular.yml");
    //转换成存储的Regular
    string fmRegular = Helper::QString2stdString(mRegular.regular);
    string fmAction = Helper::QString2stdString(mRegular.action);
    string fmFrom;
    bool fmPermission = mRegular.permission;

    //转换来源
    switch (mRegular.from)
    {
    case group:
        fmFrom = "group";
        break;
    case console:
        fmFrom = "console";
        break;
    }

    //获取原对象
    bool removed = false;
    for (int i = 0; i < regular.size(); i++) {
        if (regular[i]["Regular"].as<string>() == fmRegular && \
            regular[i]["Action"].as<string>() == fmAction && \
            regular[i]["From"].as<string>() == fmFrom && \
            regular[i]["Permissions"].as<bool>() == fmPermission) {
            regular.remove(i);
            removed = true;
            break;
        }
    }

    if (!removed) {
        QMessageBox::warning(
            this,
            u8"警告",
            u8"无法正常删除正则，请手动删除",
            QMessageBox::Yes
        );
    }

    //写入文件
    ofstream fout("data/regular.yml");
    fout << regular;
    fout.close();

    this->close();
}

void regularEdit::setRegular() {
    //设置正则
    regular_ui.regularEdit->setText(mRegular.regular);
    regular_ui.actionEdit->setText(mRegular.action);
    switch (mRegular.from)
    {
    case console:
        regular_ui.console->setChecked(true);
        break;
    case group:
        regular_ui.group->setChecked(true);
        break;
    }
    //设置是否需要权限
    regular_ui.checkBox->setChecked(mRegular.permission);
    
}

void regularEdit::stratProgress() {
    QProcess process(this);
    process.startDetached("regularTester.exe");
}

///////////////////////////////////////////// Style /////////////////////////////////////////////
void regularEdit::setGraphics(QWidget* bt) {
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(8);
    bt->setGraphicsEffect(shadow_effect);
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//标题栏的长度
const static int pos_min_x = 0;
const static int pos_max_x = 500;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//自己实现的窗口拖动操作
void regularEdit::mousePressEvent(QMouseEvent* event)
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
void regularEdit::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePressed == true)
    {
        QPoint movePot = event->globalPos() - mousePosition;
        move(movePot);
    }
}
void regularEdit::mouseReleaseEvent(QMouseEvent* event)
{
    isMousePressed = false;
}

void regularEdit::on_actionClose_triggered()
{
    close();
}