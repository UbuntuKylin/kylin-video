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

#include "bottomwidget.h"
#include "soundvolume.h"

#include <QProcess>
#include <QResizeEvent>
#include <QShortcut>
#include <QStyleFactory>
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

#include "../smplayer/timeslider.h"
#include "../smplayer/colorutils.h"
#include "../smplayer/myaction.h"
#include "../smplayer/global.h"
#include "../smplayer/preferences.h"

using namespace Global;

static const QString playStatusPlaying = "playing";
static const QString playStatusPause = "pause";
static const QString playStatusStop = "stop";
static const QString playStatusPlayList = "close";
static const char *property_FullScreen = "fullscreen";
static const char *property_PlayStatus = "playstatus";
static const char *property_PlayListStatus = "playliststatus";

BottomWidget::BottomWidget(QWidget *parent)
    : QWidget(parent, Qt::SubWindow)
    , m_spreadAnimation(0)
    , m_gatherAnimation(0)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
    , m_dragDelay(200)
{
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setAttribute(Qt::WA_TranslucentBackground, true);//窗体标题栏不透明，背景透明
    this->setWindowFlags(windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
    this->installEventFilter(this);

    m_controlWidget = new QWidget(this);
    m_controlWidget->setFixedHeight(61);

    m_timeProgress = new TimeSlider(this);
    m_timeProgress->setMouseTracking(true);
    m_timeProgress->setDragDelay(100);//pref->time_slider_drag_delay
    connect(m_timeProgress, SIGNAL(posChanged(int)), this, SIGNAL(posChanged(int)));
    connect(m_timeProgress, SIGNAL(draggingPos(int)), this, SIGNAL(draggingPos(int)));
    connect(m_timeProgress, SIGNAL(delayedDraggingPos(int)), this, SIGNAL(delayedDraggingPos(int)));
    connect(m_timeProgress, SIGNAL(wheelUp()), this, SIGNAL(wheelUp()));
    connect(m_timeProgress, SIGNAL(wheelDown()), this, SIGNAL(wheelDown()));
    connect(m_timeProgress, SIGNAL(requestSavePreviewImage(int)), this, SIGNAL(requestSavePreviewImage(int)));
    m_timeProgress->setObjectName("processProgress");
    m_timeProgress->setProperty("status", "");
    connect(m_timeProgress, SIGNAL(active_status(bool)), this, SLOT(onProgressActiveStatus(bool)));
    m_timeProgress->setFixedHeight(24);
    m_timeProgress->move(0,0);
    m_playtimeLabel = new QLabel;
    m_playtimeLabel->setText("00:00:00");
    m_playtimeLabel->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor(m_playtimeLabel, QColor(0,0,0) );
    ColorUtils::setForegroundColor(m_playtimeLabel, QColor(255,255,255) );
    m_playtimeLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    m_playtimeLabel->adjustSize();
    m_totaltimeLabel = new QLabel;
    m_totaltimeLabel->setText(" / 00:00:00");
    m_totaltimeLabel->setFrameShape(QFrame::NoFrame);
    ColorUtils::setBackgroundColor( m_totaltimeLabel, QColor(0,0,0) );
    ColorUtils::setForegroundColor( m_totaltimeLabel, QColor(255,255,255) );
    m_totaltimeLabel->setStyleSheet("QLabel{font-size:12px;color:#808080;}");
    m_totaltimeLabel->adjustSize();
    m_btnStop = new QPushButton;
    m_btnStop->setObjectName("StopBtn");
    m_btnStop->setFixedSize(24, 24);
    m_btnPrev = new QPushButton;
    m_btnPrev->setObjectName("PrevBtn");
    m_btnPrev->setFixedSize(24, 24);
    m_btnPlayPause = new QPushButton;
//    m_btnPlayPause->setShortcut(Qt::Key_Space);
    m_btnPlayPause->setObjectName("PlayBtn");
    m_btnPlayPause->setFixedSize(61, 61);
    m_btnNext = new QPushButton;
    m_btnNext->setObjectName("NextBtn");
    m_btnNext->setFixedSize(24, 24);
    m_btnSound = new QPushButton;
    m_btnSound->setObjectName("SoundBtn");
    m_btnSound->setFixedSize(24, 24);
    m_btnSound->setProperty("volume", "mid");
    m_btnFullScreen = new QPushButton;
//    m_btnFullScreen->setShortcut(QKeySequence("Ctrl+Return"));
    m_btnFullScreen->setObjectName("FullScreenBtn");
    m_btnFullScreen->setFixedSize(24, 24);
    m_btnPlayList = new QPushButton;
//    m_btnPlayList->setShortcut(QKeySequence("F3"));
    m_btnPlayList->setObjectName("PlayListBtn");
    m_btnPlayList->setFixedSize(24, 24);
    m_listCountLabel = new QLabel;
    ColorUtils::setBackgroundColor(m_listCountLabel, QColor("#141414") );
    m_listCountLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    this->m_listCountLabel->setText("0");
    m_volSlider = new SoundVolume(this);
    m_volSlider->setObjectName("VolumeSlider");
    connect(this, SIGNAL(valueChanged(int)), m_volSlider, SLOT(setValue(int)));
    connect(m_btnStop, SIGNAL(released()), this, SIGNAL(toggleStop()));
    connect(m_btnPrev, SIGNAL(released()), this, SIGNAL(togglePrev()));
    connect(m_btnPlayPause, SIGNAL(released()), this, SIGNAL(togglePlayPause()));
    connect(m_btnNext, SIGNAL(released()), this, SIGNAL(toggleNext()));
    connect(m_btnSound, SIGNAL(pressed()), this, SIGNAL(toggleMute()));
    connect(m_btnFullScreen, SIGNAL(pressed()), this, SIGNAL(toggleFullScreen()));
    connect(m_btnPlayList, SIGNAL(released()), this, SIGNAL(togglePlaylist()));
    connect(this, SIGNAL(mouseMoving(Qt::MouseButton)), m_timeProgress, SLOT(hideTip()));
    connect(m_volSlider, &SoundVolume::volumeChanged, this, [=] (int vol) {
        this->onVolumeChanged(vol);
        emit this->requestVolumeChanged(vol);
    });

    QHBoxLayout *time_layout = new QHBoxLayout();
    time_layout->setMargin(0);
    time_layout->setSpacing(2);
    time_layout->addWidget(m_playtimeLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    time_layout->addWidget(m_totaltimeLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    m_metaWidget = new QFrame(this);
    m_metaWidget->setFixedHeight(61);
    QHBoxLayout *metaLayout = new QHBoxLayout(m_metaWidget);
    metaLayout->setMargin(0);
    metaLayout->setSpacing(10);
    metaLayout->addWidget(m_btnPlayPause, 0, Qt::AlignVCenter);
    metaLayout->addLayout(time_layout);

    m_ctlWidget = new QFrame(this);
    m_ctlWidget->setFixedHeight(61);
    QHBoxLayout *ctlLayout = new QHBoxLayout(m_ctlWidget);
    ctlLayout->setMargin(0);
    ctlLayout->setSpacing(30);

    ctlLayout->addWidget(m_btnPrev, 0, Qt::AlignCenter);
    ctlLayout->addWidget(m_btnStop, 0, Qt::AlignCenter);
    ctlLayout->addWidget(m_btnNext, 0, Qt::AlignCenter);
    m_ctlWidget->adjustSize();

    QHBoxLayout *volume_layout = new QHBoxLayout();
    volume_layout->setMargin(0);
    volume_layout->setSpacing(2);
    volume_layout->addWidget(m_btnSound, 0, Qt::AlignRight | Qt::AlignVCenter);
    volume_layout->addWidget(m_volSlider, 0, Qt::AlignRight | Qt::AlignVCenter);

    QHBoxLayout *list_layout = new QHBoxLayout();
    list_layout->setMargin(0);
    list_layout->setSpacing(2);
    list_layout->addWidget(m_btnPlayList, 0, Qt::AlignRight | Qt::AlignVCenter);
    list_layout->addWidget(m_listCountLabel, 0, Qt::AlignRight | Qt::AlignVCenter);

    m_actWidget = new QWidget(this);
    m_actWidget->setFixedHeight(61);
    QHBoxLayout *actLayout = new QHBoxLayout(m_actWidget);
    actLayout->setMargin(0);
    actLayout->setSpacing(20);
    actLayout->addLayout(volume_layout);
    actLayout->addWidget(m_btnFullScreen, 0, Qt::AlignRight | Qt::AlignVCenter);
    actLayout->addLayout(list_layout);

    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp.setHorizontalStretch(33);
    m_metaWidget->setSizePolicy(sp);
    m_actWidget->setSizePolicy(sp);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 0, 20, 0);
    layout->setSpacing(20);
    layout->addWidget(m_metaWidget, 0, Qt::AlignLeft/* | Qt::AlignVCenter*/);
    layout->addStretch();
    layout->addWidget(m_ctlWidget, 0, Qt::AlignHCenter/*Qt::AlignCenter*/);
    layout->addStretch();
    layout->addWidget(m_actWidget, 0, Qt::AlignRight/* | Qt::AlignVCenter*/);
    m_controlWidget->setLayout(layout);

    vboxlayout = new QVBoxLayout(this);
    vboxlayout->setSpacing(0);
    vboxlayout->setContentsMargins(0, 18, 0, 2);
    vboxlayout->addWidget(m_controlWidget);

    m_btnStop->setFocusPolicy(Qt::NoFocus);
    m_btnPrev->setFocusPolicy(Qt::NoFocus);
    m_btnPlayPause->setFocusPolicy(Qt::NoFocus);
    m_btnNext->setFocusPolicy(Qt::NoFocus);
    m_btnFullScreen->setFocusPolicy(Qt::NoFocus);
    m_btnSound->setFocusPolicy(Qt::NoFocus);
    m_volSlider->setFocusPolicy(Qt::NoFocus);
    m_btnPlayList->setFocusPolicy(Qt::NoFocus);

    m_btnStop->setToolTip(tr("Stop"));
    m_btnPrev->setToolTip(tr("Prev"));
    m_btnPlayPause->setToolTip(tr("Play / Pause"));
    m_btnNext->setToolTip(tr("Next"));
    m_btnSound->setToolTip(tr("Mute"));
    m_btnPlayList->setToolTip(tr("Play List"));

    this->updateWidgetQssProperty(m_btnPlayPause, property_PlayStatus, playStatusPause);
    this->updateWidgetQssProperty(this, property_PlayStatus, playStatusPause);

    Utils::setWidgetOpacity(this, true, 0.8);
}

BottomWidget::~BottomWidget()
{
    if (m_spreadAnimation) delete m_spreadAnimation;
    if (m_gatherAnimation) delete m_gatherAnimation;

    if (m_metaWidget) delete m_metaWidget;
    if (m_ctlWidget) delete m_ctlWidget;
    if (m_actWidget) delete m_actWidget;
    if (m_controlWidget) delete m_controlWidget;
}

void BottomWidget::setPreviewData(bool preview)
{
    if (m_timeProgress) {
        m_timeProgress->set_preview_flag(preview);
    }
}

void BottomWidget::savePreviewImageName(int time, QString filepath)
{
    if (m_timeProgress) {
        m_timeProgress->show_save_preview_image(time, filepath);
    }
}

QString BottomWidget::getActiveStatus()
{
    QString status = m_timeProgress->property("active").toString();
    return status;
}

void BottomWidget::updateLabelCountNumber(int count)
{
    this->m_listCountLabel->setText(QString::number(count));
}

void BottomWidget::onProgressActiveStatus(bool b)
{
    if (b) {
        this->updateWidgetQssProperty(m_timeProgress, "status", "active");
    }
    else {
        this->updateWidgetQssProperty(m_timeProgress, "status", "");
    }
}

void BottomWidget::setPos(int v)
{
    bool was_blocked= m_timeProgress->blockSignals(true);
    m_timeProgress->setPos(v);
    m_timeProgress->blockSignals(was_blocked);
}

int BottomWidget::ppos()
{
    return m_timeProgress->pos();
}

void BottomWidget::setDuration(double t)
{
    m_totalTime = t;
    m_timeProgress->setDuration(t);
}

void BottomWidget::setDragDelay(int d)
{
    m_dragDelay = d;
    m_timeProgress->setDragDelay(m_dragDelay);
}

int BottomWidget::dragDelay()
{
    return m_dragDelay;
}

void BottomWidget::onSetActionsEnabled(bool b)
{
    m_btnPrev->setEnabled(b);
    m_btnPlayPause->setEnabled(b);
    m_btnNext->setEnabled(b);
    m_btnSound->setEnabled(b);
    m_btnStop->setEnabled(b);
    m_timeProgress->setEnabled(b);
}

void BottomWidget::setPlayOrPauseEnabled(bool b)
{
    m_btnPlayPause->setEnabled(b);
}

void BottomWidget::setStopEnabled(bool b)
{
    m_btnStop->setEnabled(b);
}

void BottomWidget::displayTime(QString cur_time, QString all_time)
{
    m_playtimeLabel->setText(cur_time);
    m_totaltimeLabel->setText(all_time);
}

void BottomWidget::updatePlaylistBtnQssProperty(bool b)
{
    if (b) {
        this->updateWidgetQssProperty(m_btnPlayList, property_PlayListStatus, playStatusPlayList);
    }
    else {
        this->updateWidgetQssProperty(m_btnPlayList, property_PlayListStatus, "");
    }
}

void BottomWidget::onMusicPlayed()
{
    this->updateWidgetQssProperty(m_btnPlayPause, property_PlayStatus, playStatusPlaying);
    this->updateWidgetQssProperty(this, property_PlayStatus, playStatusPlaying);
}

void BottomWidget::onMusicPause()
{
    this->updateWidgetQssProperty(m_btnPlayPause, property_PlayStatus, playStatusPause);
    this->updateWidgetQssProperty(this, property_PlayStatus, playStatusPause);
}

void BottomWidget::onMusicStoped()
{
    this->updateWidgetQssProperty(m_btnPlayPause, property_PlayStatus, playStatusStop);
    this->updateWidgetQssProperty(this, property_PlayStatus, playStatusStop);
}

void BottomWidget::onFullScreen()
{
    this->updateWidgetQssProperty(m_btnFullScreen, property_FullScreen, true);
}

void BottomWidget::onUnFullScreen()
{
    this->updateWidgetQssProperty(m_btnFullScreen, property_FullScreen, false);
}

void BottomWidget::onVolumeChanged(int volume)
{
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
    this->updateWidgetQssProperty(m_btnSound, "volume", status);
    this->m_volSlider->onVolumeChanged(volume);
}

void BottomWidget::onMutedChanged(bool muted, int volumn)
{
    if (muted) {
        this->updateWidgetQssProperty(m_btnSound, "volume", "mute");
        this->m_volSlider->onVolumeChanged(0);
    }
    else {
        this->onVolumeChanged(volumn);
    }
}

void BottomWidget::updateWidgetQssProperty(QWidget *w, const char *name, const QVariant &value)
{
    w->setProperty(name, value);
    this->style()->unpolish(w);
    this->style()->polish(w);
    w->update();
}

void BottomWidget::checkUnderMouse()
{
    if ((m_ctlWidget->isVisible()) && (!underMouse())) {
        this->showGatherAnimated();
        //tell mainwindow to hide escwidget
        emit this->requestShowOrHideEscWidget(false);
    }
}

void BottomWidget::spreadAniFinished()
{
}

void BottomWidget::gatherAniFinished()
{
    m_controlWidget->hide();
}

void BottomWidget::onShowControlWidget()
{
    m_controlWidget->show();
}

void BottomWidget::showSpreadAnimated()
{
    if (!m_spreadAnimation) {
        m_spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(m_spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), parentWidget()->size().height());
    QPoint final_position = QPoint(0, parentWidget()->size().height() - this->height());
    move(initial_position);

    QWidget::show();
    m_controlWidget->show();

    m_spreadAnimation->setDuration(300);
    m_spreadAnimation->setStartValue(initial_position);
    m_spreadAnimation->setEndValue(final_position);
    m_spreadAnimation->start();

    //tell mainwindow to show escwidget
    emit this->requestShowOrHideEscWidget(true);
}

void BottomWidget::showGatherAnimated()
{
    if (!m_gatherAnimation) {
        m_gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(m_gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, parentWidget()->size().height() - this->height());
    QPoint final_position = QPoint(pos().x(), parentWidget()->size().height() - 4);//kobe 0616:给最底下的进度条留下空间
    move(initial_position);

    m_gatherAnimation->setDuration(300);
    m_gatherAnimation->setStartValue(initial_position);
    m_gatherAnimation->setEndValue(final_position);
    m_gatherAnimation->start();
}

void BottomWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_timeProgress->resize(this->width(), 24);
}

void BottomWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);

    emit mouseEnter();
}

void BottomWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    emit mouseLeave();
}

void BottomWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}

bool BottomWidget::eventFilter(QObject * obj, QEvent * event)
{
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;
    if (mouseEvent->modifiers() != Qt::NoModifier) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    if (event->type() == QEvent::MouseMove) {
        if (!isVisible()) {
            emit this->requestTemporaryShow();
        }
        else if (!m_ctlWidget->isVisible()) {
            emit this->requestTemporaryShow();
        }
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        m_dragState = START_DRAGGING;
        m_startDrag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (m_dragState != DRAGGING || mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        // Stop dragging and eat event
        m_dragState = NOT_DRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (m_dragState == NOT_DRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - m_startDrag;
    if (m_dragState == START_DRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        m_dragState = DRAGGING;
    }

    emit mouseMovedDiff(diff);
    m_startDrag = pos;

    event->accept();
    return true;
}
