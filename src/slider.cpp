/*
 * Copyright (C) 2013 ~ 2020 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "slider.h"

#include <QMouseEvent>

Slider::Slider(QWidget *parent)
    : QSlider(parent)
{

}

Slider::Slider(Qt::Orientation orientation, QWidget *parent)
    :QSlider(orientation, parent)
{

}

Slider::~Slider()
{

}

void Slider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        double value = 0;
        if (orientation() == Qt::Vertical) {
            value = ((this->maximum() - this->minimum()) * (this->height()- event->y()) * 1.0) / this->height();
        }
        else {
            value = ((this->maximum() - this->minimum()) * event->x() * 1.0) / this->width();
        }
        this->setValue(this->minimum() + qRound(value)) ;

        event->accept();
    }

    QSlider::mousePressEvent(event);
}
