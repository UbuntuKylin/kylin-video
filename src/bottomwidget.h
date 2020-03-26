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

#ifndef BOTTOMWIDGET_H
#define BOTTOMWIDGET_H

#include "utils.h"

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

class VideoWindow;
class QPropertyAnimation;
class TimeSlider;
class SoundVolume;
class MyAction;

class BottomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BottomWidget(QWidget *parent = 0);
    ~BottomWidget();

    void updateWidgetQssProperty(QWidget *w, const char *name, const QVariant &value);
    QString getActiveStatus();
    void onShowControlWidget();
    void setPreviewData(bool preview);
    void savePreviewImageName(int time, QString filepath);
//    void showWidget();

signals:
    void requestVolumeChanged(int volume);
    void togglePlaylist();
    void toggleFullScreen();
    void toggleStop();
    void togglePrev();
    void togglePlayPause();
    void toggleNext();
    void toggleMute();
    void mouseMoving(Qt::MouseButton botton);
    void start_open_file();
    void valueChanged(int);
    void posChanged(int);
    void draggingPos(int);
    void delayedDraggingPos(int);
    void wheelUp();
    void wheelDown();
    void mouseMovedDiff(QPoint);
//    void requestShowOrHideEscWidget(bool b);
    void resize_bottom_widget_height(bool b);
    void requestSavePreviewImage(int time, QPoint pos);
    void requestHideTip();
    void mouseEnter();
    void mouseLeave();
    void requestTemporaryShow();

public slots:
    void updatePlaylistBtnQssProperty(bool b);
    void onMusicPlayed();
    void onMusicPause();
    void onMusicStoped();
    void onVolumeChanged(int volume);
    void onMutedChanged(bool muted, int volumn);
    void onFullScreen();
    void onUnFullScreen();
    void displayTime(QString cur_time, QString all_time);
    void onSetActionsEnabled(bool b);
    void setPlayOrPauseEnabled(bool b);

    //progress
    void setPos(int);
    int ppos();
    void setDuration(double);
    double duration() { return m_totalTime; };
    void setDragDelay(int);
    int dragDelay();
    void onProgressActiveStatus(bool b);
//    void spreadAniFinished();
//    void gatherAniFinished();
    void updateLabelCountNumber(int count);

protected:
    virtual bool eventFilter(QObject * obj, QEvent * event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private slots:
//    void checkUnderMouse();
//    void showSpreadAnimated();
//    void showGatherAnimated();

private:
//    QPropertyAnimation *m_spreadAnimation = nullptr;
//    QPropertyAnimation *m_gatherAnimation = nullptr;
    QLabel *m_playtimeLabel = nullptr;
    QLabel *m_totaltimeLabel = nullptr;
    QPushButton *m_btnStop = nullptr;
    QPushButton *m_btnPrev = nullptr;
    QPushButton *m_btnPlayPause = nullptr;
    QPushButton *m_btnNext = nullptr;
    QPushButton *m_btnSound = nullptr;
    QPushButton *m_btnFullScreen = nullptr;
    QPushButton *m_btnPlayList = nullptr;
    QLabel *m_listCountLabel = nullptr;
    TimeSlider *m_timeProgress = nullptr;
    QWidget *m_controlWidget = nullptr;
    SoundVolume *m_volSlider = nullptr;
    QFrame *m_metaWidget = nullptr;
    QFrame *m_ctlWidget = nullptr;
    QWidget *m_actWidget = nullptr;
    double m_totalTime;
    int m_dragDelay;
    DragState m_dragState;
    QPoint m_startDrag;
    QVBoxLayout *vboxlayout = nullptr;
};

#endif // BOTTOMWIDGET_H
