/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    lixiang@kylinos.cn/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "systemtray.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include "myaction.h"
#include "images.h"

SystemTray::SystemTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    this->createAction();
    this->addActions();
    this->translateLanguage();
}

SystemTray::~SystemTray()
{
    if (tray_menu != NULL) {
        delete tray_menu;
        tray_menu = NULL;
    }
}

void SystemTray::translateLanguage() {
    //放在托盘图标上时候显示
    this->setToolTip(tr("Kylin Video"));
}

void SystemTray::createTopAction() {

}

void SystemTray::createBottomAction() {

}

void SystemTray::createAction() {
    this->setIcon(Images::icon("logo", 22));

    tray_menu = new QMenu();

    action_show = new MyAction(this, "open_window");
    action_show->change(Images::icon("open_window_normal"), tr("Open Homepage"));

    action_opendir = new MyAction(this, "open_dir");
    action_opendir->change(Images::icon("open_screen"), tr("Open screenshots folder"));

    action_pref = new MyAction(QKeySequence("Ctrl+P"), this, "show_preferences" );
    action_pref->change(QPixmap(":/res/prefs.png"), tr("Preferences"));//首选项

    action_help = new MyAction(QKeySequence("Ctrl+H"), this, "show_help" );
    action_help->change(QPixmap(":/res/help_normal.png"), tr("Help"));

    action_about = new MyAction(QKeySequence("Ctrl+A"), this, "about_kylin_video");
    action_about->change(Images::icon("about_normal"), tr("About"));

    action_exit = new MyAction(this, "quit");
    action_exit->change(Images::icon("quit_normal"), tr("Quit"));

    connect(action_show, SIGNAL(triggered()), this, SIGNAL(showWidget()));
    connect(action_opendir, SIGNAL(triggered()), this, SIGNAL(sig_open_screenshot_dir()));
    connect(action_pref, SIGNAL(triggered()), this, SIGNAL(showPref()));
    connect(action_help, SIGNAL(triggered()), this, SIGNAL(showHelp()));
    connect(action_about, SIGNAL(triggered()), this, SIGNAL(showAbout()));
    connect(action_exit, SIGNAL(triggered()), this, SIGNAL(quitApp()));

    tray_menu->setFixedWidth(250);

    this->setContextMenu(tray_menu);
}

void SystemTray::addActions() {
    //添加菜单项
    tray_menu->addAction(action_show);
    tray_menu->addAction(action_opendir);
    tray_menu->addSeparator();
    tray_menu->addAction(action_about);
    tray_menu->addAction(action_help);
    tray_menu->addSeparator();
    tray_menu->addAction(action_pref);
    tray_menu->addAction(action_exit);
}

void SystemTray::showTipMsg(QString msg) {
    this->showMessage(tr("Information"), msg, QSystemTrayIcon::Information, 2000);//QSystemTrayIcon::Warning
}
