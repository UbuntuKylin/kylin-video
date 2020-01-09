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

#ifndef _KYLINVIDEO_H_
#define _KYLINVIDEO_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include "mainwindow.h"

class InfoWorker;
//class ControllerWorker;

class KylinVideo : public QObject
{
	Q_OBJECT

public:
	enum ExitCode { ErrorArgument = -3, NoAction = -2, NoRunningInstance = -1, NoError = 0, NoExit = 1 };

    KylinVideo(const QString &arch = QString::null, const QString &snap = QString::null, /*ControllerWorker *controller = nullptr, */QObject * parent = 0);
	~KylinVideo();

	//! Process arguments. If ExitCode != NoExit the application must be exited.
	ExitCode processArgs(QStringList args);

    MainWindow *gui();

private:
    MainWindow *createGUI(QString arch, QString snap);
	void deleteGUI();
	void showInfo();
	void deleteConfig();
    static MainWindow *main_window;
    QStringList m_filesToPlay;
    QString m_subtitleFile;
    QString m_mediaTitle; //!< Force a title for the first file
	// Change position and size
    bool m_moveGui;
    QPoint m_guiPosition;
    bool m_resizeGui;
    QSize m_guiSize;
    QString m_arch;
    QString m_snap;
    QThread *m_thread = nullptr;
    InfoWorker *m_infoWorker = nullptr;
//    ControllerWorker *m_controllerWorker = nullptr;
};

#endif
