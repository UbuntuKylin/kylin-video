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

#ifndef PLAYMASK_H
#define PLAYMASK_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QPaintEvent>

class PlayMask : public QWidget
{
	Q_OBJECT

public:
    PlayMask(QWidget *parent = 0);
    ~PlayMask();

	void setTransparent(bool transparent);
	void set_widget_opacity(const float &opacity=0.8);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void signal_play_continue();

private:
    QPushButton *play_Btn;
};

#endif // PLAYMASK_H
