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
#include "merge/basegui.h"

class InfoWorker;

class KylinVideo : public QObject
{
	Q_OBJECT

public:
	enum ExitCode { ErrorArgument = -3, NoAction = -2, NoRunningInstance = -1, NoError = 0, NoExit = 1 };

    KylinVideo(const QString &arch = QString::null, const QString &snap = QString::null, QObject * parent = 0);
	~KylinVideo();

	//! Process arguments. If ExitCode != NoExit the application must be exited.
	ExitCode processArgs(QStringList args);

    BaseGui *gui();

public slots:
    void changeGUI();
    void showWindow();

private:
    BaseGui *createGUI(QString arch_type, QString snap);
	void deleteGUI();
	void showInfo();
	void deleteConfig();
    static BaseGui *main_window;
	QStringList files_to_play;
	QString subtitle_file;
	QString media_title; //!< Force a title for the first file

	// Change position and size
	bool move_gui;
	QPoint gui_position;

	bool resize_gui;
	QSize gui_size;

	// Options to pass to gui
	int close_at_end; // -1 = not set, 1 = true, 0 false
	int start_in_fullscreen; // -1 = not set, 1 = true, 0 false
    QString arch_type;

    QString m_snap;

    QThread *thread = nullptr;
    InfoWorker *m_infoWorker = nullptr;
};

#endif
