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

#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QPointer>

#include "systembutton.h"
#include "utils.h"

class QHBoxLayout;
class QProcess;
class QMenu;
class QPropertyAnimation;

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();

    void updateMaxButtonStatus(bool is_maxed);
//    void showWidget();

signals:
    void requestMinWindow();
    void requestMaxWindow(bool b);
    void requestShowMenu();
    void requestCloseWindow();
    void mouseMovedDiff(QPoint);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    enum MenuItemId {
        IdCreateAlbum,
        IdSwitchTheme,
        IdSetting,
        IdImport,
        IdHelp,
        IdAbout,
        IdQuick,
        IdSeparator
    };

    void initLeftContent();
    void initMiddleContent();
    void initRightContent();
    void initMenu();
    void initWidgets();

private:
    QColor m_coverBrush;
    QColor m_topBorderColor;
    QColor m_bottomBorderColor;
    QPointer<QProcess> m_manualPro;
    QHBoxLayout *m_layout = nullptr;
    QHBoxLayout *m_lLayout = nullptr;
    QHBoxLayout *m_mLayout = nullptr;
    QHBoxLayout *m_rLayout = nullptr;
    SystemButton *min_button = nullptr;
    SystemButton *close_button = nullptr;
    SystemButton *max_button = nullptr;
    SystemButton *menu_button = nullptr;

public slots:
    void onSetPlayingTitleName(const QString &name);
    void cleaTitleName();
//    void showSpreadAnimated();
//    void showGatherAnimated();
//    void spreadAniFinished();
//    void gatherAniFinished();

protected:
    bool eventFilter(QObject * obj, QEvent * event);
//    void paintEvent(QPaintEvent *event);

private slots:
//    void checkUnderMouse();

private:
//    QPropertyAnimation *m_spreadAnimation = nullptr;
//    QPropertyAnimation *m_gatherAnimation = nullptr;
    QLabel *m_logoLabel = nullptr;
    QLabel *m_softLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    DragState m_dragState;
    QPoint m_startDrag;
};

#endif // TITLEWIDGET_H
