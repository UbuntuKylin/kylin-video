/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
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

#include "bottomwidget.h"
#include <QPainter>
#include <QProcess>
#include <QResizeEvent>
#include <QShortcut>
#include <QStyleFactory>
#include <QTimer>
#include <QDebug>
#include <QPropertyAnimation>
#include <QStyle>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QProgressBar>
#include <QStackedLayout>
#include <QPoint>
#include "timeslider.h"
#include "soundvolume.h"
#include "colorutils.h"
#include "myaction.h"
#include "mplayerwindow.h"

static const QString playStatusPlaying = "playing";
static const QString playStatusPause = "pause";
static const QString playStatusStop = "stop";
static const QString playStatusPlayList = "close";
static const char *property_FullScreen = "fullscreen";
static const char *property_PlayStatus = "playstatus";
static const char *property_PlayListStatus = "playliststatus";

BottomWidget::BottomWidget(QWidget *parent)
    : QWidget(parent, Qt::SubWindow)
    , turned_on(false)
    , spacing(0)
//    , activation_area(Anywhere)
    , internal_widget(0)
    , timer(0)
    , spreadAnimation(0)
    , gatherAnimation(0)
    , drag_state(NOT_BDRAGGING)
    , start_drag(QPoint(0,0))
{
    this->setMouseTracking(true);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#040404"));
    this->setPalette(palette);

    setWindowFlags(windowFlags() | Qt::SubWindow);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    drag_delay = 200;
    enableMove      = false;

    //20170810
    parent->installEventFilter(this);
//    parent->setMouseTracking(true);
    installFilter(parent);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkUnderMouse()));
    timer->setInterval(3000);

    controlWidget = new QWidget(this);
    controlWidget->setFixedHeight(61);
    progress = NULL;
    progress = new TimeSlider(this);
    progress->setMouseTracking(true);
