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

#include "kylinvideo.h"
#include "myapplication.h"
#include "infoworker.h"
//#include "controllerworker.h"

#include "../smplayer/global.h"
#include "../smplayer/paths.h"
#include "../smplayer/translator.h"
#include "../smplayer/version.h"
#include "../smplayer/cleanconfig.h"

#include <QDir>
#include <QUrl>
#include <QTime>
#include <QThread>
#include <QDebug>

using namespace Global;

MainWindow * KylinVideo::main_window = 0;

KylinVideo::KylinVideo(const QString &arch, const QString &snap, /*ControllerWorker *controller, */QObject *parent)
    : QObject(parent)
//    , m_controllerWorker(controller)
    , m_moveGui(false)
    , m_resizeGui(false)
    , m_arch(arch)
    , m_snap(snap)
{
    Paths::setAppPath(qApp->applicationDirPath());//snap: /snap/kylin-video/x1/usr/bin    deb:/usr/bin

    global_init(m_arch);

    translator->load(this->m_snap);

    m_infoWorker = new InfoWorker;
    m_thread = new QThread;
    m_infoWorker->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, [=] () {
        // 托盘启动时显示主界面与否，与配置文件~/.config/kylin-video/kylin-video.ini的变量mainwindow_visible和文件列表变量files_to_play有关，可修改构造函数里面的mainwindow_visible=false让软件第一次使用托盘时显示主界面
        gui()->show();
        main_window->bindThreadWorker(this->m_infoWorker);

        if (!m_filesToPlay.isEmpty()) {
            if (!m_subtitleFile.isEmpty()) gui()->setInitialSubtitle(m_subtitleFile);
            if (!m_mediaTitle.isEmpty()) gui()->getCore()->addForcedTitle(m_filesToPlay[0], m_mediaTitle);
            gui()->openFiles(m_filesToPlay);
        }
    });
    m_thread->start();

    showInfo();
}

KylinVideo::~KylinVideo()
{
    if (m_infoWorker) {
        m_infoWorker->deleteLater();
    }

    if (m_thread) {
        m_thread->quit();
        m_thread->wait(2000);
    }

	if (main_window != 0) {
		deleteGUI();
	}

	global_end();
}

MainWindow * KylinVideo::gui()
{
	if (main_window == 0) {
		QDir::setCurrent(Paths::appPath());
        main_window = createGUI(this->m_arch, this->m_snap);

        if (m_moveGui) {
            main_window->move(m_guiPosition);
		}
        if (m_resizeGui) {
            main_window->resize(m_guiSize);
		}
	}

	return main_window;
}

// lc add 20200908 提示用户重启
#include "smplayer/restarttip.h"
MainWindow * KylinVideo::createGUI(QString arch, QString snap)
{
    MainWindow * gui = 0;
    gui = new MainWindow(arch, snap, /*m_controllerWorker, */0);
    connect(gui, &MainWindow::requestGuiChanged, this, [=] () {
        deleteGUI();
        main_window = createGUI(this->m_arch, this->m_snap);
        main_window->show();
        RestartTip::showTip();
    });

#if SINGLE_INSTANCE
	MyApplication * app = MyApplication::instance();
    connect(app, SIGNAL(messageReceived(const QString&)), gui, SLOT(handleMessageFromOtherInstances(const QString&)));
	app->setActivationWindow(gui);
#endif

	return gui;
}

void KylinVideo::deleteGUI()
{
    delete main_window;
    main_window = 0;
}

KylinVideo::ExitCode KylinVideo::processArgs(QStringList args)
{
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
                    m_subtitleFile = QFileInfo(file).absoluteFilePath();
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
                if (m_mediaTitle.isEmpty()) m_mediaTitle = args[n];
			}
		}
		else
		if (argument == "-pos") {
			if (n+2 < args.count()) {
				bool ok_x, ok_y;
				n++;
                m_guiPosition.setX( args[n].toInt(&ok_x) );
				n++;
                m_guiPosition.setY( args[n].toInt(&ok_y) );
                if (ok_x && ok_y) m_moveGui = true;
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
                m_guiSize.setWidth( args[n].toInt(&ok_width) );
				n++;
                m_guiSize.setHeight( args[n].toInt(&ok_height) );
                if (ok_width && ok_height) m_resizeGui = true;
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
            m_filesToPlay.append( argument );
		}
	}

    for (int n=0; n < m_filesToPlay.count(); n++) {
        qDebug("KylinVideo::processArgs: m_filesToPlay[%d]: '%s'", n, m_filesToPlay[n].toUtf8().data());
	}

    // Single instance
    MyApplication * a = MyApplication::instance();
    if (a->isRunning()) {
        a->sendMessage("Hello");

        if (!action.isEmpty()) {
            a->sendMessage("action " + action);
        }
        else {
            if (!m_subtitleFile.isEmpty()) {
                a->sendMessage("load_sub " + m_subtitleFile);
            }

            if (!m_mediaTitle.isEmpty()) {
                a->sendMessage("media_title " + m_filesToPlay[0] + " <<sep>> " + m_mediaTitle);
            }

            if (!m_filesToPlay.isEmpty()) {
                QString command = "open_files";
                if (add_to_playlist) command = "add_to_playlist";
                a->sendMessage(command +" "+ m_filesToPlay.join(" <<sep>> "));
            }
        }

        return NoError;
    }

    return KylinVideo::NoExit;
}

void KylinVideo::showInfo()
{
    QString s = QObject::tr("This is Kylin Vedio v. %1 running on %2")
            .arg(Version::printable())
#ifdef Q_OS_LINUX
           .arg("Linux")
#else
		   .arg("Other OS")
#endif
           ;

    qDebug("%s", s.toUtf8().data());//printf("%s\n", s.toLocal8Bit().data());
	qDebug("Compiled with Qt v. %s, using %s", QT_VERSION_STR, qVersion());
	qDebug(" * application path: '%s'", Paths::appPath().toUtf8().data());
	qDebug(" * config path: '%s'", Paths::configPath().toUtf8().data());
	qDebug(" * ini path: '%s'", Paths::iniPath().toUtf8().data());
    qDebug(" * current path: '%s'", QDir::currentPath().toUtf8().data());//snap:/home/lixiang/work/snap/kylin-video
}
