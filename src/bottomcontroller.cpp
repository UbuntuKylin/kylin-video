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

#include "bottomcontroller.h"

#include <QTimer>

BottomController::BottomController(QObject *parent) : QObject(parent)
   , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(3 * 1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

BottomController::~BottomController()
{
    if (m_timer) {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
        if(m_timer->isActive()) {
            m_timer->stop();
        }
        delete m_timer;
        m_timer = nullptr;
    }
}

void BottomController::temporaryShow()
{
    if (m_timer->isActive()) {
        m_timer->start();
        return;
    }

    m_timer->start();
    emit requestShow();
}

void BottomController::permanentShow()
{
    m_timer->stop();

    emit requestShow();
}

void BottomController::onTimeout()
{
    m_timer->stop();

    emit requestHide();
}
