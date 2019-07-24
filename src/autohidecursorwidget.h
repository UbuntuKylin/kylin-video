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

#ifndef AUTOHIDECURSORWIDGET_H
#define AUTOHIDECURSORWIDGET_H

#include <QWidget>
//#include <QSize>
#include <QPoint>
#include <QMouseEvent>

class QLabel;
class QTimer;

class AutoHideCursorWidget : public QWidget
{
    Q_OBJECT

public:
    AutoHideCursorWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~AutoHideCursorWidget();

    void setAutoHideCursor(bool b);

public slots:
    virtual void playingStarted();
    virtual void playingStopped();

signals:
    void mouseMoved(QPoint);

protected:
    virtual void mouseMoveEvent(QMouseEvent * e);

protected slots:
    virtual void checkMousePos();

private:
    QTimer *m_checkMouseTimer = nullptr;
    QPoint m_mouseLastPosition;
    bool m_autohideCursor;
    int autohide_interval;
};

#endif // AUTOHIDECURSORWIDGET_H
