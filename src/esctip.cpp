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

#include "esctip.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

EscTip::EscTip(QWidget *parent) : QFrame(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(0, 0, 0, 0);
    setObjectName("EscTip");//设置背景色

    m_radius = 4;
    m_shadow = 20;
    m_shadowMargins = QMargins(20, 20, 20, 20);
    m_borderColor = QColor(0, 0, 0, 0.2 * 255);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_textLabel = new QLabel(this);
    m_textLabel->adjustSize();
    m_textLabel->setStyleSheet("QLabel{font-size: 16px;color: #ffffff;}");
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setText(tr("Press ESC to exit full screen mode"));

    layout->addStretch();
    layout->addWidget(m_textLabel);
    layout->addStretch();
    this->setLayout(layout);

    hide();
}

EscTip::~EscTip()
{

}

QBrush EscTip::background() const
{
    return this->m_background;
}

int EscTip::radius() const
{
    return this->m_radius;
}

QColor EscTip::borderColor() const
{
    return this->m_borderColor;
}

void EscTip::setBackground(QBrush background)
{
    this->m_background = background;
}

void EscTip::setRadius(int radius)
{
    this->m_radius = radius;
}

void EscTip::setBorderColor(QColor borderColor)
{
    this->m_borderColor = borderColor;
}

void EscTip::aniFinished()
{
    this->hide();
}

void EscTip::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

    bool outer = true;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    int radius = this->m_radius;
    double w_pen = 1.0;
    QBrush background =  this->m_background;
    QColor border_color = this->m_borderColor;
    double margin = 2.0;
    QMarginsF m_shadowMargins = QMarginsF(margin, margin, margin, margin);

    //background
    QRectF bg_rect = QRectF(rect()).marginsRemoved(m_shadowMargins);
    QPainterPath bg_path;
    bg_path.addRoundedRect(bg_rect, radius, radius);
    painter.fillPath(bg_path, background);

    //border
    QPainterPath border_path;
    QRectF border_rect = QRectF(rect());
    int border_radius = radius;
    QMarginsF border_margin(w_pen / 2, w_pen / 2, w_pen / 2, w_pen / 2);
    if (outer) {
        border_radius += w_pen / 2;
        border_rect = border_rect.marginsAdded(border_margin).marginsRemoved(m_shadowMargins);
    } else {
        border_radius -= w_pen / 2;
        border_rect = border_rect.marginsRemoved(border_margin).marginsRemoved(m_shadowMargins);
    }
    border_path.addRoundedRect(border_rect, border_radius, border_radius);
    QPen border_pen(border_color);
    border_pen.setWidthF(w_pen);
    painter.strokePath(border_path, border_pen);
}
