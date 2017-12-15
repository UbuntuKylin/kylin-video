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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _KYLINVIDEO_H_
#define _KYLINVIDEO_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include "merge/basegui.h"

class KylinVideo : public QObject
{
	Q_OBJECT

public:
	enum ExitCode { ErrorArgument = -3, NoAction = -2, NoRunningInstance = -1, NoError = 0, NoExit = 1 };

    KylinVideo(const QString &arch = QString::null, QObject * parent = 0);
	~KylinVideo();

	//! Process arguments. If ExitCode != NoExit the application must be exited.
	ExitCode processArgs(QStringList args);

    BaseGui * gui();

	void start();

private slots:
    void changeGUI();

private:
    BaseGui * createGUI(QString arch_type);
	void deleteGUI();
	void showInfo();
	void deleteConfig();
    static BaseGui * main_window;
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
};

#endif
