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

#include "colorutils.h"
#include <QWidget>

//QString ColorUtils::colorToRRGGBBAA(unsigned int color) {
//	QColor c;
//	c.setRgb( color );

//	QString s;
//	return s.sprintf("%02x%02x%02x00", c.red(), c.green(), c.blue() );
//}

//QString ColorUtils::colorToRRGGBB(unsigned int color) {
//	QColor c;
//	c.setRgb( color );

//	QString s;
//	return s.sprintf("%02x%02x%02x", c.red(), c.green(), c.blue() );
//}

//QString ColorUtils::colorToRGB(unsigned int color) {
//	QColor c;
//	c.setRgb( color );

//	QString s;
//	return s.sprintf("0x%02x%02x%02x", c.blue(), c.green(), c.red() );
//}

//QString ColorUtils::colorToAABBGGRR(unsigned int color) {
//	QColor c;
//	c.setRgb( color );

//	QString s;
//	return s.sprintf("00%02x%02x%02x", c.blue(), c.green(), c.red() );
//}

QString ColorUtils::colorToRGB(QColor c) {
    QString s;
    return s.sprintf("0x%02x%02x%02x", c.blue(), c.green(), c.red() );
}

QString ColorUtils::colorToRRGGBBAA(QColor c) {
    QString s;
    return s.sprintf("%02x%02x%02x%02x", c.red(), c.green(), c.blue(), c.alpha());
}

QString ColorUtils::colorToRRGGBB(QColor c) {
    QString s;
    return s.sprintf("%02x%02x%02x", c.red(), c.green(), c.blue());
}

QString ColorUtils::colorToAABBGGRR(QColor c) {
    QString s;
    return s.sprintf("%02x%02x%02x%02x", 255 - c.alpha(), c.blue(), c.green(), c.red() );
}

QString ColorUtils::colorToAARRGGBB(QColor c) {
    QString s;
    return s.sprintf("%02x%02x%02x%02x", c.alpha(), c.red(), c.green(), c.blue());
}

QColor ColorUtils::AARRGGBBToColor(const QString & s) {
    QRegExp rx("([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})");
    if (rx.indexIn(s) > -1 && rx.captureCount() == 4) {
        //qDebug() << "ColorUtils::AARRGGBBToColor:" << rx.cap(1) << rx.cap(2) << rx.cap(3) << rx.cap(4);
        uint alpha = rx.cap(1).toUInt(0, 16);
        uint red = rx.cap(2).toUInt(0, 16);
        uint green = rx.cap(3).toUInt(0, 16);
        uint blue = rx.cap(4).toUInt(0, 16);
        QColor c(red, green, blue, alpha);
        return c;
    }
    return QColor();
}

void ColorUtils::setForegroundColor(QWidget * w, const QColor & color) {
	QPalette p = w->palette(); 
	p.setColor(w->foregroundRole(), color); 
	w->setPalette(p);
}

void ColorUtils::setBackgroundColor(QWidget * w, const QColor & color) {
	QPalette p = w->palette(); 
    p.setColor(w->backgroundRole(), color);
	w->setPalette(p);
}

QString ColorUtils::stripColorsTags(QString s) {
    QRegExp rx_console_colors("\033\\[\\d\\d?;\\d\\d?m");
    int removePos = rx_console_colors.lastIndexIn(s);
    removePos += rx_console_colors.matchedLength();
    return s.remove(0, removePos);
}