//    progress->installEventFilter(this);
//    progress->setParent(this);
    progress->setDragDelay(100);//pref->time_slider_drag_delay
    connect(progress, SIGNAL(posChanged(int)), this, SIGNAL(posChanged(int)));
    connect(progress, SIGNAL(draggingPos(int)), this, SIGNAL(draggingPos(int)));
    connect(progress, SIGNAL(delayedDraggingPos(int)), this, SIGNAL(delayedDraggingPos(int)));
    connect(progress, SIGNAL(wheelUp()), this, SIGNAL(wheelUp()));
    connect(progress, SIGNAL(wheelDown()), this, SIGNAL(wheelDown()));
    connect(progress, SIGNAL(need_to_save_pre_image(int)), this, SIGNAL(need_to_save_pre_image(int)));
    connect(this, SIGNAL(send_save_preview_image_name(int,QString)), progress, SLOT(show_save_preview_image(int,QString)));
    progress->setObjectName("processProgress");
    progress->setProperty("status", "");
    connect(progress, SIGNAL(active_status(bool)), this, SLOT(slot_active_status(bool)));
    progress->setFixedHeight(24);
    progress->move(0,0);
    playtime_label = new QLabel;
    playtime_label->setText("00:00:00");
    playtime_label->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor(playtime_label, QColor(0,0,0) );
    ColorUtils::setForegroundColor(playtime_label, QColor(255,255,255) );
    playtime_label->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    playtime_label->adjustSize();
    alltime_label = new QLabel;
    alltime_label->setText(" / 00:00:00");
    alltime_label->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor( alltime_label, QColor(0,0,0) );
    ColorUtils::setForegroundColor( alltime_label, QColor(255,255,255) );
    alltime_label->setStyleSheet("QLabel{font-size:12px;color:#808080;}");
    alltime_label->adjustSize();
    btStop = new QPushButton;
    btStop->setObjectName("StopBtn");
    btStop->setFixedSize(24, 24);
    btPrev = new QPushButton;
    btPrev->setObjectName("PrevBtn");
    btPrev->setFixedSize(24, 24);
    btPlayPause = new QPushButton;
    btPlayPause->setShortcut(Qt::Key_Space);
    btPlayPause->setObjectName("PlayBtn");
    btPlayPause->setFixedSize(61, 61);
    btNext = new QPushButton;
    btNext->setObjectName("NextBtn");
    btNext->setFixedSize(24, 24);
    btSound = new QPushButton;
    btSound->setObjectName("SoundBtn");
    btSound->setFixedSize(24, 24);
    btSound->setProperty("volume", "mid");
    btFullScreen = new QPushButton;
    btFullScreen->setShortcut(QKeySequence("Ctrl+Return"));
    btFullScreen->setObjectName("FullScreenBtn");
    btFullScreen->setFixedSize(24, 24);
    btPlayList = new QPushButton;
    btPlayList->setShortcut(QKeySequence("F3"));
    btPlayList->setObjectName("PlayListBtn");
    btPlayList->setFixedSize(24, 24);
    listCountLabel = new QLabel;
    ColorUtils::setBackgroundColor(listCountLabel, QColor("#141414") );
    listCountLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    this->listCountLabel->setText("0");
    volSlider = new SoundVolume(this);
    volSlider->setObjectName("VolumeSlider");
    connect(this, SIGNAL(valueChanged(int)), volSlider, SLOT(setValue(int)));
    connect(btStop, SIGNAL(released()), this, SIGNAL(signal_stop()));
    connect(btPrev, SIGNAL(released()), this, SIGNAL(signal_prev()));
    connect(btPlayPause, SIGNAL(released()), this, SIGNAL(signal_play_pause_status()));
    connect(btNext, SIGNAL(released()), this, SIGNAL(signal_next()));
    connect(btSound, SIGNAL(pressed()), this, SIGNAL(signal_mute(/*bool*/)));
    connect(volSlider, SIGNAL(volumeChanged(int)), this, SLOT(slot_volumn_changed(int)));
    connect(btFullScreen, SIGNAL(pressed()), this, SIGNAL(toggleFullScreen()));
    connect(btPlayList, SIGNAL(released()), this, SIGNAL(togglePlaylist()));
    connect(this, SIGNAL(mouseMoving(Qt::MouseButton)), progress, SLOT(hideTip()));//20170714

    QHBoxLayout *time_layout = new QHBoxLayout();
    time_layout->setMargin(0);
    time_layout->setSpacing(2);
    time_layout->addWidget(playtime_label, 0, Qt::AlignLeft | Qt::AlignVCenter);
    time_layout->addWidget(alltime_label, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metaWidget = new QFrame(this);
    metaWidget->setFixedHeight(61);
    QHBoxLayout *metaLayout = new QHBoxLayout(metaWidget);
    metaLayout->setMargin(0);
    metaLayout->setSpacing(10);
    metaLayout->addWidget(btPlayPause, 0, Qt::AlignVCenter);
    metaLayout->addLayout(time_layout);

    ctlWidget = new QFrame(this);
    ctlWidget->setFixedHeight(61);
    QHBoxLayout *ctlLayout = new QHBoxLayout(ctlWidget);
    ctlLayout->setMargin(0);
    ctlLayout->setSpacing(30);

    ctlLayout->addWidget(btPrev, 0, Qt::AlignCenter);
    ctlLayout->addWidget(btStop, 0, Qt::AlignCenter);
    ctlLayout->addWidget(btNext, 0, Qt::AlignCenter);
    ctlWidget->adjustSize();

    QHBoxLayout *volume_layout = new QHBoxLayout();
    volume_layout->setMargin(0);
    volume_layout->setSpacing(2);
    volume_layout->addWidget(btSound, 0, Qt::AlignRight | Qt::AlignVCenter);
    volume_layout->addWidget(volSlider, 0, Qt::AlignRight | Qt::AlignVCenter);

    QHBoxLayout *list_layout = new QHBoxLayout();
    list_layout->setMargin(0);
    list_layout->setSpacing(2);
    list_layout->addWidget(btPlayList, 0, Qt::AlignRight | Qt::AlignVCenter);
    list_layout->addWidget(listCountLabel, 0, Qt::AlignRight | Qt::AlignVCenter);

    actWidget = new QWidget(this);
    actWidget->setFixedHeight(61);
    QHBoxLayout *actLayout = new QHBoxLayout(actWidget);
    actLayout->setMargin(0);
    actLayout->setSpacing(20);
    actLayout->addLayout(volume_layout);
    actLayout->addWidget(btFullScreen, 0, Qt::AlignRight | Qt::AlignVCenter);
    actLayout->addLayout(list_layout);

    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp.setHorizontalStretch(33);
    metaWidget->setSizePolicy(sp);
    actWidget->setSizePolicy(sp);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 0, 20, 0);
    layout->setSpacing(20);
    layout->addWidget(metaWidget, 0, Qt::AlignLeft/* | Qt::AlignVCenter*/);
    layout->addStretch();
    layout->addWidget(ctlWidget, 0, Qt::AlignHCenter/*Qt::AlignCenter*/);
    layout->addStretch();
    layout->addWidget(actWidget, 0, Qt::AlignRight/* | Qt::AlignVCenter*/);
    controlWidget->setLayout(layout);

    vboxlayout = new QVBoxLayout(this);
    vboxlayout->setSpacing(0);
    vboxlayout->setContentsMargins(0, 18, 0, 2);
    vboxlayout->addWidget(controlWidget);

    btStop->setFocusPolicy(Qt::NoFocus);
    btPrev->setFocusPolicy(Qt::NoFocus);
    btPlayPause->setFocusPolicy(Qt::NoFocus);
    btNext->setFocusPolicy(Qt::NoFocus);
    btFullScreen->setFocusPolicy(Qt::NoFocus);
    btSound->setFocusPolicy(Qt::NoFocus);
    volSlider->setFocusPolicy(Qt::NoFocus);
    btPlayList->setFocusPolicy(Qt::NoFocus);

    btStop->setToolTip(tr("Stop"));
    btPrev->setToolTip(tr("Prev"));
    btPlayPause->setToolTip(tr("Play / Pause"));
    btNext->setToolTip(tr("Next"));
    btSound->setToolTip(tr("Mute"));
    btPlayList->setToolTip(tr("Play List"));

    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPause);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPause);
}

