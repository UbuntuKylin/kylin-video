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

#ifndef _FILTERHANDLER_H_
#define _FILTERHANDLER_H_

#include <QObject>

class MainWindow;

class FilterHandler : public QObject
{
    Q_OBJECT

public:
    FilterHandler(MainWindow &gui, QObject &obj);
    ~FilterHandler();

signals:
    void mouseMoved();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    MainWindow *m_mainWindow = nullptr;
};

#endif // _FILTERHANDLER_H_
