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

#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_

#include "ui_aboutdialog.h"

#include <QDialog>
#include <QPushButton>
class QParallelAnimationGroup;

enum ADragState {NOT_ADRAGGING, START_ADRAGGING, ADRAGGING};
enum TabState {TAB_ABOUT, TAB_CONTRIBUTOR};

class AboutDialog : public QDialog, public Ui::AboutDialog
{
	Q_OBJECT

public:
    AboutDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~AboutDialog();

    void initConnect();
    void initAnimation();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

    void setVersions();

public slots:
    void onAboutBtnClicked();
    void onContributorBtnClicked();

private:
    QPushButton *okBtn;
    QParallelAnimationGroup *aboutGroup;
    QParallelAnimationGroup *contributorGroup;
    ADragState drag_state;
    TabState tab_state;
    QPoint start_drag;
};

#endif
