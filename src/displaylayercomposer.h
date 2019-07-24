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

#ifndef DISPLAYLAYERCOMPOSER_H
#define DISPLAYLAYERCOMPOSER_H

#include <QWidget>
#include <QPaintEvent>

#include "autohidecursorwidget.h"

class DisplayLayerComposer : public AutoHideCursorWidget
{
    Q_OBJECT

public:
    DisplayLayerComposer(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~DisplayLayerComposer();

    void setRepaintBackground(bool b);

public slots:
    virtual void playingStarted();
    virtual void playingStopped();

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    bool m_repaintBackground;
    bool m_playing;
};

#endif // DISPLAYLAYERCOMPOSER_H
