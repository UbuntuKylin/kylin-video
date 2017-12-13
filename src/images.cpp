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

#include "images.h"
#include <QFile>
#include <QDebug>
#include <QResource>

#include "global.h"
#include "preferences.h"
#include "paths.h"
using namespace Global;

QString Images::current_theme;
QString Images::themes_path;

QString Images::last_resource_loaded;
bool Images::has_rcc = false;

QString Images::resourceFilename() {
	QString filename = QString::null;

	if ((!themes_path.isEmpty()) && (!current_theme.isEmpty())) {
		filename = themes_path +"/"+ current_theme +"/"+ current_theme +".rcc";
	}

	qDebug() << "Images::resourceFilename:" << filename;

	return filename;
}

QString Images::file(const QString & name) {
    QString icon_name;
    icon_name = ":/res/" + name + ".png";
    return icon_name;
}


QPixmap Images::icon(QString name, int size) {
    QString icon_name = file(name);
    QPixmap p(icon_name);

    if (!p.isNull()) {
        if (size != -1) {
            p = resize(&p, size);
        }
    }

    return p;
}

QPixmap Images::resize(QPixmap *p, int size) {
	return QPixmap::fromImage( (*p).toImage().scaled(size,size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation) );
}

QPixmap Images::flip(QPixmap *p) {
	return QPixmap::fromImage( (*p).toImage().mirrored(true, false) );
}

QPixmap Images::flippedIcon(QString name, int size) {
	QPixmap p = icon(name, size);
	p = flip(&p);
	return p;
}
