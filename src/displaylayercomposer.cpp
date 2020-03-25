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

#include "displaylayercomposer.h"

#include <QPainter>

DisplayLayerComposer::DisplayLayerComposer(QWidget* parent, Qt::WindowFlags f)
    : AutoHideCursorWidget(parent, f)
    , m_repaintBackground(false)
    , m_playing(false)
{
#if QT_VERSION < 0x050000
    setAttribute(Qt::WA_OpaquePaintEvent);
    #if QT_VERSION >= 0x040400
    setAttribute(Qt::WA_NativeWindow);
    #endif
    setAttribute(Qt::WA_PaintUnclipped);
#endif
}

DisplayLayerComposer::~DisplayLayerComposer()
{

}

void DisplayLayerComposer::setRepaintBackground(bool b)
{
    m_repaintBackground = b;
}

void DisplayLayerComposer::paintEvent(QPaintEvent * e)
{
    if (!m_playing) {//if m_repaintBackground is true, Qt5 will call "QPainter::begin: Paint device returned engine == 0, type: 1"
        QPainter painter(this);
        //painter.drawRoundedRect(e->rect(), 6, 6);
        painter.eraseRect(e->rect());//painter.fillRect(e->rect(), QColor(255,0,0));

//        QPainterPath painterPath;
//        painterPath.addRoundedRect(e->rect(), 6, 6);
//        painter.drawPath(painterPath);
    }
}

void DisplayLayerComposer::playingStarted()
{
    repaint();
    m_playing = true;
//    setAttribute(Qt::WA_PaintOnScreen);//WA_PaintOnScreen该属性设置会导致播放音频文件时界面不刷新，此时如果显示或隐藏播放列表，则播放列表重影
    setAttribute(Qt::WA_NativeWindow, true);
    AutoHideCursorWidget::playingStarted();
}

void DisplayLayerComposer::playingStopped()
{
    m_playing = false;
    setAttribute(Qt::WA_NativeWindow, false);
    repaint();
    AutoHideCursorWidget::playingStopped();
}
