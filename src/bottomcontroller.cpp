/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "bottomcontroller.h"

#include <QTimer>

BottomController::BottomController(QObject *parent) : QObject(parent)
   , timer(new QTimer(this))
{
    timer->setSingleShot(true);
    timer->setInterval(3 * 1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

BottomController::~BottomController()
{
    if (timer) {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
        if(timer->isActive()) {
            timer->stop();
        }
        delete timer;
        timer = nullptr;
    }
}

void BottomController::temporaryShow()
{
    if (timer->isActive()) {
        timer->start();
        return;
    }

    timer->start();
    emit requestShow();
}

void BottomController::permanentShow()
{
    timer->stop();

    emit requestShow();
}

void BottomController::onTimeout()
{
    timer->stop();

    emit requestHide();
}
