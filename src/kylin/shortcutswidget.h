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

#ifndef SHORTCUTSWIDGET_H
#define SHORTCUTSWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QDebug>
#include "../smplayer/preferences.h"

class QTimer;
class QLabel;
class QPushButton;
class QGridLayout;
class QFormLayout;

class ShortcutsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsWidget(QWidget *parent = 0);
    ~ShortcutsWidget();
    static ShortcutsWidget *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new ShortcutsWidget;
            }
        }
        return self;
    }

    void restart_timer();
    void init_ui();

    void setPrefData(Preferences * pref);

protected:
    void showEvent(QShowEvent *);
//    virtual void keyReleaseEvent(QKeyEvent *event);

public slots:
    void set_parent_widget(QWidget *idget);
    void set_background_color(const QColor &bgColor);
    void set_widget_opacity(double opacity);
    void hide_mask_widget();
    void onCloseButtonClicked();

private:
    static ShortcutsWidget *self;
    QWidget *parent_widget;
    QTimer *timer;
    QLabel *title_label;
    QLabel *control_title_label;
    QLabel *other_title_label;
    QPushButton *close_button;
//    QFormLayout *control_form_layout;
//    QList <QLabel *> control_label_list;
    QGridLayout *control_grid_layout;
//    QList <QLabel *> other_label_list;
    QGridLayout *other_grid_layout;
    QLabel *play_title;
    QLabel *play_label;
    QLabel *play_pre_title;
    QLabel *play_pre_label;
    QLabel *play_next_title;
    QLabel *play_next_label;
    QLabel *forward_10s_title;
    QLabel *forward_10s_label;
    QLabel *forward_1m_title;
    QLabel *forward_1m_label;
    QLabel *forward_10m_title;
    QLabel *forward_10m_label;
    QLabel *back_10s_title;
    QLabel *back_10s_label;
    QLabel *back_1m_title;
    QLabel *back_1m_label;
    QLabel *back_10m_title;
    QLabel *back_10m_label;
    QLabel *jump_title;
    QLabel *jump_label;
    QLabel *mute_title;
    QLabel *mute_label;
    QLabel *vol_up_title;
    QLabel *vol_up_label;
    QLabel *vol_down_title;
    QLabel *vol_down_label;
    QLabel *set_audio_delay_title;
    QLabel *set_audio_delay_label;
    QLabel *audio_delay_title;
    QLabel *audio_delay_label;

    QLabel *playlist_title;
    QLabel *playlist_label;
    QLabel *open_title;
    QLabel *open_label;
    QLabel *screenshot_title;
    QLabel *screenshot_label;
    QLabel *pref_title;
    QLabel *pref_label;
    QLabel *info_title;
    QLabel *info_label;
    QLabel *about_title;
    QLabel *about_label;
    QLabel *quit_title;
    QLabel *quit_label;
    QLabel *fullscreen_title;
    QLabel *fullscreen_label;
};

#endif // SHORTCUTSWIDGET_H
