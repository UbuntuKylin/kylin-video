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

#ifndef _MESSAGEDIALOG_H_
#define _MESSAGEDIALOG_H_

#include "utils.h"

#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <QDialogButtonBox>

class QPushButton;
class QPoint;


class MessageDialog : public QDialog
{
	Q_OBJECT

public:
    MessageDialog(QWidget *parent = 0, const QString &title = "", const QString &text = "", QMessageBox::StandardButtons buttons = QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::StandardButton defaultButton = QMessageBox::Ok);
    ~MessageDialog();

    QAbstractButton *clickedButton() const;
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
//    void setDefaultButton(QPushButton *button);
//    void setDefaultButton(QMessageBox::StandardButton button);
    void setIcon(const QString &icon);
    void setDialogSize(int w, int h);

    void initConnect();
    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

private slots:
    void onButtonClicked(QAbstractButton *button);

private:
    int returnCodeByRun(QAbstractButton *button);

private:
    QWidget *m_centerWidget;
    QLabel *title_label = nullptr;
    QLabel *icon_label = nullptr;
    QLabel *msg_label = nullptr;
    QPushButton *close_Btn = nullptr;
//    QGridLayout *gridLayout;
    QVBoxLayout *main_layout = nullptr;
    QDialogButtonBox *buttonBox = nullptr;
    QAbstractButton *clickedBtn = nullptr;
    QAbstractButton *defaultBtn = nullptr;
    DragState m_dragState;
    QPoint m_startDrag;
};

#endif