BottomWidget::~BottomWidget() {
    if (spreadAnimation) delete spreadAnimation;
    if (gatherAnimation) delete gatherAnimation;

    if (metaWidget) delete metaWidget;
    if (ctlWidget) delete ctlWidget;
    if (actWidget) delete actWidget;
    if (controlWidget) delete controlWidget;

    if (timer != NULL) {
        disconnect(timer,SIGNAL(timeout()),this,SLOT(checkUnderMouse()));
        if(timer->isActive()) {
            timer->stop();
        }
        delete timer;
        timer = NULL;
    }
}

/*void BottomWidget::setParentWindow(MplayerWindow* window) {
    p_mainwindow = window;
    p_mainwindow->installEventFilter(this);
    p_mainwindow->setMouseTracking(true);
}*/

void BottomWidget::setPreviewData(bool preview) {
    if (progress) {
        progress->set_preview_flag(preview);
    }
}

QString BottomWidget::get_status()
{
    QString status = progress->property("active").toString();
    return status;
}

void BottomWidget::update_playlist_count_label(int count) {
    this->listCountLabel->setText(QString::number(count));
}

void BottomWidget::setTransparent(bool transparent) {
    if (transparent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
        set_widget_opacity(0.5);
    }
    else {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void BottomWidget::set_widget_opacity(const float &opacity) {
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void BottomWidget::slot_active_status(bool b) {
    if (b) {
        this->update_widget_qss_property(progress, "status", "active");
    }
    else {
        this->update_widget_qss_property(progress, "status", "");
    }
}

void BottomWidget::setPos(int v) {
    bool was_blocked= progress->blockSignals(true);
    progress->setPos(v);
    progress->blockSignals(was_blocked);
}

int BottomWidget::ppos() {
    return progress->pos();
}

void BottomWidget::setDuration(double t) {
    total_time = t;
    progress->setDuration(t);
}

void BottomWidget::setDragDelay(int d) {
    drag_delay = d;
    progress->setDragDelay(drag_delay);
}

int BottomWidget::dragDelay() {
    return drag_delay;
}

void BottomWidget::setActionsEnabled(bool b) {
    btPrev->setEnabled(b);
    btPlayPause->setEnabled(b);
    btNext->setEnabled(b);
    btSound->setEnabled(b);
    btStop->setEnabled(b);
    progress->setEnabled(b);
}

void BottomWidget::setPlayOrPauseEnabled(bool b) {
    btPlayPause->setEnabled(b);
}

void BottomWidget::setStopEnabled(bool b) {
    btStop->setEnabled(b);
}

void BottomWidget::displayTime(QString cur_time, QString all_time) {
    playtime_label->setText(cur_time);
    alltime_label->setText(all_time);
}

void BottomWidget::slot_playlist_btn_status(bool b) {
    if (b) {
        this->update_widget_qss_property(btPlayList, property_PlayListStatus, playStatusPlayList);
    }
    else {
        this->update_widget_qss_property(btPlayList, property_PlayListStatus, "");
    }
}

void BottomWidget::slot_volumn_changed(int vol) {
    this->onVolumeChanged(vol);
    emit this->volumeChanged(vol);
}

void BottomWidget::onMusicPlayed() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPlaying);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPlaying);
}

void BottomWidget::onMusicPause() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPause);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPause);
}

void BottomWidget::onMusicStoped() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusStop);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusStop);
}

void BottomWidget::onFullScreen() {
    this->update_widget_qss_property(btFullScreen, property_FullScreen, true);
}

void BottomWidget::onUnFullScreen() {
    this->update_widget_qss_property(btFullScreen, property_FullScreen, false);
}

void BottomWidget::onVolumeChanged(int volume) {
    QString status = "mid";
    if (volume > 77) {
        status = "high";
    } else if (volume > 33) {
        status = "mid";
    } else if (volume > 0) {
        status = "low";
    } else  {
        status = "mute";
    }
    this->update_widget_qss_property(btSound, "volume", status);
    this->volSlider->onVolumeChanged(volume);
}

void BottomWidget::onMutedChanged(bool muted, int volumn) {
    if (muted) {
        this->update_widget_qss_property(btSound, "volume", "mute");
        this->volSlider->onVolumeChanged(0);
    }
    else {
        this->onVolumeChanged(volumn);
    }
}

