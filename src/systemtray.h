#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>

class MyAction;

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT

public:

    explicit SystemTray(QWidget *parent = 0);
    ~SystemTray();
    void createAction();
    void addActions();
    void translateLanguage();
    void showTipMsg(QString msg);

signals:
    void showWidget();
    void showInfo();
    void showPref();
    void showHelp();
    void showAbout();
    void quitApp();
    void sig_open_screenshot_dir();

private:
    void createTopAction();
    void createBottomAction();

private:
    QMenu *tray_menu; //托盘菜单
    MyAction *action_show;
    MyAction *action_opendir;
    MyAction *action_pref;
    MyAction *action_help;
    MyAction *action_about;
    MyAction *action_exit;
};

#endif // SYSTEMTRAY_H
