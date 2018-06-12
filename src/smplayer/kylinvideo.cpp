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

#include "kylinvideo.h"
#include "global.h"
#include "paths.h"
#include "translator.h"
#include "version.h"
#include "config.h"
#include "cleanconfig.h"
#include "myapplication.h"
#include <QDir>
#include <QUrl>
#include <QTime>
#include <stdio.h>
#include <QDebug>

using namespace Global;

BaseGui * KylinVideo::main_window = 0;

KylinVideo::KylinVideo(const QString &arch, QObject * parent )
	: QObject(parent) 
{
    arch_type = arch;

	close_at_end = -1; // Not set
	start_in_fullscreen = -1; // Not set

	move_gui = false;
	resize_gui = false;

    Paths::setAppPath(qApp->applicationDirPath());//snap: /snap/kylin-video/x1/usr/bin    deb:/usr/bin

    global_init(arch_type);

	// Application translations
    translator->load();
	showInfo();
}

KylinVideo::~KylinVideo() {
	if (main_window != 0) {
		deleteGUI();
	}
	global_end();
}

BaseGui * KylinVideo::gui() {
	if (main_window == 0) {
		// Changes to app path, so smplayer can find a relative mplayer path
		QDir::setCurrent(Paths::appPath());

        main_window = createGUI(this->arch_type/*gui_to_use*/);

		if (move_gui) {
            qDebug("KylinVideo::gui: moving main window to %d %d", gui_position.x(), gui_position.y());
			main_window->move(gui_position);
		}
        if (resize_gui) {
            qDebug("KylinVideo::gui: resizing main window to %dx%d", gui_size.width(), gui_size.height());
            main_window->resize(gui_size);
		}
	}

	return main_window;
}

BaseGui * KylinVideo::createGUI(QString arch_type/*QString gui_name*/) {
    BaseGui * gui = 0;
    gui = new BaseGui(arch_type, 0);//kobe:forced to go here always
	gui->setForceCloseOnFinish(close_at_end);
	gui->setForceStartInFullscreen(start_in_fullscreen);
	connect(gui, SIGNAL(quitSolicited()), qApp, SLOT(quit()));
    connect(gui, SIGNAL(guiChanged()), this, SLOT(changeGUI()));//kobe 20170710

#if SINGLE_INSTANCE
	MyApplication * app = MyApplication::instance();
	connect(app, SIGNAL(messageReceived(const QString&)),
            gui, SLOT(handleMessageFromOtherInstances(const QString&)));
	app->setActivationWindow(gui);
#endif

	return gui;
}

void KylinVideo::deleteGUI() {
    delete main_window;
    main_window = 0;
}

void KylinVideo::changeGUI() {//kobe 20170710
	deleteGUI();

    main_window = createGUI(this->arch_type);

	main_window->show();
}