void BottomWidget::setHideDelay(int ms) {
    timer->setInterval(ms);
}

int BottomWidget::hideDelay() {
    return timer->interval();
}

void BottomWidget::update_widget_qss_property(QWidget *w, const char *name, const QVariant &value) {
    w->setProperty(name, value);
    this->style()->unpolish(w);
    this->style()->polish(w);
    w->update();
}

void BottomWidget::installFilter(QObject *o) {
    QObjectList children = o->children();
    for (int n=0; n < children.count(); n++) {
        if (children[n]->isWidgetType()) {
            if (children[n]->objectName() == "PlayListViewScrollBar") {//kobe:让滚动条可以鼠标拖动
                continue;
            }
            else if (children[n]->objectName() == "min_button" || children[n]->objectName() == "close_button" || children[n]->objectName() == "menu_button" || children[n]->objectName() == "max_button") {
                continue;
            }
            else if (children[n]->objectName() == "StopBtn" || children[n]->objectName() == "PrevBtn" || children[n]->objectName() == "PlayBtn" || children[n]->objectName() == "NextBtn") {
                continue;
            }
            else if (children[n]->objectName() == "SoundBtn" || children[n]->objectName() == "FullScreenBtn" || children[n]->objectName() == "PlayListBtn" || children[n]->objectName() == "VolumeSlider") {
                continue;
            }
            QWidget *w = static_cast<QWidget *>(children[n]);
            w->setMouseTracking(true);
            w->installEventFilter(this);
            installFilter(children[n]);
        }
    }
}

void BottomWidget::activate() {
    turned_on = true;
    if (timer->isActive())
        timer->stop();
    timer->start();
}

void BottomWidget::deactivate() {
    turned_on = false;
    timer->stop();
    this->showWidget();
}

void BottomWidget::showAlways() {
    turned_on = false;
    timer->stop();
    QPoint dest_position = QPoint(0, parentWidget()->size().height() - this->height());
    move(dest_position);
    controlWidget->show();
    QWidget::show();
}

void BottomWidget::enable_turned_on() {
    turned_on = true;
}

void BottomWidget::showWidget() {
    showSpreadAnimated();
    if (timer->isActive())
        timer->stop();
    timer->start();
}

void BottomWidget::checkUnderMouse() {
    if ((ctlWidget->isVisible()) && (!underMouse())) {//0616
        this->showGatherAnimated();
        //0621 tell mainwindow to hide escwidget
        emit this->sig_show_or_hide_esc(false);
    }
}

bool BottomWidget::eventFilter(QObject * obj, QEvent * event) {
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;
    if (mouseEvent->modifiers() != Qt::NoModifier) {
        drag_state = NOT_BDRAGGING;
        return false;
    }
    if (turned_on) {
        if (event->type() == QEvent::MouseMove) {
            if (!isVisible()) {
                showWidget();
                /*if (activation_area == Anywhere) {
                    showWidget();
                } else {
                    QMouseEvent * mouse_event = dynamic_cast<QMouseEvent*>(event);
                    QWidget * parent = parentWidget();
                    QPoint p = parent->mapFromGlobal(mouse_event->globalPos());
                    if (p.y() > (parent->height() - height() - spacing)) {
                        showWidget();
                    }
                }*/
            }
            else if (!ctlWidget->isVisible()) {
                showWidget();
                /*if (activation_area == Anywhere) {
                    showWidget();
                } else {
                    QMouseEvent * mouse_event = dynamic_cast<QMouseEvent*>(event);
                    QWidget * parent = parentWidget();
                    QPoint p = parent->mapFromGlobal(mouse_event->globalPos());
                    if (p.y() > (parent->height() - height() - spacing)) {
                        showWidget();
                    }
                }*/
            }
        }

        if (type == QEvent::MouseButtonPress) {
            if (mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_BDRAGGING;
                return false;
            }

            drag_state = START_BDRAGGING;
            start_drag = mouseEvent->globalPos();
            // Don't filter, so others can have a look at it too
            return false;
        }

        if (type == QEvent::MouseButtonRelease) {
            if (drag_state != BDRAGGING || mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_BDRAGGING;
                return false;
            }

            // Stop dragging and eat event
            drag_state = NOT_BDRAGGING;
            event->accept();
            return true;
        }

        // type == QEvent::MouseMove
        if (drag_state == NOT_BDRAGGING)
            return false;

        // buttons() note the s
        if (mouseEvent->buttons() != Qt::LeftButton) {
            drag_state = NOT_BDRAGGING;
            return false;
        }

        QPoint pos = mouseEvent->globalPos();
        QPoint diff = pos - start_drag;
        if (drag_state == START_BDRAGGING) {
            // Don't start dragging before moving at least DRAG_THRESHOLD pixels
            if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
                return false;

            drag_state = BDRAGGING;
        }

        emit mouseMovedDiff(diff);
        start_drag = pos;

        event->accept();
        return true;
    }
    else {
        if (type == QEvent::MouseButtonPress) {
            if (mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_BDRAGGING;
                return false;
            }

            drag_state = START_BDRAGGING;
            start_drag = mouseEvent->globalPos();
            // Don't filter, so others can have a look at it too
            return false;
        }

        if (type == QEvent::MouseButtonRelease) {
            if (drag_state != BDRAGGING || mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_BDRAGGING;
                return false;
            }

            // Stop dragging and eat event
            drag_state = NOT_BDRAGGING;
            event->accept();
            return true;
        }

        // type == QEvent::MouseMove
        if (drag_state == NOT_BDRAGGING)
            return false;

        // buttons() note the s
        if (mouseEvent->buttons() != Qt::LeftButton) {
            drag_state = NOT_BDRAGGING;
            return false;
        }

        QPoint pos = mouseEvent->globalPos();
        QPoint diff = pos - start_drag;
        if (drag_state == START_BDRAGGING) {
            // Don't start dragging before moving at least DRAG_THRESHOLD pixels
            if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
                return false;

            drag_state = BDRAGGING;
        }

        emit mouseMovedDiff(diff);
        start_drag = pos;

        event->accept();
        return true;
    }
}

