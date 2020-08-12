/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
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

#include "autohidecursorwidget.h"

#include <QCursor>
#include <QTimer>

AutoHideCursorWidget::AutoHideCursorWidget(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f )
    , m_checkMouseTimer(new QTimer(this))
    , m_mouseLastPosition(QPoint(0,0))
    , m_autohideCursor(false)
    , autohide_interval(1000)
{
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setMinimumSize(QSize(0,0));

    this->setAutoHideCursor(false);

    connect(m_checkMouseTimer, SIGNAL(timeout()), this, SLOT(checkMousePos()));
}

AutoHideCursorWidget::~AutoHideCursorWidget()
{

}

void AutoHideCursorWidget::setAutoHideCursor(bool b)
{
    m_autohideCursor = b;
    if (m_autohideCursor) {
        m_checkMouseTimer->setInterval(autohide_interval);
        m_checkMouseTimer->start();
    } else {
        m_checkMouseTimer->stop();
    }
}

void AutoHideCursorWidget::checkMousePos()
{
    if (!m_autohideCursor) {
        setCursor(QCursor(Qt::ArrowCursor));
        return;
    }

    QPoint pos = mapFromGlobal(QCursor::pos());
    if (m_mouseLastPosition != pos) {
        setCursor(QCursor(Qt::ArrowCursor));
    } else {
        setCursor(QCursor(Qt::BlankCursor));
    }
    m_mouseLastPosition = pos;
}

void AutoHideCursorWidget::mouseMoveEvent(QMouseEvent * e)
{
    emit mouseMoved(e->pos());

    if (cursor().shape() != Qt::ArrowCursor) {
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void AutoHideCursorWidget::playingStarted()
{
    setAutoHideCursor(true);
}

void AutoHideCursorWidget::playingStopped()
{
    setAutoHideCursor(false);

    if (cursor().shape() != Qt::ArrowCursor) {
        setCursor(QCursor(Qt::ArrowCursor));
    }
}
