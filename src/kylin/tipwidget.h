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

#ifndef _TIPWIDGET_H_
#define _TIPWIDGET_H_

#include <QFrame>
class QLabel;

class TipWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
public:
    explicit TipWidget(const QString &text, QWidget *parent = 0);
    ~TipWidget();
    void setBackgroundImage(const QPixmap &srcPixmap);

    int radius() const;
    QColor borderColor() const;
    QBrush background() const;

    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity);

public slots:
    void setText(const QString text);

    void setBackground(QBrush background);
    void setRadius(int radius);
    void setBorderColor(QColor borderColor);

    void aniFinished();

private:
    QLabel          *text_label;

    QBrush          m_background;
    int             m_radius;
    int             w_shadow;
    QMargins        shadow_margins;
    QColor          m_borderColor;
};

#endif