void BottomWidget::spreadAniFinished() {
}

void BottomWidget::gatherAniFinished()
{
    controlWidget->hide();
}

void BottomWidget::show_control_widget()
{
    controlWidget->show();
}

void BottomWidget::showSpreadAnimated() {
    if (!spreadAnimation) {
        spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), parentWidget()->size().height());
    QPoint final_position = QPoint(0, parentWidget()->size().height() - this->height());
    move(initial_position);

    QWidget::show();
    controlWidget->show();

    spreadAnimation->setDuration(300);
    spreadAnimation->setStartValue(initial_position);
    spreadAnimation->setEndValue(final_position);
    spreadAnimation->start();

    //kobe: tell mainwindow to show escwidget
    emit this->sig_show_or_hide_esc(true);
}


void BottomWidget::showGatherAnimated() {
    if (!gatherAnimation) {
        gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, parentWidget()->size().height() - this->height());
    QPoint final_position = QPoint(pos().x(), parentWidget()->size().height() - 4);//kobe 0616:给最底下的进度条留下空间
    move(initial_position);

    gatherAnimation->setDuration(300);
    gatherAnimation->setStartValue(initial_position);
    gatherAnimation->setEndValue(final_position);
    gatherAnimation->start();
}

//--------------------------------------
/*void BottomWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint topLeftPoint (0, this->height()-140);
    QPoint rightBottomPoint(this->width(), this->height());
    topLeftPoint  = this->mapToGlobal(topLeftPoint);
    rightBottomPoint = this->mapToGlobal(rightBottomPoint);
    QPoint mouseGlobalPoint = this->mapToGlobal(event->pos());

    if(isLeftPressDown)  //
    {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (!mouseEvent)
            return;
        if (mouseEvent->modifiers() != Qt::NoModifier) {
            return;
        }
        QPoint pos = mouseEvent->globalPos();
        QPoint diff = pos - start_drag;
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return;

        emit mouseMovedDiff(diff);
        start_drag = pos;

//        emit mouseMovedDiff(event->globalPos()-start_drag);
//         emit signalMovePoint(event->globalPos()-start_drag);
    }
}

void BottomWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        this->releaseMouse();

    }
}

void BottomWidget::mousePressEvent(QMouseEvent *event)
{

    switch(event->button()) {
    case Qt::LeftButton:
    {
        isLeftPressDown = true;
//        emit signalLeftPressDown(true);
        start_drag = event->globalPos();
        break;
    }
    case Qt::RightButton:
        break;
    default:
        break;
    }
}*/
//--------------------------------------

//void BottomWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    QWidget::mouseMoveEvent(event);

//    Qt::MouseButton button = event->buttons() & Qt::LeftButton ? Qt::LeftButton : Qt::NoButton;
//    if (this->enableMove && event->buttons() == Qt::LeftButton) {
//        emit mouseMoving(button);
//    }
//}

void BottomWidget::resizeEvent(QResizeEvent *event)
{
    //20170810
    QWidget::resizeEvent(event);
    emit sig_resize_corner();
    progress->resize(this->width(), 24);
}














