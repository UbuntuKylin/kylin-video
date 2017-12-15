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

#include "titlebutton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

namespace {
const int LEFT_WIDTH = 2;
const int TITLE_LM_BIG = 10;
const int TITLE_LM_NORMAL = 20;
const int FONT_SIZE_BIG = 14;
const int FONT_SIZE_NORMAL = 14;
const QColor FONT_COLOR_ACTIVED = QColor("#ffffff");
const QColor FONT_COLOR_NORMAL = QColor("#999999");
const QColor LEFT_COLOR_ACTIVED = QColor("#0a9ff5");
const QColor RIGHT_COLOR_ACTIVED = QColor("#1f1f1f");
}

TitleButton::TitleButton(int id, bool bigFont, const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_bigFont(bigFont)
    , m_isActived(false)
    , m_id(id)
    , m_title(title)
{
    setFixedHeight(45);
    this->setStyleSheet("QWidget{border:none;}");
}

void TitleButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        emit this->clicked(m_id);
        e->accept();
    }
}

void TitleButton::paintEvent(QPaintEvent *e)
{
    const QColor lbc = isActived() ? LEFT_COLOR_ACTIVED : QColor("#2e2e2e");//没有选中时，让其左侧2px宽度的区域颜色和title_widget背景颜色一致
    const QColor rbc = isActived() ? RIGHT_COLOR_ACTIVED : QColor("#2e2e2e");
    const QRect lr(0, 0, LEFT_WIDTH, height());
    const QRect rr(LEFT_WIDTH, 0, width() - LEFT_WIDTH, height());

    const QRect bfr(TITLE_LM_BIG, (height() - FONT_SIZE_BIG) / 4,
                    width() - TITLE_LM_BIG, height());
    const QRect nfr(TITLE_LM_NORMAL, (height() - FONT_SIZE_NORMAL) / 3,
                    width() - TITLE_LM_NORMAL, height());

    // background
    QPainter painter(this);
    painter.fillRect(lr, lbc);
    if (isActived()) {
        painter.fillRect(rr, rbc);
    }

    // title
    QFont f;
    if (m_bigFont)
        f.setWeight(700);
    f.setPixelSize(m_bigFont ? FONT_SIZE_BIG : FONT_SIZE_NORMAL);
    QPen p(isActived() ? FONT_COLOR_ACTIVED : FONT_COLOR_NORMAL);
    painter.setFont(f);
    painter.setPen(p);
    painter.drawText(m_bigFont ? bfr : nfr, m_title);
//    painter.drawText(rect(), Qt::AlignCenter, "Kobe");
}

bool TitleButton::isActived() const
{
    return m_isActived;
}

void TitleButton::setActived(bool isActived)
{
    m_isActived = isActived;
    this->update();
}

int TitleButton::id() const
{
    return m_id;
}

void TitleButton::setId(const int &id)
{
    m_id = id;
}
