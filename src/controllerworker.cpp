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

#include "controllerworker.h"

#include <QApplication>
#include <QTimer>
#include <QDebug>

ControllerWorker::ControllerWorker()
{

}

ControllerWorker::~ControllerWorker()
{

}

void ControllerWorker::playPause()
{
    emit this->requestPlayPause();
}

bool ControllerWorker::seek_forward(int seconds)
{
    qDebug() << "seek_forward: " << seconds;
    emit this->requestSeekForward(seconds);
    return true;
}

bool ControllerWorker::seek_rewind(int seconds)
{
    qDebug() << "seek_rewind: " << seconds;
    emit this->requestSeekRewind(seconds);
    return true;
}

void ControllerWorker::stop()
{
    emit this->requestStop();
}

void ControllerWorker::quit()
{
    QTimer::singleShot(0, QApplication::instance(), SLOT(quit()));
}
