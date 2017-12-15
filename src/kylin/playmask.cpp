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

#include "playmask.h"
#include <QTime>
#include <QWindowStateChangeEvent>
#include <QHBoxLayout>
#define WAIT_TIME_TO_MAXIMIZE_OVERLAY_MS 300
#include <QPainter>
#include <QDebug>
#include <QStyleOption>

PlayMask::PlayMask(QWidget *parent)
    : QWidget(parent, Qt::SubWindow)
{
	setWindowFlags(windowFlags() | Qt::SubWindow);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
//    setTransparent(true);

    setFixedSize(172, 172);
	
    play_Btn = new QPushButton(this);
    play_Btn->setObjectName("PlayMaskBtn");
    play_Btn->setFocusPolicy(Qt::NoFocus);
    play_Btn->setFixedSize(172, 172);
	QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(play_Btn);
    layout->setMargin(0);
	this->setLayout(layout);


    connect(play_Btn, SIGNAL(released()), this, SIGNAL(signal_play_continue()));
}

PlayMask::~PlayMask()
{

}

void PlayMask::setTransparent(bool transparent)
{
	if (transparent)
	{
		setAttribute(Qt::WA_TranslucentBackground);
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
		set_widget_opacity(0.5);
	}
	else
	{
		setAttribute(Qt::WA_TranslucentBackground,false);
		setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	}
}

void PlayMask::set_widget_opacity(const float &opacity)
{
	QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
	this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void PlayMask::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}
