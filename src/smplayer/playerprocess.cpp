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

#include "playerprocess.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "mpvprocess.h"
#include "mplayerprocess.h"

PlayerProcess::PlayerProcess(QObject * parent) : MyProcess(parent) {
    qRegisterMetaType<SubTracks>("SubTracks");//字幕
    qRegisterMetaType<Tracks>("Tracks");//音频和视频使用的数据类型
    qRegisterMetaType<Chapters>("Chapters");//章节
}

void PlayerProcess::writeToStdin(QString text) {
	if (isRunning()) {
        //qDebug("PlayerProcess::writeToStdin: %s", text.toUtf8().constData());
		write( text.toLocal8Bit() + "\n");
	} else {
        qWarning("PlayerProcess::writeToStdin: process not running");
	}
}

PlayerProcess * PlayerProcess::createPlayerProcess(const QString & player_bin, const QString & snap, QObject * parent) {
    PlayerProcess * proc = 0;
    //edited by kobe 20180623
    if (Utils::player(player_bin/*, snap*/) == Utils::MPLAYER) {//kobe:go here
        proc = new MplayerProcess(parent);
    } else {
        proc = new MPVProcess(snap, parent);
    }

	return proc;
}

//#ifdef CAPTURE_STREAM
//void PlayerProcess::setCaptureDirectory(const QString & dir) {
//	capture_filename = "";
//	if (!dir.isEmpty() && (QFileInfo(dir).isDir())) {
//		// Find a unique filename
//		QString prefix = "capture";
//		for (int n = 1; ; n++) {
//			QString c = QDir::toNativeSeparators(QString("%1/%2_%3.dump").arg(dir).arg(prefix).arg(n, 4, 10, QChar('0')));
//			if (!QFile::exists(c)) {
//				capture_filename = c;
//				return;
//			}
//		}
//	}
//}
//#endif

//#include "moc_playerprocess.cpp"
