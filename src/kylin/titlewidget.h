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

class QHBoxLayout;
class QProcess;
class QMenu;
class QTimer;
class QPropertyAnimation;

enum TitleDragState {NOT_TDRAGGING, START_TDRAGGING, TDRAGGING};


class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();
//    enum Activation { Anywhere = 1, Bottom = 2 };
    void enable_turned_on();
    void showAlways();
    void update_max_status(bool is_maxed);
    void show_title_widget();

signals:
    void sig_min();
    void sig_max();
    void sig_menu();
    void sig_close();
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

private slots:
    void onMaxOrNormal();
    void onMinBtnClicked();

private:
    QColor m_coverBrush;
    QColor m_topBorderColor;
    QColor m_bottomBorderColor;
    QPointer<QProcess> m_manualPro;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_lLayout;
    QHBoxLayout *m_mLayout;
    QHBoxLayout *m_rLayout;
//    QPushButton *menu_button;
//    QPushButton *min_button;
//    QPushButton *max_button;
//    QPushButton *close_button;
    SystemButton *min_button;
    SystemButton *close_button;
    SystemButton *max_button;
    SystemButton *menu_button;


public slots:
    void showWidget();
    void activate();
    void deactivate();
    void setMargin(int margin) { spacing = margin; };
//    void setActivationArea(Activation m) { activation_area = m; }
    void setHideDelay(int ms);
    void set_title_name(QString title);
    void clear_title_name();
    void showSpreadAnimated();
    void showGatherAnimated();
    void spreadAniFinished();
    void gatherAniFinished();

public:
    bool isActive() { return turned_on; };
    int margin() { return spacing; };
//    Activation activationArea() { return activation_area; }
    int hideDelay();

protected:
    bool eventFilter(QObject * obj, QEvent * event);
    void paintEvent(QPaintEvent *event);

private slots:
    void checkUnderMouse();

private:
    void installFilter(QObject *o);
private:
    bool turned_on;
    int spacing;
//    Activation activation_area;
    QWidget * internal_widget;
    QTimer * timer;
    QPropertyAnimation *spreadAnimation;
    QPropertyAnimation *gatherAnimation;
    QLabel *logo_label;
    QLabel *soft_label;
    QLabel *title_label;
    TitleDragState drag_state;
    QPoint start_drag;
};



/*class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();
    enum Activation { Anywhere = 1, Bottom = 2 };
    void enable_turned_on();
    void showAlways();
    void update_max_status(bool is_maxed);
    void show_title_widget();

signals:
    void sig_min();
    void sig_max();
    void sig_menu();
    void sig_close();
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

private slots:
    void onMaxOrNormal();
    void onMinBtnClicked();

private:
    QColor m_coverBrush;
    QColor m_topBorderColor;
    QColor m_bottomBorderColor;
    QPointer<QProcess> m_manualPro;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_lLayout;
    QHBoxLayout *m_mLayout;
    QHBoxLayout *m_rLayout;
//    QPushButton *menu_button;
//    QPushButton *min_button;
//    QPushButton *max_button;
//    QPushButton *close_button;

    SystemButton *min_button;
    SystemButton *close_button;
    SystemButton *max_button;
    SystemButton *menu_button;

public slots:
    void setMargin(int margin) { spacing = margin; };
    void setActivationArea(Activation m) { activation_area = m; }
    void set_title_name(QString title);
    void clear_title_name();
    void showSpreadAnimated();
    void showGatherAnimated();
    void spreadAniFinished();
    void gatherAniFinished();

public:
    bool isActive() { return turned_on; };
    int margin() { return spacing; };
    Activation activationArea() { return activation_area; }

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool turned_on;
    int spacing;
    Activation activation_area;
    QWidget * internal_widget;
    QPropertyAnimation *spreadAnimation;
    QPropertyAnimation *gatherAnimation;
    QLabel *logo_label;
    QLabel *soft_label;
    QLabel *title_label;
    TitleDragState drag_state;
    QPoint start_drag;
};*/

#endif // TITLEWIDGET_H
