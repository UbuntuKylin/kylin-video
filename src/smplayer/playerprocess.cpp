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

#include "playerprocess.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "mpvprocess.h"
#include "mplayerprocess.h"

PlayerProcess::PlayerProcess(QObject * parent) : MyProcess(parent) {
#if NOTIFY_SUB_CHANGES
	qRegisterMetaType<SubTracks>("SubTracks");
#endif

#if NOTIFY_AUDIO_CHANGES
	qRegisterMetaType<Tracks>("Tracks");
#endif
}

void PlayerProcess::writeToStdin(QString text) {
	if (isRunning()) {
		write( text.toLocal8Bit() + "\n");
	} else {
        qWarning("PlayerProcess::writeToStdin: process not running");
	}
}

PlayerProcess * PlayerProcess::createPlayerProcess(const QString & player_bin, QObject * parent) {
	PlayerProcess * proc = 0;
    if (PlayerID::player(player_bin) == PlayerID::MPLAYER) {//kobe:go here
        proc = new MplayerProcess(parent);
    } else {
        proc = new MPVProcess(parent);
    }

	return proc;
}

//#include "moc_playerprocess.cpp"
