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

#include "translator.h"
#include "paths.h"
#include <QTranslator>
#include <QLocale>
#include <QApplication>
#include <QDebug>

Translator::Translator()
{
    qApp->installTranslator(&app_trans);
    qApp->installTranslator(&app_qt_trans); // lc add 20201015
    qApp->installTranslator(&qt_trans);
}

Translator::~Translator()
{
}

bool Translator::loadCatalog(QTranslator & t, QString name, QString locale, QString dir)
{
    QString s = name + "_" + locale + ".qm"; //.toLower();
	bool r = t.load(s, dir);
    if (!r) {
		qDebug("Translator::loadCatalog: can't load %s from %s", s.toUtf8().data(), dir.toUtf8().data());
    }
	return r;
}

void Translator::load(const QString &snap)
{
    QString locale = QLocale::system().name();

	QString trans_path = Paths::translationPath();

    QString qt_trans_path;
    if (!snap.isEmpty()) {
        qt_trans_path = QString("%1%2").arg(snap).arg(Paths::qtTranslationPath());
    }
    else {
        qt_trans_path = Paths::qtTranslationPath();
    }

    loadCatalog(qt_trans, "qt", locale, qt_trans_path);
    loadCatalog(app_trans, "kylin-video", locale, trans_path);
    loadCatalog(app_qt_trans, "qtbase", locale, trans_path); // lc add 20201015
}
