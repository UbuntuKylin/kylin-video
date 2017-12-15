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

#ifndef _MESSAGEDIALOG_H_
#define _MESSAGEDIALOG_H_

#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <QDialogButtonBox>

class QPushButton;
class QPoint;

enum MDragState {NOT_MDRAGGING, START_MDRAGGING, MDRAGGING};

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
    QLabel *title_label;
    QLabel *icon_label;
    QLabel *msg_label;
    QPushButton *close_Btn;
//    QGridLayout *gridLayout;
    QVBoxLayout *main_layout;
    QDialogButtonBox *buttonBox;
    QAbstractButton *clickedBtn;
    QAbstractButton *defaultBtn;
    MDragState drag_state;
    QPoint start_drag;
};

#endif
