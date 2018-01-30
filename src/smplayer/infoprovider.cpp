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

#include "infoprovider.h"
#include "global.h"
#include "preferences.h"
#include "playerprocess.h"
#include "playerid.h"
#include <QFileInfo>

MediaData InfoProvider::getInfo(QString mplayer_bin, QString filename) {
	qDebug("InfoProvider::getInfo: %s", filename.toUtf8().data());

	QFileInfo fi(mplayer_bin);
	if (fi.exists() && fi.isExecutable() && !fi.isDir()) {
		mplayer_bin = fi.absoluteFilePath();
	}

	PlayerProcess * proc = PlayerProcess::createPlayerProcess(mplayer_bin, 0);

	proc->setExecutable(mplayer_bin);
	proc->setFixedOptions();
	proc->setOption("frames", "1");
	proc->setOption("vo", "null");
	proc->setOption("ao", "null");
	proc->setMedia(filename);

	QString commandline = proc->arguments().join(" ");
	qDebug("InfoProvider::getInfo: command: '%s'", commandline.toUtf8().data());

	proc->start();
	if (!proc->waitForFinished()) {
		qWarning("InfoProvider::getInfo: process didn't finish. Killing it...");
		proc->kill();
	}

	MediaData md = proc->mediaData();
	delete proc;

	return md;
}

MediaData InfoProvider::getInfo(QString filename) {
	return getInfo( Global::pref->mplayer_bin, filename );
}
