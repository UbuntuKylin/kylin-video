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

#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include <QWidget>
#include <QMutex>

class QMovie;
class QLabel;

class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget *parent = 0);
    ~MaskWidget();
    static MaskWidget *Instance()
    {
        static QMutex mutex;
        if (!self) {
            QMutexLocker locker(&mutex);
            if (!self) {
                self = new MaskWidget;
            }
        }
        return self;
    }

    void showMask();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent* event) Q_DECL_OVERRIDE;

private:
    static MaskWidget *self;
    QMovie *m_movie = nullptr;
    QLabel *m_iconLabel = nullptr;
    QLabel *m_textLabel = nullptr;
};

#endif // MASKWIDGET_H
