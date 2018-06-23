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

#include "playerid.h"
#include <QFileInfo>
#include <QDebug>

//edited by kobe 20180623
PlayerID::Player PlayerID::player(const QString & player_bin/*, const QString &snap*/) {
    /*Player p;
	QString bin_name;

    //edited by kobe 20180623
    QString m_player_bin;
    if (!snap.isEmpty()) {
        m_player_bin = QString("%1%2").arg(snap).arg(player_bin);
    }
    else {
        m_player_bin = player_bin;
    }

    QFileInfo fi(m_player_bin);
	if (fi.exists() && fi.isExecutable() && !fi.isDir()) {
		bin_name = fi.fileName();
	} else {
        bin_name = m_player_bin;
    }

//	qDebug() << "PlayerID::Player: m_player_bin:" << m_player_bin << "filename:" << bin_name;


	if (bin_name.toLower().startsWith("mplayer")) {
		p = MPLAYER;
	} else {
		p = MPV;
	}
    */
    Player p;
    if (player_bin.toLower().contains("mplayer")) {
        p = MPLAYER;
    } else {
        p = MPV;
    }

	return p;
}
