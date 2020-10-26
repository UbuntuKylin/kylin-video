/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "myapplication.h"
#include "kylinvideo.h"
//#include "remotecontroller.h"
//#include "controllerworker.h"

#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QtDBus>
#include <QDesktopWidget>
#include <X11/Xlib.h>

#include <signal.h>

#define CONTROL_DBUS_SERVICE_NAME  "com.kylin.kylinvideo.controller"

int main(int argc, char **argv)
{
    signal(SIGINT, [](int) { QApplication::quit(); });// 设置退出信号

//#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
//    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
//#endif
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif


    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MyApplication a("kylin-video", argc, argv );
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setQuitOnLastWindowClosed(false);
    a.setOrganizationName("kylin");
    a.setApplicationName("kylin-video");
    a.setApplicationVersion("2.1.1");

#if QT_VERSION >= 0x040400
    // Enable icons in menus
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#endif

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif


    /*
    //----------------register controller dbus service----------------
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    //registerService在FT arm上将导致麒麟影音在播放视频时，如果在其他视频文件上右键选择麒麟影音进行播放，麒麟影音无反应，即无法调用handleMessageFromOtherInstances()，而在x86上无此问题
    if (!connection.registerService(CONTROL_DBUS_SERVICE_NAME)) {
        fprintf(stderr, "%s\n", qPrintable(connection.lastError().message()));
        exit(1);
    }

    ControllerWorker *controller = new ControllerWorker;
    ControllerAdaptor *adaptor = new ControllerAdaptor(controller);
//    connection(&a, &QApplication::aboutToQuit, adaptor, &ControllerAdaptor::aboutToQuit);
    QDBusConnection::sessionBus().registerObject("/", controller);
    //-------------------------------------------------------------------
    */

    QStringList args = a.arguments();
    QFile qss(":/qss/res/style.qss");
    if (!qss.open(QIODevice::ReadOnly)) {
        qWarning("Can't open the style sheet file: :/qss/res/style.qss.");
    }
    else {
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    }

    QString arch = "";

#ifdef __x86_64__
        arch = "x86_64";
#elif __i386__
        arch = "i386";
#elif __aarch64__
        arch = "aarch64";
#endif

    const char *snap;
    if ((snap = getenv("SNAP")) != NULL) {///snap/kylin-video/x1/
        QString snap_path = QString::fromStdString(std::string(snap));
        qDebug() << "SNAP: " << snap_path;
    }

    KylinVideo *player = new KylinVideo(arch, snap/*, controller*/);
    KylinVideo::ExitCode c = player->processArgs(args);
    if (c != KylinVideo::NoExit) {
		return c;
	}
	int r = a.exec();
    delete player;

	return r;
}
