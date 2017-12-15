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

#ifndef BOTTOMWIDGET_H
#define BOTTOMWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QEvent>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPointer>

class MplayerWindow;
class QTimer;
class QPropertyAnimation;
class TimeSlider;
class SoundVolume;
class MyAction;

enum BottomDragState {NOT_BDRAGGING, START_BDRAGGING, BDRAGGING};


class BottomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BottomWidget(QWidget *parent = 0);
    ~BottomWidget();
//    enum Activation { Anywhere = 1, Bottom = 2 };
    void enable_turned_on();
    void update_widget_qss_property(QWidget *w, const char *name, const QVariant &value);
    void showAlways();
    QString get_status();
    void show_control_widget();
    void setPreviewData(bool preview);
//    void setParentWindow(MplayerWindow* window) { p_mainwindow = window;}
//    void setParentWindow(MplayerWindow* window);

signals:
    void sig_resize_corner();
    void changeProgress(qint64 value, qint64 m_duration);
    void volumeChanged(int volume);
    void toggleMute();
    void togglePlaylist();
    void toggleFullScreen();
    void signal_stop();
    void signal_prev();
    void signal_play_pause_status();
    void signal_next();
    void signal_open_file();
    void signal_mute(/*bool*/);
    void mouseMoving(Qt::MouseButton botton);
    void start_open_file();
    void valueChanged(int);
    void posChanged(int);
    void draggingPos(int);
    void delayedDraggingPos(int);
    void wheelUp();
    void wheelDown();
    void mouseMovedDiff(QPoint);//kobe
    void sig_show_or_hide_esc(bool b);
    void resize_bottom_widget_height(bool b);
    void need_to_save_pre_image(int time);//kobe
    void send_save_preview_image_name(int time, QString filepath);//kobe

public slots:
    void showWidget();
    void activate();
    void deactivate();
    void setMargin(int margin) { spacing = margin; };
//    void setActivationArea(Activation m) { activation_area = m; }
    void setHideDelay(int ms);
    void slot_volumn_changed(int vol);
    void slot_playlist_btn_status(bool b);
    void onMusicPlayed();
    void onMusicPause();
    void onMusicStoped();
    void onVolumeChanged(int volume);
    void onMutedChanged(bool muted, int volumn);
    void onFullScreen();
    void onUnFullScreen();
    void displayTime(QString cur_time, QString all_time);
    void setActionsEnabled(bool b);
    void setPlayOrPauseEnabled(bool b);
    void setStopEnabled(bool b);
    //progress
    virtual void setPos(int);
    virtual int ppos();
    virtual void setDuration(double);
    virtual double duration() { return total_time; };
    void setDragDelay(int);
    int dragDelay();
    void slot_active_status(bool b);

    void spreadAniFinished();
    void gatherAniFinished();

    void update_playlist_count_label(int count);

public:
    bool isActive() { return turned_on; };
    int margin() { return spacing; };
//    Activation activationArea() { return activation_area; }
    int hideDelay();
    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity=0.8);

protected:
    bool eventFilter(QObject * obj, QEvent * event);
    virtual void resizeEvent(QResizeEvent *event);
//    virtual void mouseMoveEvent(QMouseEvent *event);
//        void mousePressEvent(QMouseEvent *);
//        void mouseMoveEvent(QMouseEvent *);
//        void mouseReleaseEvent(QMouseEvent *);
//    signals:
//        void signalMovePoint(QPoint);

private slots:
    void checkUnderMouse();
    void showSpreadAnimated();
    void showGatherAnimated();

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
    QLabel          *playtime_label;
    QLabel          *alltime_label;
    QPushButton     *btStop;
    QPushButton     *btPrev;
    QPushButton     *btPlayPause;
    QPushButton     *btNext;
    QPushButton     *btSound;
    QPushButton     *btFullScreen;
    QPushButton     *btPlayList;
    QLabel          *listCountLabel;
    TimeSlider      *progress;
    QWidget         *controlWidget;
    SoundVolume     *volSlider;
    QFrame          *metaWidget;
    QFrame          *ctlWidget;
    QWidget         *actWidget;
    bool            enableMove;
    double total_time;
    int drag_delay;
    BottomDragState drag_state;
    QPoint start_drag;