/*BottomWidget::BottomWidget(QWidget *parent)
    : QWidget(parent, Qt::SubWindow)
    , turned_on(false)
    , spacing(0)
    , activation_area(Anywhere)
    , internal_widget(0)
    , spreadAnimation(0)
    , gatherAnimation(0)
    , drag_state(NOT_BDRAGGING)
    , start_drag(QPoint(0,0))
{
    this->setMouseTracking(true);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#040404"));
    this->setPalette(palette);

    setWindowFlags(windowFlags() | Qt::SubWindow);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    drag_delay = 200;
    enableMove      = false;

    controlWidget = new QWidget(this);
    controlWidget->setFixedHeight(61);
    progress = NULL;
    progress = new TimeSlider(this);
    progress->setMouseTracking(true);
    progress->setDragDelay(100);//pref->time_slider_drag_delay
    connect(progress, SIGNAL(posChanged(int)), this, SIGNAL(posChanged(int)));
    connect(progress, SIGNAL(draggingPos(int)), this, SIGNAL(draggingPos(int)));
    connect(progress, SIGNAL(delayedDraggingPos(int)), this, SIGNAL(delayedDraggingPos(int)));
    connect(progress, SIGNAL(wheelUp()), this, SIGNAL(wheelUp()));
    connect(progress, SIGNAL(wheelDown()), this, SIGNAL(wheelDown()));
    connect(progress, SIGNAL(need_to_save_pre_image(int)), this, SIGNAL(need_to_save_pre_image(int)));
    connect(this, SIGNAL(send_save_preview_image_name(int,QString)), progress, SLOT(show_save_preview_image(int,QString)));
    progress->setObjectName("processProgress");
    progress->setProperty("status", "");
    connect(progress, SIGNAL(active_status(bool)), this, SLOT(slot_active_status(bool)));
    progress->setFixedHeight(24);
    progress->move(0,0);
    playtime_label = new QLabel;
    playtime_label->setText("00:00:00");
    playtime_label->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor(playtime_label, QColor(0,0,0) );
    ColorUtils::setForegroundColor(playtime_label, QColor(255,255,255) );
    playtime_label->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    playtime_label->adjustSize();
    alltime_label = new QLabel;
    alltime_label->setText(" / 00:00:00");
    alltime_label->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor( alltime_label, QColor(0,0,0) );
    ColorUtils::setForegroundColor( alltime_label, QColor(255,255,255) );
    alltime_label->setStyleSheet("QLabel{font-size:12px;color:#808080;}");
    alltime_label->adjustSize();
    btStop = new QPushButton;
    btStop->setObjectName("StopBtn");
    btStop->setFixedSize(24, 24);
    btPrev = new QPushButton;
    btPrev->setObjectName("PrevBtn");
    btPrev->setFixedSize(24, 24);
    btPlayPause = new QPushButton;
    btPlayPause->setShortcut(Qt::Key_Space);
    btPlayPause->setObjectName("PlayBtn");
    btPlayPause->setFixedSize(61, 61);
    btNext = new QPushButton;
    btNext->setObjectName("NextBtn");
    btNext->setFixedSize(24, 24);
    btSound = new QPushButton;
    btSound->setObjectName("SoundBtn");
    btSound->setFixedSize(24, 24);
    btSound->setProperty("volume", "mid");
    btFullScreen = new QPushButton;
    btFullScreen->setShortcut(QKeySequence("Ctrl+Return"));
    btFullScreen->setObjectName("FullScreenBtn");
    btFullScreen->setFixedSize(24, 24);
    btPlayList = new QPushButton;
    btPlayList->setShortcut(QKeySequence("F3"));
    btPlayList->setObjectName("PlayListBtn");
    btPlayList->setFixedSize(24, 24);
    listCountLabel = new QLabel;
    ColorUtils::setBackgroundColor(listCountLabel, QColor("#141414") );
    listCountLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    this->listCountLabel->setText("0");
    volSlider = new SoundVolume(this);
    volSlider->setObjectName("VolumeSlider");
    connect(this, SIGNAL(valueChanged(int)), volSlider, SLOT(setValue(int)));
    connect(btStop, SIGNAL(released()), this, SIGNAL(signal_stop()));
    connect(btPrev, SIGNAL(released()), this, SIGNAL(signal_prev()));
    connect(btPlayPause, SIGNAL(released()), this, SIGNAL(signal_play_pause_status()));
    connect(btNext, SIGNAL(released()), this, SIGNAL(signal_next()));
    connect(btSound, SIGNAL(pressed()), this, SIGNAL(signal_mute()));
    connect(volSlider, SIGNAL(volumeChanged(int)), this, SLOT(slot_volumn_changed(int)));
    connect(btFullScreen, SIGNAL(pressed()), this, SIGNAL(toggleFullScreen()));
    connect(btPlayList, SIGNAL(released()), this, SIGNAL(togglePlaylist()));
    connect(this, SIGNAL(mouseMoving(Qt::MouseButton)), progress, SLOT(hideTip()));//20170714

    QHBoxLayout *time_layout = new QHBoxLayout();
    time_layout->setMargin(0);
    time_layout->setSpacing(2);
    time_layout->addWidget(playtime_label, 0, Qt::AlignLeft | Qt::AlignVCenter);
    time_layout->addWidget(alltime_label, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metaWidget = new QFrame(this);
    metaWidget->setFixedHeight(61);
    QHBoxLayout *metaLayout = new QHBoxLayout(metaWidget);
    metaLayout->setMargin(0);
    metaLayout->setSpacing(10);
    metaLayout->addWidget(btPlayPause, 0, Qt::AlignVCenter);
    metaLayout->addLayout(time_layout);

    ctlWidget = new QFrame(this);
    ctlWidget->setFixedHeight(61);
    QHBoxLayout *ctlLayout = new QHBoxLayout(ctlWidget);
    ctlLayout->setMargin(0);
    ctlLayout->setSpacing(30);

    ctlLayout->addWidget(btPrev, 0, Qt::AlignCenter);
    ctlLayout->addWidget(btStop, 0, Qt::AlignCenter);
    ctlLayout->addWidget(btNext, 0, Qt::AlignCenter);
    ctlWidget->adjustSize();

    QHBoxLayout *volume_layout = new QHBoxLayout();
    volume_layout->setMargin(0);
    volume_layout->setSpacing(2);
    volume_layout->addWidget(btSound, 0, Qt::AlignRight | Qt::AlignVCenter);
    volume_layout->addWidget(volSlider, 0, Qt::AlignRight | Qt::AlignVCenter);

    QHBoxLayout *list_layout = new QHBoxLayout();
    list_layout->setMargin(0);
    list_layout->setSpacing(2);
    list_layout->addWidget(btPlayList, 0, Qt::AlignRight | Qt::AlignVCenter);
    list_layout->addWidget(listCountLabel, 0, Qt::AlignRight | Qt::AlignVCenter);

    actWidget = new QWidget(this);
    actWidget->setFixedHeight(61);
    QHBoxLayout *actLayout = new QHBoxLayout(actWidget);
    actLayout->setMargin(0);
    actLayout->setSpacing(20);
    actLayout->addLayout(volume_layout);
    actLayout->addWidget(btFullScreen, 0, Qt::AlignRight | Qt::AlignVCenter);
    actLayout->addLayout(list_layout);

    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp.setHorizontalStretch(33);
    metaWidget->setSizePolicy(sp);
    actWidget->setSizePolicy(sp);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 0, 20, 0);
    layout->setSpacing(20);
    layout->addWidget(metaWidget, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(ctlWidget, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addWidget(actWidget, 0, Qt::AlignRight);
    controlWidget->setLayout(layout);

    vboxlayout = new QVBoxLayout(this);
    vboxlayout->setSpacing(0);
    vboxlayout->setContentsMargins(0, 18, 0, 2);
    vboxlayout->addWidget(controlWidget);

    btStop->setFocusPolicy(Qt::NoFocus);
    btPrev->setFocusPolicy(Qt::NoFocus);
    btPlayPause->setFocusPolicy(Qt::NoFocus);
    btNext->setFocusPolicy(Qt::NoFocus);
    btFullScreen->setFocusPolicy(Qt::NoFocus);
    btSound->setFocusPolicy(Qt::NoFocus);
    volSlider->setFocusPolicy(Qt::NoFocus);
    btPlayList->setFocusPolicy(Qt::NoFocus);

    btStop->setToolTip(tr("Stop"));
    btPrev->setToolTip(tr("Prev"));
    btPlayPause->setToolTip(tr("Play / Pause"));
    btNext->setToolTip(tr("Next"));
    btSound->setToolTip(tr("Mute"));
    btPlayList->setToolTip(tr("Play List"));

    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPause);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPause);
}

BottomWidget::~BottomWidget() {
    if (spreadAnimation) delete spreadAnimation;
    if (gatherAnimation) delete gatherAnimation;

    if (metaWidget) delete metaWidget;
    if (ctlWidget) delete ctlWidget;
    if (actWidget) delete actWidget;
    if (controlWidget) delete controlWidget;
}

void BottomWidget::setPreviewData(bool preview) {
    if (progress) {
        progress->set_preview_flag(preview);
    }
}

void BottomWidget::update_playlist_count_label(int count) {
    this->listCountLabel->setText(QString::number(count));
}

void BottomWidget::setTransparent(bool transparent) {
    if (transparent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
        set_widget_opacity(0.5);
    }
    else {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void BottomWidget::set_widget_opacity(const float &opacity) {
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void BottomWidget::slot_active_status(bool b) {
    if (b) {
        this->update_widget_qss_property(progress, "status", "active");
    }
    else {
        this->update_widget_qss_property(progress, "status", "");
    }
}

void BottomWidget::setPos(int v) {
    bool was_blocked= progress->blockSignals(true);
    progress->setPos(v);
    progress->blockSignals(was_blocked);
}

int BottomWidget::ppos() {
    return progress->pos();
}

void BottomWidget::setDuration(double t) {
    total_time = t;
    progress->setDuration(t);
}

void BottomWidget::setDragDelay(int d) {
    drag_delay = d;
    progress->setDragDelay(drag_delay);
}

int BottomWidget::dragDelay() {
    return drag_delay;
}

void BottomWidget::setActionsEnabled(bool b) {
    btPrev->setEnabled(b);
    btPlayPause->setEnabled(b);
    btNext->setEnabled(b);
    btSound->setEnabled(b);
    btStop->setEnabled(b);
    progress->setEnabled(b);
}

void BottomWidget::setPlayOrPauseEnabled(bool b) {
    btPlayPause->setEnabled(b);
}

void BottomWidget::setStopEnabled(bool b) {
    btStop->setEnabled(b);
}

void BottomWidget::displayTime(QString cur_time, QString all_time) {
    playtime_label->setText(cur_time);
    alltime_label->setText(all_time);
}

void BottomWidget::slot_playlist_btn_status(bool b) {
    if (b) {
        this->update_widget_qss_property(btPlayList, property_PlayListStatus, playStatusPlayList);
    }
    else {
        this->update_widget_qss_property(btPlayList, property_PlayListStatus, "");
    }
}

void BottomWidget::slot_volumn_changed(int vol) {
    this->onVolumeChanged(vol);
    emit this->volumeChanged(vol);
}

void BottomWidget::onMusicPlayed() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPlaying);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPlaying);
}

void BottomWidget::onMusicPause() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusPause);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusPause);
}

void BottomWidget::onMusicStoped() {
    this->update_widget_qss_property(btPlayPause, property_PlayStatus, playStatusStop);
    this->update_widget_qss_property(this, property_PlayStatus, playStatusStop);
}

void BottomWidget::onFullScreen() {
    this->update_widget_qss_property(btFullScreen, property_FullScreen, true);
}

void BottomWidget::onUnFullScreen() {
    this->update_widget_qss_property(btFullScreen, property_FullScreen, false);
}

void BottomWidget::onVolumeChanged(int volume) {
    QString status = "mid";
    if (volume > 77) {
        status = "high";
    } else if (volume > 33) {
        status = "mid";
    } else if (volume > 0) {
        status = "low";
    } else  {
        status = "mute";
    }
    this->update_widget_qss_property(btSound, "volume", status);
    this->volSlider->onVolumeChanged(volume);
}

void BottomWidget::onMutedChanged(bool muted, int volumn) {
    if (muted) {
        this->update_widget_qss_property(btSound, "volume", "mute");
        this->volSlider->onVolumeChanged(0);
    }
    else {
        this->onVolumeChanged(volumn);
    }
}

void BottomWidget::update_widget_qss_property(QWidget *w, const char *name, const QVariant &value) {
    w->setProperty(name, value);
    this->style()->unpolish(w);
    this->style()->polish(w);
    w->update();
}

void BottomWidget::showAlways() {
    turned_on = false;
    QPoint dest_position = QPoint(0, parentWidget()->size().height() - this->height());
    move(dest_position);
    controlWidget->show();
    QWidget::show();
}

void BottomWidget::enable_turned_on() {
    turned_on = true;
}

bool BottomWidget::getCtrlWidgetVisible()
{
    return this->ctlWidget->isVisible();
}

void BottomWidget::spreadAniFinished() {
}

void BottomWidget::gatherAniFinished()
{
    controlWidget->hide();
}

void BottomWidget::show_control_widget()
{
    controlWidget->show();
}

void BottomWidget::showSpreadAnimated() {
    if (!spreadAnimation) {
        spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), parentWidget()->size().height());
    QPoint final_position = QPoint(0, parentWidget()->size().height() - this->height());
    move(initial_position);

    QWidget::show();
    controlWidget->show();

    spreadAnimation->setDuration(300);
    spreadAnimation->setStartValue(initial_position);
    spreadAnimation->setEndValue(final_position);
    spreadAnimation->start();

    //kobe: tell mainwindow to show escwidget
    emit this->sig_show_or_hide_esc(true);
}


void BottomWidget::showGatherAnimated() {
    if (!gatherAnimation) {
        gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, parentWidget()->size().height() - this->height());
    QPoint final_position = QPoint(pos().x(), parentWidget()->size().height() - 4);//kobe 0616:给最底下的进度条留下空间
    move(initial_position);

    gatherAnimation->setDuration(300);
    gatherAnimation->setStartValue(initial_position);
    gatherAnimation->setEndValue(final_position);
    gatherAnimation->start();
}

void BottomWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit sig_resize_corner();
    progress->resize(this->width(), 24);
}
*/
