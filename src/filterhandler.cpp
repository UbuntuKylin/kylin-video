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

#include "filterhandler.h"
#include "mainwindow.h"

#include <QApplication>
#include <QEvent>

FilterHandler::FilterHandler(MainWindow &gui, QObject &obj) : QObject(&gui),
   m_mainWindow(&gui)
{
    obj.installEventFilter(this);
    qApp->installEventFilter(this);
}

FilterHandler::~FilterHandler()
{

}

bool FilterHandler::eventFilter(QObject *obj, QEvent *event)
{
    (void) obj;

    /*if (event->type() == QEvent::WindowStateChange) {// MainWindow::event
        Qt::WindowStates winState = m_mainWindow->windowState();
        switch(winState)
        {
        case Qt::WindowMinimized:
        case Qt::WindowMaximized:
        case Qt::WindowFullScreen:
            break;
        }
        return false;
    }
    if (event->type() == QEvent::KeyPress) {// MainWindow::keyPressEvent
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        switch (e->key())
        {
        case Qt::Key_Space:
            break;
        case Qt::Key_Escape:
            break;
        default: return false;
        }

        event->accept();
        return true;
    }*/

    if (event->type() == QEvent::MouseMove) {
        emit mouseMoved();
    }

    return false;
}
