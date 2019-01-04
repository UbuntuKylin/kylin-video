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


#include "global.h"
#include "preferences.h"

#include <QSettings>
#include "translator.h"
#include "paths.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

QSettings * Global::settings = 0;
Preferences * Global::pref = 0;
Translator * Global::translator = 0;

using namespace Global;

void Global::global_init(const QString &arch) {
//    qDebug("global_init");

	// Translator
    translator = new Translator();

	// settings
    QString filename = Paths::iniPath() + "/kylin-video.ini";
    settings = new QSettings(filename, QSettings::IniFormat );
    settings->setIniCodec("UTF-8");//add by kobe

	// Preferences
    pref = new Preferences(arch);
}

void Global::global_end() {
//	qDebug("global_end");

	// delete
	delete pref;
	pref = 0;

	delete settings;
    delete translator;
}
