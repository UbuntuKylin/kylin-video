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

#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H

#include <QWidget>

class TitleButton : public QWidget
{
    Q_OBJECT
public:
    explicit TitleButton(int id, bool bigFont, const QString &title, QWidget *parent = 0);

    int id() const;
    void setId(const int &id);

    bool isActived() const;
    void setActived(bool isActived);

signals:
    void clicked(int id);

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    bool m_bigFont;
    bool m_isActived;
    int m_id;
    QString m_title;
};

#endif // TITLEBUTTON_H
