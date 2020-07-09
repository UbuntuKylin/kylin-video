/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "systembutton.h"
#include <QDebug>

SystemButton::SystemButton(QWidget *parent) :
    QPushButton(parent)
    , m_alignOff(0)
{
    this->setMouseTracking(false);
    status = NORMAL;
    mouse_press = false;
}

SystemButton::SystemButton(bool singleIcon, QWidget *parent)
    :SystemButton(parent)
{
    this->m_singleIcon = singleIcon;
}

void SystemButton::loadPixmap(QString pic_name, int w, int h, int alignOff)
{
    m_alignOff = alignOff;
    pixmap = QPixmap(pic_name);
    if (this->m_singleIcon) {
        btn_width = pixmap.width();
    }
    else {
        btn_width = pixmap.width()/3;
    }
    btn_height = pixmap.height();

    if (m_alignOff == 0) {
        this->setFixedSize(btn_width, btn_height);
    }
    else {
        this->setFixedSize(w, h);
    }
}

void SystemButton::enterEvent(QEvent *)
{
    status = ENTER;
    update();
}

void SystemButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
        status = PRESS;
        update();
    }
}

void SystemButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(mouse_press  && this->rect().contains(event->pos()))
    {
        mouse_press = false;
        status = ENTER;
        update();
        emit clicked();
    }
}

void SystemButton::leaveEvent(QEvent *)
{
    status = NORMAL;
    update();
}

void SystemButton::paintEvent(QPaintEvent *)
{
    QPainter painter;

    painter.begin(this);
    if (this->m_singleIcon) {
        QRect r(m_alignOff, m_alignOff, this->rect().width() - m_alignOff*2, this->rect().height() - m_alignOff*2);
        painter.drawPixmap(r, pixmap.copy(0, 0, btn_width, btn_height));
    }
    else {
        painter.drawPixmap(this->rect(), pixmap.copy(btn_width * status, 0, btn_width, btn_height));
    }
    if (status != NORMAL) {
        //painter.fillRect(this->rect(), QColor("#3253bc"));
        QPainterPath path;
        path.addRoundedRect(QRectF(rect()), 2, 2);
        painter.setOpacity(0.1);
        painter.fillPath(path, QColor("#2bb6ea"));
    }

    painter.end();
}
