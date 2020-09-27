#include "restarttip.h"
#include "ui_restarttip.h"

// lc add .class 切换播放引擎提示重启

RestartTip* RestartTip::instance = nullptr;

RestartTip::RestartTip(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestartTip)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    ui->pushButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
}

void RestartTip::showTip()
{
    if(instance == nullptr)
        instance = new RestartTip(nullptr);
    instance->show();
}

RestartTip::~RestartTip()
{
    delete ui;
}

void RestartTip::on_pushButton_clicked()
{
    exit(0);
}
