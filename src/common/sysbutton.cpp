/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
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

#include "sysbutton.h"
#include <QDebug>

SysButton::SysButton(QWidget *parent) :
    QPushButton(parent)
{
    status = NORMAL;
    mouse_press = false;
}


void SysButton::loadPixmap(QString pic_name)
{
    pixmap = QPixmap(pic_name);
    btn_width = pixmap.width()/3;
    btn_height = pixmap.height();
    this->setFixedSize(btn_width, btn_height);
}

void SysButton::enterEvent(QEvent *)
{
    status = ENTER;
    update();
}

void SysButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
        status = PRESS;
        update();
    }
}

void SysButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(mouse_press  && this->rect().contains(event->pos()))
    {
        mouse_press = false;
        status = ENTER;
        update();
        emit clicked();
    }
}

void SysButton::leaveEvent(QEvent *)
{
    status = NORMAL;
    update();
}

void SysButton::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(this->rect(), pixmap.copy(btn_width * status, 0, btn_width, btn_height));
    painter.end();
}
