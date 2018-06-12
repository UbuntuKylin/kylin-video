/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "myapplication.h"
#include "kylinvideo.h"

#include <QDir>
#include <QFile>
#include <QTextCodec>
//#include <QTranslator>
//#include <QLibraryInfo>

int main( int argc, char ** argv ) 
{
    MyApplication a("kylin-video", argc, argv );
    a.setQuitOnLastWindowClosed(false);

    a.setOrganizationName("kylin");
    a.setApplicationName("kylin-video");
    a.setApplicationVersion("1.1.6");
	
#if QT_VERSION >= 0x040400
	// Enable icons in menus
	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#endif

	QStringList args = a.arguments();

//    QString locale = QLocale::system().name();
//    QTranslator translator;
//    if(locale == "zh_CN") {
//        if(!translator.load("kylin-video_" + locale + ".qm", ":/qm/translations/"))
//            qDebug() << "Load translation file："<< "kylin-video_" + locale + ".qm" << " failed!";
//        else
//            a.installTranslator(&translator);
//    }

//    //加载Qt对话框默认的国际化
//    QTranslator qtTranslator;
//    if (!qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
//        qDebug() << "Load Qt translations file："<< "/usr/share/qt5/translations/qt_" + locale + ".qm" << " failed!";
//    else
//        a.installTranslator(&qtTranslator);

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

    KylinVideo *player = new KylinVideo(arch);
    KylinVideo::ExitCode c = player->processArgs(args);
    if (c != KylinVideo::NoExit) {
		return c;
	}
    player->start();

	int r = a.exec();

    delete player;

	return r;
}
