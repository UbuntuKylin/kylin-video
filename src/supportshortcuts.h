/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    lixiang@kylinos.cn/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SUPPORTSHORTCUTS_H_
#define _SUPPORTSHORTCUTS_H_

#include "ui_supportshortcuts.h"
#include "../smplayer/preferences.h"

class SupportShortcuts : public QWidget, public Ui::SupportShortcuts
{
	Q_OBJECT

public:
    SupportShortcuts( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~SupportShortcuts();

    void setData(Preferences * pref);

protected:
	virtual void retranslateStrings();
};

#endif
