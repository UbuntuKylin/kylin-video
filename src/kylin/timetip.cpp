/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "timetip.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

TimeTip::TimeTip(const QString &text, QWidget *parent) : QFrame(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    repaint_flag = false;

    text_frame = new QFrame();
    text_frame->setContentsMargins(0, 0, 0, 0);
    text_frame->adjustSize();
    QHBoxLayout *textlayout = new QHBoxLayout(this->text_frame);
    textlayout->setContentsMargins(10, 2, 10, 0);
//    textlayout->setContentsMargins(0, 0, 0, 0);
    textlayout->setSpacing(0);

    text_label = new QLabel(text);
    text_label->setObjectName("WhiteTipText");
    text_label->setAlignment(Qt::AlignCenter);

    split_line = new QLabel;
    split_line->setObjectName("SplitText");
    split_line->setFixedWidth(1);
//    split_line->setFixedSize(1,20);

    textlayout->addWidget(text_label, 0, Qt::AlignVCenter);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSpacing(0);
    vlayout->addWidget(text_frame, 0, Qt::AlignHCenter);
    vlayout->addWidget(split_line, 0, Qt::AlignHCenter);
    layout->addLayout(vlayout);

    hide();
}

TimeTip::~TimeTip()
{

}

void TimeTip::setText(const QString text)
{
    repaint_flag = false;
    this->text_label->setText(text);
}


void TimeTip::setPixMapAndTime(QPixmap pixmap, const QString time)
{
    repaint_flag = true;
    // Add current time text
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect(), Qt::AlignHCenter | Qt::AlignBottom, time);
    this->text_label->setPixmap(pixmap);
}

void TimeTip::paintEvent(QPaintEvent *event)
{
    if (!repaint_flag) {
        QFrame::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    double w_pen = 2.0;
    QBrush background(QColor(255, 255, 255, 70));
    QColor borderColor = QColor(0, 0, 0, 0.2 * 255);
    double margin = 9.0;
    QMarginsF shadow_margins = QMarginsF(margin, 0, margin, 26);

    //background
    QRectF bg_rect = QRectF(rect()).marginsRemoved(shadow_margins);
    QPainterPath bg_path;
    bg_path.addRoundedRect(bg_rect, 2, 2);
    painter.fillPath(bg_path, background);

    //border
    QPainterPath border_path;
    QRectF border_rect = QRectF(rect());
    int border_radius = 4;
    QMarginsF border_margin(w_pen / 2, w_pen / 2, w_pen / 2, w_pen / 2);

    border_radius += w_pen / 2;
    border_rect = border_rect.marginsAdded(border_margin).marginsRemoved(shadow_margins);

    border_path.addRoundedRect(border_rect, border_radius, border_radius);
    QPen border_pen(borderColor);
    border_pen.setWidthF(w_pen);
    painter.strokePath(border_path, border_pen);
}
