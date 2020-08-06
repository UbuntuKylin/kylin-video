/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
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

#include "videowindow.h"
#include "displaylayercomposer.h"

#include "smplayer/global.h"
#include "smplayer/desktopinfo.h"
#include "smplayer/colorutils.h"
#include "smplayer/images.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QDesktopWidget>
#include <QPropertyAnimation>

VideoWindow::VideoWindow(QWidget* parent, Qt::WindowFlags f)
    : AutoHideCursorWidget(parent, f)
    , m_videoWidth(0)
    , m_videoHeight(0)
    , m_aspect((double) 4/3)
    , m_monitorAspect(0)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_zoomFactor(1.0)
    , m_origX(0)
    , m_origY(0)
    , m_origWidth(0)
    , m_origHeight(0)
    , m_allowVideoMovement(false)
    , m_animatedLogo(false)
    , m_doubleClicked(false)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
    , m_mouseDragTracking(false)
    , m_stoped(true)
{
    this->setMouseTracking(true);
    this->setMinimumSize(QSize(0,0));
    this->setAutoFillBackground(true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFocusPolicy(Qt::StrongFocus);
//    this->setStyleSheet("QWidget{border:none; border-radius:6px;}");//background-color: transparent;

    ColorUtils::setBackgroundColor( this, QColor(0,0,0) );

    m_displayLayer = new DisplayLayerComposer(this);
    m_displayLayer->setObjectName("m_displayLayer");
    m_displayLayer->setAutoFillBackground(true);
//    m_displayLayer->setStyleSheet("QWidget{border:none; border-radius:6px;}");

    m_logoLabel = new QLabel(m_displayLayer);
    m_logoLabel->setAutoFillBackground(true);
    ColorUtils::setBackgroundColor(m_logoLabel, QColor(0,0,0) );

    QVBoxLayout * mplayerlayerLayout = new QVBoxLayout(m_displayLayer);
    mplayerlayerLayout->addWidget(m_logoLabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    this->installEventFilter(this);
    m_displayLayer->installEventFilter(this);

    //为了避免双击时触发单击事件，在单击处理函数clicked()中启动一timer，延时 qApp->doubleClickInterval()，而在此timer的timeout()中处理单击事件，在双击处理函数停止此timer
    m_leftClickTimer = new QTimer(this);
    m_leftClickTimer->setSingleShot(true);
    m_leftClickTimer->setInterval(qApp->doubleClickInterval()+10);//1
    connect(m_leftClickTimer, SIGNAL(timeout()), this, SIGNAL(leftClicked()));

    m_logoLabel->setPixmap(Images::icon("background") );//kobe:设置显示区域的背景图 :/default-theme/background.png
}

VideoWindow::~VideoWindow()
{
    if (m_leftClickTimer != NULL) {
        disconnect(m_leftClickTimer, SIGNAL(timeout()), this, SIGNAL(leftClicked()));
        if(m_leftClickTimer->isActive()) {
            m_leftClickTimer->stop();
        }
        delete m_leftClickTimer;
        m_leftClickTimer = NULL;
    }
}

void VideoWindow::setCornerWidget(QWidget * w)
{
    m_cornerWidget = w;
//    m_cornerWidget->setStyleSheet("QWidget{border:none; border-radius:6px;}");
    QHBoxLayout * blayout = new QHBoxLayout;
    blayout->addStretch();
    blayout->addWidget(m_cornerWidget);
    blayout->addStretch();
    this->setLayout(blayout);
}

void VideoWindow::setColorKey( QColor c )
{
    ColorUtils::setBackgroundColor(m_displayLayer, c);
}

void VideoWindow::setLogoVisible(bool b)
{
    if (b) m_displayLayer->setUpdatesEnabled(true);
    /*if (m_cornerWidget) {// m_cornerWidget is playmask widget
        m_cornerWidget->setVisible(b);
    }*/
    if (b) {
        m_displayLayer->move(0,0);
        m_displayLayer->resize(this->size());
    }
    m_stoped = b;

    m_logoLabel->setVisible(b);

    /*if (b) {
        m_logoLabel->show();
        QPropertyAnimation * animation = new QPropertyAnimation(m_logoLabel, "pos");
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setStartValue(QPoint(m_logoLabel->x(), 0 - m_logoLabel->y()));
        animation->setEndValue(m_logoLabel->pos());
        animation->start();
    }
    else {
        QPropertyAnimation * animation = new QPropertyAnimation(m_logoLabel, "pos");
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setEndValue(QPoint(width(), m_logoLabel->y()));
        animation->setStartValue(m_logoLabel->pos());
        animation->start();
        connect(animation, SIGNAL(finished()), m_logoLabel, SLOT(hide()));
    }*/
}

void VideoWindow::setResolution( int w, int h)
{
    m_videoWidth = w;
    m_videoHeight = h;

    updateVideoWindow();
}

void VideoWindow::resizeEvent( QResizeEvent *e)
{
     m_offsetX = 0;
     m_offsetY = 0;

     updateVideoWindow();
     setZoom(m_zoomFactor);
}

void VideoWindow::hideLogoForTemporary()
{
    m_logoLabel->setVisible(false);
}

void VideoWindow::updateLogoPosition()
{
    if (m_stoped) {
        m_displayLayer->move(0,0);
        m_displayLayer->resize(this->size());
        m_logoLabel->show();
        QPropertyAnimation * animation = new QPropertyAnimation(m_logoLabel, "pos");
        animation->setDuration(500);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setStartValue(QPoint(m_logoLabel->x(), 0 - m_logoLabel->y()/2));
        animation->setEndValue(m_logoLabel->pos());
        animation->start();
    }
}

void VideoWindow::setMonitorAspect(double asp)
{
    m_monitorAspect = asp;
}

void VideoWindow::setAspect( double asp)
{
    m_aspect = asp;
    if (m_monitorAspect!=0) {
        m_aspect = m_aspect / m_monitorAspect * DesktopInfo::desktop_aspectRatio(this);
    }
    updateVideoWindow();
}

void VideoWindow::updateVideoWindow()
{
    int w_width = size().width();
    int w_height = size().height();

    //-----------------------add by zhaoyubiao-----------------------
    //When no fullscreen, Reduce the videowindow height so that the videowindow is not blocked
    if(!window()->isFullScreen())
    {
        w_height = size().height()*0.9 - 20;
    }
    //------------------------add end--------------------------------

    int w = w_width;
    int h = w_height;
    int x = 0;
    int y = 0;

    if (m_aspect != 0) {
        int pos1_w = w_width;
        int pos1_h = w_width / m_aspect + 0.5;

        int pos2_h = w_height;
        int pos2_w = w_height * m_aspect + 0.5;

        if (pos1_h <= w_height) {
            w = pos1_w;
            h = pos1_h;
            y = (w_height - h) /2;
        }
        else {
            w = pos2_w;
            h = pos2_h;
            x = (w_width - w) /2;
        }
    }

    m_displayLayer->move(x,y);
    m_displayLayer->resize(w, h);
    m_origX = x;
    m_origY = y;
    m_origWidth = w;
    m_origHeight = h;
}

void VideoWindow::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton) {
        e->accept();
        if (!m_doubleClicked) m_leftClickTimer->start();
        m_doubleClicked = false;
    }
    else
    if (e->button() == Qt::MidButton) {
        e->accept();
        emit middleClicked();
    }
    else
    if (e->button() == Qt::XButton1) {
        e->accept();
        emit xbutton1Clicked();
    }
    else
    if (e->button() == Qt::XButton2) {
        e->accept();
        emit xbutton2Clicked();
    }
    else
    if (e->button() == Qt::RightButton) {
        e->accept();
        emit rightClicked();
    }
    else {
        e->ignore();
    }
}

void VideoWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        m_leftClickTimer->stop();
        m_doubleClicked = true;
        emit doubleClicked();
    } else {
        event->ignore();
    }
}

void VideoWindow::wheelEvent(QWheelEvent *e)
{
    e->accept();

    if (e->orientation() == Qt::Vertical) {
        if (e->delta() >= 0)
            emit wheelUp();
        else
            emit wheelDown();
    } else {
        qDebug("VideoWindow::wheelEvent: horizontal event received, doing nothing");
    }
}

QSize VideoWindow::sizeHint() const
{
    return QSize(m_videoWidth, m_videoHeight);
}

QSize VideoWindow::minimumSizeHint () const
{
    return QSize(0,0);
}

void VideoWindow::setOffsetX( int d)
{
    m_offsetX = d;
    m_displayLayer->move(m_origX + m_offsetX, m_displayLayer->y());
}

int VideoWindow::offsetX()
{
    return m_offsetX;
}

void VideoWindow::setOffsetY(int d)
{
    m_offsetY = d;
    m_displayLayer->move(m_displayLayer->x(), m_origY + m_offsetY);
}

int VideoWindow::offsetY()
{
    return m_offsetY;
}

void VideoWindow::setZoom(double d)
{
    m_zoomFactor = d;
    m_offsetX = 0;
    m_offsetY = 0;

    int x = m_origX;
    int y = m_origY;
    int w = m_origWidth;
    int h = m_origHeight;

    if (m_zoomFactor != 1.0) {
        w = w * m_zoomFactor;
        h = h * m_zoomFactor;

        // Center
        x = (width() - w) / 2;
        y = (height() -h) / 2;
    }

    m_displayLayer->move(x,y);
    m_displayLayer->resize(w,h);
}

double VideoWindow::zoom()
{
    return m_zoomFactor;
}

void VideoWindow::moveDisplayLayer(int m_offsetX, int m_offsetY)
{
    int x = m_displayLayer->x();
    int y = m_displayLayer->y();

    m_displayLayer->move(x + m_offsetX, y + m_offsetY);
}

void VideoWindow::moveLeft()
{
    if ((m_allowVideoMovement) || (m_displayLayer->x()+m_displayLayer->width() > width()))
        moveDisplayLayer(-16, 0);
}

void VideoWindow::moveRight()
{
    if ((m_allowVideoMovement) || ( m_displayLayer->x() < 0))
        moveDisplayLayer(+16, 0);
}

void VideoWindow::moveUp()
{
    if ((m_allowVideoMovement) || (m_displayLayer->y()+m_displayLayer->height() > height()))
        moveDisplayLayer(0, -16);
}

void VideoWindow::moveDown()
{
    if ((m_allowVideoMovement) || ( m_displayLayer->y() < 0))
        moveDisplayLayer(0, +16);
}

void VideoWindow::incZoom()
{
    setZoom(m_zoomFactor + ZOOM_STEP);
}

void VideoWindow::decZoom()
{
    double zoom = m_zoomFactor - ZOOM_STEP;
    if (zoom < ZOOM_MIN)
        zoom = ZOOM_MIN;
    setZoom(zoom);
}


bool VideoWindow::eventFilter(QObject *object, QEvent *event)
{
    if (!m_mouseDragTracking)
        return false;

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
        if (abs(diff.x()) < DRAG_THRESHOLD && abs(diff.y()) < DRAG_THRESHOLD)
            return false;

        m_dragState = DRAGGING;
    }

    emit mouseMovedDiff(diff);
    m_startDrag = pos;
    event->accept();
    return true;
}
