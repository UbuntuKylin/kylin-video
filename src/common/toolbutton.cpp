/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
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

#include "toolbutton.h"

ToolButton::ToolButton(const QString &pic_name, const QString &text, QWidget *parent)
    :QToolButton(parent)
{
    /*//设置文本颜色
    QPalette text_palette = palette();
    text_palette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
    setPalette(text_palette);

    //设置文本粗体
    QFont &text_font = const_cast<QFont &>(font());
    text_font.setWeight(QFont::Bold);

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    //设置图标
    QPixmap pixmap(pic_name);
    setIcon(pixmap);
    setIconSize(pixmap.size());

    //设置大小
    setFixedSize(pixmap.width()+25, pixmap.height()+25);
    setAutoRaise(true);
    setObjectName("transparentToolButton");*/


    setDown(false);
    setFocusPolicy(Qt::NoFocus);

    this->setText(text);

    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setPopupMode(QToolButton::InstantPopup);

    QPixmap pixmap(pic_name);
    setIcon(pixmap);
    setIconSize(pixmap.size());
    setAutoRaise(true);
    this->setObjectName("toolButton");

    is_over = false;
    is_press = false;
}

ToolButton::~ToolButton()
{

}

void ToolButton::enterEvent(QEvent *)
{
    is_over = true;
}

void ToolButton::leaveEvent(QEvent *)
{
    is_over = false;
}

void ToolButton::mousePressEvent(QMouseEvent *event)
{
    QToolButton::mousePressEvent(event);//注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    if(event->button() == Qt::LeftButton)
    {
//        emit clicked();
    }
}

void ToolButton::setMousePress(bool mouse_press)
{
    this->is_press = mouse_press;
    update();
}

void ToolButton::paintEvent(QPaintEvent *event)
{
    if(is_over)
    {
        //绘制鼠标移到按钮上的按钮效果
        painterInfo(0, 100, 150);
    }
    else
    {
        if(is_press)
        {
            painterInfo(0, 100, 150);
        }
    }

    QToolButton::paintEvent(event);//0522
}

void ToolButton::painterInfo(int top_color, int middle_color, int bottom_color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
    linear.setColorAt(0, QColor(230, 230, 230, top_color));
    linear.setColorAt(0.5, QColor(230, 230, 230, middle_color));
    linear.setColorAt(1, QColor(230, 230, 230, bottom_color));
    painter.setBrush(linear);
    painter.drawRect(rect());
}