//    bool isLeftPressDown;  // 判断左键是否按下
    QVBoxLayout *vboxlayout;
//    MplayerWindow *p_mainwindow;
};









/*class BottomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BottomWidget(QWidget *parent = 0);
    ~BottomWidget();
    enum Activation { Anywhere = 1, Bottom = 2 };
    void enable_turned_on();
    void update_widget_qss_property(QWidget *w, const char *name, const QVariant &value);
    void showAlways();
    void show_control_widget();
    void setPreviewData(bool preview);
    bool getCtrlWidgetVisible();

signals:
    void sig_resize_corner();
    void changeProgress(qint64 value, qint64 m_duration);
    void volumeChanged(int volume);
    void toggleMute();
    void togglePlaylist();
    void toggleFullScreen();
    void signal_stop();
    void signal_prev();
    void signal_play_pause_status();
    void signal_next();
    void signal_open_file();
    void signal_mute();
    void mouseMoving(Qt::MouseButton botton);
    void start_open_file();
    void valueChanged(int);
    void posChanged(int);
    void draggingPos(int);
    void delayedDraggingPos(int);
    void wheelUp();
    void wheelDown();
    void mouseMovedDiff(QPoint);//kobe
    void sig_show_or_hide_esc(bool b);
    void resize_bottom_widget_height(bool b);
    void need_to_save_pre_image(int time);//kobe
    void send_save_preview_image_name(int time, QString filepath);//kobe

public slots:
    void setMargin(int margin) { spacing = margin; };
    void setActivationArea(Activation m) { activation_area = m; }
    void slot_volumn_changed(int vol);
    void slot_playlist_btn_status(bool b);
    void onMusicPlayed();
    void onMusicPause();
    void onMusicStoped();
    void onVolumeChanged(int volume);
    void onMutedChanged(bool muted, int volumn);
    void onFullScreen();
    void onUnFullScreen();
    void displayTime(QString cur_time, QString all_time);
    void setActionsEnabled(bool b);
    void setPlayOrPauseEnabled(bool b);
    void setStopEnabled(bool b);
    //progress
    virtual void setPos(int);
    virtual int ppos();
    virtual void setDuration(double);
    virtual double duration() { return total_time; };
    void setDragDelay(int);
    int dragDelay();
    void slot_active_status(bool b);

    void spreadAniFinished();
    void gatherAniFinished();

    void update_playlist_count_label(int count);
    void showSpreadAnimated();
    void showGatherAnimated();

public:
    bool isActive() { return turned_on; };
    int margin() { return spacing; };
    Activation activationArea() { return activation_area; }
    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity=0.8);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    void installFilter(QObject *o);

private:
    bool turned_on;
    int spacing;
    Activation activation_area;
    QWidget * internal_widget;
    QPropertyAnimation *spreadAnimation;
    QPropertyAnimation *gatherAnimation;
    QLabel          *playtime_label;
    QLabel          *alltime_label;
    QPushButton     *btStop;
    QPushButton     *btPrev;
    QPushButton     *btPlayPause;
    QPushButton     *btNext;
    QPushButton     *btSound;
    QPushButton     *btFullScreen;
    QPushButton     *btPlayList;
    QLabel          *listCountLabel;
    TimeSlider      *progress;
    QWidget         *controlWidget;
    SoundVolume     *volSlider;
    QFrame          *metaWidget;
    QFrame          *ctlWidget;
    QWidget         *actWidget;
    bool            enableMove;
    double total_time;
    int drag_delay;
    BottomDragState drag_state;
    QPoint start_drag;
    QVBoxLayout *vboxlayout;
};*/

#endif // BOTTOMWIDGET_H
