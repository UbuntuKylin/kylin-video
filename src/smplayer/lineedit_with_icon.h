/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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

#ifndef LINEEDIT_WITH_ICON_H
#define LINEEDIT_WITH_ICON_H

#include <QLineEdit>

class QToolButton;

class LineEditWithIcon : public QLineEdit
{
    Q_OBJECT

public:
    LineEditWithIcon(QWidget *parent = 0);

	void setIcon(const QPixmap & pixmap);

protected:
    void resizeEvent(QResizeEvent *);
	virtual void setupButton();

protected:
    QToolButton *button;
};

#endif