KylinVideo::ExitCode KylinVideo::processArgs(QStringList args) {
	QString action; // Action to be passed to running instance
	bool show_help = false;
	bool add_to_playlist = false;

	if (args.contains("-delete-config")) {
		CleanConfig::clean(Paths::configPath());
		return NoError;
	}

	for (int n = 1; n < args.count(); n++) {
		QString argument = args[n];

		if (argument == "-send-action") {
			if (n+1 < args.count()) {
				n++;
				action = args[n];
			} else {
				printf("Error: expected parameter for -send-action\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-sub") {
			if (n+1 < args.count()) {
				n++;
				QString file = args[n];
				if (QFile::exists(file)) {
					subtitle_file = QFileInfo(file).absoluteFilePath();
				} else {
					printf("Error: file '%s' doesn't exists\r\n", file.toUtf8().constData());
				}
			} else {
				printf("Error: expected parameter for -sub\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-media-title") {
			if (n+1 < args.count()) {
				n++;
				if (media_title.isEmpty()) media_title = args[n];
			}
		}
		else
		if (argument == "-pos") {
			if (n+2 < args.count()) {
				bool ok_x, ok_y;
				n++;
				gui_position.setX( args[n].toInt(&ok_x) );
				n++;
				gui_position.setY( args[n].toInt(&ok_y) );
				if (ok_x && ok_y) move_gui = true;
			} else {
				printf("Error: expected parameter for -pos\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-size") {
			if (n+2 < args.count()) {
				bool ok_width, ok_height;
				n++;
				gui_size.setWidth( args[n].toInt(&ok_width) );
				n++;
				gui_size.setHeight( args[n].toInt(&ok_height) );
				if (ok_width && ok_height) resize_gui = true;
			} else {
				printf("Error: expected parameter for -resize\r\n");
				return ErrorArgument;
			}
		}
		else
		if ((argument == "--help") || (argument == "-help") ||
            (argument == "-h") || (argument == "-?") ) 
		{
			show_help = true;
		}
		else
		if (argument == "-close-at-end") {
			close_at_end = 1;
		}
		else
		if (argument == "-no-close-at-end") {
			close_at_end = 0;
		}
		else
		if (argument == "-fullscreen") {
			start_in_fullscreen = 1;
		}
		else
		if (argument == "-no-fullscreen") {
			start_in_fullscreen = 0;
		}
		else
		if (argument == "-add-to-playlist") {
			add_to_playlist = true;
		}
        else
        if (argument == "-ontop") {
            pref->stay_on_top = Preferences::AlwaysOnTop;
        }
        else
        if (argument == "-no-ontop") {
            pref->stay_on_top = Preferences::NeverOnTop;
        }
		else {
			// File
			#if QT_VERSION >= 0x040600
			QUrl fUrl = QUrl::fromUserInput(argument);
			if (fUrl.isValid() && fUrl.scheme().toLower() == "file") {
			    argument = fUrl.toLocalFile();
			}
			#endif
			if (QFile::exists( argument )) {
				argument = QFileInfo(argument).absoluteFilePath();
			}
			files_to_play.append( argument );
		}
	}

	for (int n=0; n < files_to_play.count(); n++) {
        qDebug("KylinVideo::processArgs: files_to_play[%d]: '%s'", n, files_to_play[n].toUtf8().data());
	}

    // Single instance
    MyApplication * a = MyApplication::instance();
    if (a->isRunning()) {
        a->sendMessage("Hello");

        if (!action.isEmpty()) {
            a->sendMessage("action " + action);
        }
        else {
            if (!subtitle_file.isEmpty()) {
                a->sendMessage("load_sub " + subtitle_file);
            }

            if (!media_title.isEmpty()) {
                a->sendMessage("media_title " + files_to_play[0] + " <<sep>> " + media_title);
            }

            if (!files_to_play.isEmpty()) {
                /* a->sendMessage("open_file " + files_to_play[0]); */
                QString command = "open_files";
                if (add_to_playlist) command = "add_to_playlist";
                a->sendMessage(command +" "+ files_to_play.join(" <<sep>> "));
            }
        }

        return NoError;
    }

    return KylinVideo::NoExit;
}

void KylinVideo::start() {
    //kobe:托盘启动时显示主界面与否，与配置文件~/.config/smplayer/smplayer.ini的变量mainwindow_visible和文件列表变量files_to_play有关，可修改BaseGuiPlus的构造函数里面的mainwindow_visible=false让软件第一次使用托盘时显示主界面
	if (!gui()->startHidden() || !files_to_play.isEmpty() ) gui()->show();
	if (!files_to_play.isEmpty()) {
		if (!subtitle_file.isEmpty()) gui()->setInitialSubtitle(subtitle_file);
		if (!media_title.isEmpty()) gui()->getCore()->addForcedTitle(files_to_play[0], media_title);
		gui()->openFiles(files_to_play);
	}
}

void KylinVideo::showInfo() {
    QString s = QObject::tr("This is Kylin Vedio v. %1 running on %2")
            .arg(Version::printable())
#ifdef Q_OS_LINUX
           .arg("Linux")
#else
		   .arg("Other OS")
#endif
           ;

	printf("%s\n", s.toLocal8Bit().data() );
	qDebug("%s", s.toUtf8().data() );
	qDebug("Compiled with Qt v. %s, using %s", QT_VERSION_STR, qVersion());

	qDebug(" * application path: '%s'", Paths::appPath().toUtf8().data());
	qDebug(" * config path: '%s'", Paths::configPath().toUtf8().data());
	qDebug(" * ini path: '%s'", Paths::iniPath().toUtf8().data());
    qDebug(" * current path: '%s'", QDir::currentPath().toUtf8().data());//snap:/home/lixiang/work/snap/kylin-video
}

//#include "moc_kylinvideo.cpp"
