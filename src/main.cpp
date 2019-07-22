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

#include <QDir>
#include <QFile>
#include <QTextCodec>

int main(int argc, char **argv)
{
    MyApplication a("kylin-video", argc, argv );
    a.setQuitOnLastWindowClosed(false);

    a.setOrganizationName("kylin");
    a.setApplicationName("kylin-video");
    a.setApplicationVersion("2.0.0");
	
#if QT_VERSION >= 0x040400
	// Enable icons in menus
	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#endif

	QStringList args = a.arguments();

    QFile qss(":/qss/res/style.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

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

    KylinVideo *player = new KylinVideo(arch, snap);
    KylinVideo::ExitCode c = player->processArgs(args);
    if (c != KylinVideo::NoExit) {
		return c;
	}

	int r = a.exec();
    delete player;

	return r;
}
