/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "mplayerwindow.h"
#include "global.h"
#include "desktopinfo.h"
#include "colorutils.h"

//#ifndef MINILIB
#include "images.h"
//#endif

#include <QLabel>
#include <QTimer>
#include <QCursor>
#include <QEvent>
#include <QLayout>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QDesktopWidget>
#include <QPropertyAnimation>

Screen::Screen(QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f )
	, check_mouse_timer(0)
	, mouse_last_position(QPoint(0,0))
	, autohide_cursor(false)
	, autohide_interval(0)
{
	setMouseTracking(true);
	setFocusPolicy( Qt::NoFocus );
	setMinimumSize( QSize(0,0) );

//    this->setWindowOpacity(0.5);
//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
//    setPalette(pal);
//    QPalette bgpal = palette();
//    bgpal.setColor (QPalette::Background, QColor (0, 0 , 0, 255));//背景刷成黑色
//    //bgpal.setColor (QPalette::Background, Qt::transparent);
//    bgpal.setColor (QPalette::Foreground, QColor (255,255,255,255));//前景色设为白色
//    setPalette (bgpal);

//    this->setAutoFillBackground(true);//20170615

	check_mouse_timer = new QTimer(this);
    connect(check_mouse_timer, SIGNAL(timeout()), this, SLOT(checkMousePos()) );

	setAutoHideInterval(1000);
	setAutoHideCursor(false);
}

Screen::~Screen() {
}

void Screen::setAutoHideCursor(bool b) {
//    qDebug("Screen::setAutoHideCursor: %d", b);

	autohide_cursor = b;
	if (autohide_cursor) {
		check_mouse_timer->setInterval(autohide_interval);
		check_mouse_timer->start();
	} else {
		check_mouse_timer->stop();
	}
}

void Screen::checkMousePos() {
//    qDebug("Screen::checkMousePos");

	if (!autohide_cursor) {
		setCursor(QCursor(Qt::ArrowCursor));
		return;
	}

	QPoint pos = mapFromGlobal(QCursor::pos());

//    qDebug("Screen::checkMousePos: x: %d, y: %d", pos.x(), pos.y());

	if (mouse_last_position != pos) {
//        qDebug("Screen::checkMousePos  ArrowCursor");
		setCursor(QCursor(Qt::ArrowCursor));
	} else {
//        qDebug("Screen::checkMousePos  BlankCursor");
		setCursor(QCursor(Qt::BlankCursor));
	}
	mouse_last_position = pos;
}

void Screen::mouseMoveEvent( QMouseEvent * e ) {
//    qDebug("Screen::mouseMoveEvent");
	emit mouseMoved(e->pos());

	if (cursor().shape() != Qt::ArrowCursor) {
//        emit this->sigShowControls();//0830
//        qDebug(" showing mouse cursor" );
		setCursor(QCursor(Qt::ArrowCursor));
	}
}

void Screen::playingStarted() {
//    qDebug("Screen::playingStarted");
	setAutoHideCursor(true);
}

void Screen::playingStopped() {
//    qDebug("Screen::playingStopped");
	setAutoHideCursor(false);
}

/* ---------------------------------------------------------------------- */

MplayerLayer::MplayerLayer(QWidget* parent, Qt::WindowFlags f)
	: Screen(parent, f)
	, playing(false)
{
	#if QT_VERSION < 0x050000
	setAttribute(Qt::WA_OpaquePaintEvent);
	#if QT_VERSION >= 0x040400
	setAttribute(Qt::WA_NativeWindow);
	#endif
	setAttribute(Qt::WA_PaintUnclipped);
	//setAttribute(Qt::WA_PaintOnScreen);
	#endif
}

MplayerLayer::~MplayerLayer() {
}

void MplayerLayer::paintEvent( QPaintEvent * e ) {
//    qDebug("MplayerLayer::paintEvent: repaint_background: %d", repaint_background);
    if (/*repaint_background || */!playing) {//kobe: if repaint_background is true, Qt5 will call "QPainter::begin: Paint device returned engine == 0, type: 1"
//        qDebug("MplayerLayer::paintEvent: painting");
        QPainter painter(this);
        painter.eraseRect( e->rect() );
        //painter.fillRect( e->rect(), QColor(255,0,0) );
    }
}
//#endif

void MplayerLayer::playingStarted() {
//	qDebug("MplayerLayer::playingStarted");
	repaint();
	playing = true;

    //kobe:WA_PaintOnScreen该属性设置会导致播放音频文件时界面不刷新，此时如果显示或隐藏播放列表，则播放列表重影
//#ifndef Q_OS_WIN
//    setAttribute(Qt::WA_PaintOnScreen);
//#endif
    setAttribute(Qt::WA_NativeWindow, true);

	Screen::playingStarted();
}

void MplayerLayer::playingStopped() {
	qDebug("MplayerLayer::playingStopped");
	playing = false;

    //kobe:WA_PaintOnScreen该属性设置会导致播放音频文件时界面不刷新，此时如果显示或隐藏播放列表，则播放列表重影
//#ifndef Q_OS_WIN
//    setAttribute(Qt::WA_PaintOnScreen, false);
//#endif
    setAttribute(Qt::WA_NativeWindow, false);

    repaint();
	Screen::playingStopped();
}

/* ---------------------------------------------------------------------- */

MplayerWindow::MplayerWindow(QWidget* parent, Qt::WindowFlags f)
	: Screen(parent, f)
	, video_width(0)
	, video_height(0)
	, aspect((double) 4/3)
	, monitoraspect(0)
	, mplayerlayer(0)
    , logo(0)
	, offset_x(0)
	, offset_y(0)
	, zoom_factor(1.0)
	, orig_x(0)
	, orig_y(0)
	, orig_width(0)
	, orig_height(0)
	, allow_video_movement(false)
	, left_click_timer(0)
	, double_clicked(false)
	, corner_widget(0)
    , drag_state(NOT_DRAGGING)
    , start_drag(QPoint(0,0))
    , mouse_drag_tracking(false)
    , stoped(true)
{
	setAutoFillBackground(true);
    ColorUtils::setBackgroundColor( this, QColor(0,0,0) );
//    ColorUtils::setBackgroundColor( this, QColor("#0d87ca") );

	mplayerlayer = new MplayerLayer(this);
	mplayerlayer->setObjectName("mplayerlayer");
	mplayerlayer->setAutoFillBackground(true);

    logo = new QLabel(mplayerlayer);
    logo->setObjectName("mplayerwindow logo");
    logo->setAutoFillBackground(true);
    ColorUtils::setBackgroundColor(logo, QColor(0,0,0) );

    QVBoxLayout * mplayerlayerLayout = new QVBoxLayout(mplayerlayer);
    mplayerlayerLayout->addWidget(logo, 0, Qt::AlignHCenter | Qt::AlignVCenter);

	setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
	setFocusPolicy( Qt::StrongFocus );

	installEventFilter(this);
	mplayerlayer->installEventFilter(this);
	//logo->installEventFilter(this);

    //kobe:为了避免双击时触发单击事件，在单击处理函数clicked()中启动一timer，延时 qApp->doubleClickInterval()，而在此timer的timeout()中处理单击事件，在双击处理函数停止此timer
	left_click_timer = new QTimer(this);
    left_click_timer->setSingleShot(true);
    left_click_timer->setInterval(qApp->doubleClickInterval()+10);//10
    connect(left_click_timer, SIGNAL(timeout()), this, SIGNAL(leftClicked()));//0621

	retranslateStrings();
}

MplayerWindow::~MplayerWindow() {
    if (left_click_timer != NULL) {
        disconnect(left_click_timer, SIGNAL(timeout()), this, SIGNAL(leftClicked()));
        if(left_click_timer->isActive()) {
            left_click_timer->stop();
        }
        delete left_click_timer;
        left_click_timer = NULL;
    }
}

void MplayerWindow::setCornerWidget(QWidget * w) {
	corner_widget = w;

    QHBoxLayout * blayout = new QHBoxLayout;
    blayout->addStretch();
    blayout->addWidget(corner_widget);
    blayout->addStretch();
    this->setLayout(blayout);

//	QHBoxLayout * blayout = new QHBoxLayout;
//	blayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
//	blayout->addWidget(corner_widget);

//	QVBoxLayout * layout = new QVBoxLayout(this);
//	layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
//	layout->addLayout(blayout);
}

void MplayerWindow::setColorKey( QColor c ) {
	ColorUtils::setBackgroundColor( mplayerlayer, c );
}

void MplayerWindow::retranslateStrings() {
    logo->setPixmap(Images::icon("background") );//kobe:设置显示区域的背景图 :/default-theme/background.png
}

void MplayerWindow::setLogoVisible( bool b) {
    if (corner_widget) {
        corner_widget->setVisible(false);
    }
    if (b) {//Fixed bug: 4979
        mplayerlayer->move(0,0);
        mplayerlayer->resize(this->size());
    }
    stoped = b;
    logo->setVisible(b);
}

//void MplayerWindow::show_or_hide_logo(bool b)
//{
//    logo->setVisible(b);
//}

/*
void MplayerWindow::changePolicy() {
	setSizePolicy( QSizePolicy::Preferred , QSizePolicy::Preferred  );
}
*/

void MplayerWindow::setResolution( int w, int h)
{
    video_width = w;
    video_height = h;

    updateVideoWindow();
}

//kobe:窗口尺寸变化响应函数
void MplayerWindow::resizeEvent( QResizeEvent *e)
{
//    qDebug("MplayerWindow::resizeEvent: %d, %d", e->size().width(), e->size().height() );

     offset_x = 0;
     offset_y = 0;

     updateVideoWindow();//kobe:
     setZoom(zoom_factor);//0526
}

void MplayerWindow::hideLogoForTemporary()
{
    logo->setVisible(false);
}

void MplayerWindow::update_logo_pos()
{
    if (stoped) {//Fixed bug: 4979
        mplayerlayer->move(0,0);
        mplayerlayer->resize(this->size());
        logo->show();
        QPropertyAnimation * animation = new QPropertyAnimation(logo, "pos");
        animation->setDuration(500);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setStartValue(QPoint(logo->x(), 0 - logo->y()/2));
        animation->setEndValue(logo->pos());
        animation->start();
//		} else {
//			QPropertyAnimation * animation = new QPropertyAnimation(logo, "pos");
//			animation->setDuration(200);
//			animation->setEasingCurve(QEasingCurve::OutBounce);
//			animation->setEndValue(QPoint(width(), logo->y()));
//			animation->setStartValue(logo->pos());
//			animation->start();
//			connect(animation, SIGNAL(finished()), logo, SLOT(hide()));
//			//logo->hide();
//		}
    }
}

void MplayerWindow::setMonitorAspect(double asp) {
    monitoraspect = asp;
}

void MplayerWindow::setAspect( double asp) {
    aspect = asp;
    if (monitoraspect!=0) {
        aspect = aspect / monitoraspect * DesktopInfo::desktop_aspectRatio(this);
    }
    updateVideoWindow();
}

//kobe:设置视频在屏幕上的显示
void MplayerWindow::updateVideoWindow()
{
    //qDebug("aspect= %f", aspect);
    int w_width = size().width();
    int w_height = size().height();

	int w = w_width;
	int h = w_height;
	int x = 0;
	int y = 0;

	if (aspect != 0) {
	    int pos1_w = w_width;
	    int pos1_h = w_width / aspect + 0.5;
    
	    int pos2_h = w_height;
	    int pos2_w = w_height * aspect + 0.5;
    
//        qDebug("pos1_w: %d, pos1_h: %d", pos1_w, pos1_h);//pos1_w: 700, pos1_h: 298
//        qDebug("pos2_w: %d, pos2_h: %d", pos2_w, pos2_h);//pos2_w: 1602, pos2_h: 681
    
	    if (pos1_h <= w_height) {
		//qDebug("Pos1!");
			w = pos1_w;
			h = pos1_h;
	
			y = (w_height - h) /2;
	    } else {
		//qDebug("Pos2!");
			w = pos2_w;
			h = pos2_h;
	
			x = (w_width - w) /2;
	    }
	}

    mplayerlayer->move(x,y);
    mplayerlayer->resize(w, h);//kobe:主界面全部显示视频刷新时会导致标题栏和控制栏重影  0526
    orig_x = x;
    orig_y = y;
    //kobe: will cause ghosting, why???
    orig_width = w;
    orig_height = h;
//    qDebug( "w_width: %d, w_height: %d", w_width, w_height);//w_width: 700, w_height: 681
//    qDebug("w: %d, h: %d", w,h);//w: 700, h: 298

     emit this->resize_mainwindow(w, h);//add by kobe
}


void MplayerWindow::mouseReleaseEvent( QMouseEvent * e) {
//	qDebug( "MplayerWindow::mouseReleaseEvent" );

	if (e->button() == Qt::LeftButton) {
		e->accept();
//		if (delay_left_click) {
        if (!double_clicked) left_click_timer->start();
        double_clicked = false;
//		} else {
//			emit leftClicked();
//		}
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
		//emit rightButtonReleased( e->globalPos() );
		emit rightClicked();
    } 
	else {
		e->ignore();
	}
}

void MplayerWindow::mouseDoubleClickEvent( QMouseEvent *event) {//0621
    if (event->button() == Qt::LeftButton) {
        event->accept();
//		if (delay_left_click) {
        left_click_timer->stop();
        double_clicked = true;
//		}
		emit doubleClicked();
	} else {
        event->ignore();
	}
}

void MplayerWindow::wheelEvent( QWheelEvent * e ) {
//	qDebug("MplayerWindow::wheelEvent: delta: %d", e->delta());
	e->accept();

	if (e->orientation() == Qt::Vertical) {
	    if (e->delta() >= 0)
	        emit wheelUp();
	    else
	        emit wheelDown();
	} else {
		qDebug("MplayerWindow::wheelEvent: horizontal event received, doing nothing");
	}
}

//kobe:鼠标放上时显示，移开鼠标则消失
bool MplayerWindow::event(QEvent *event) {
//    if (event->type() == QEvent::Enter) {
//      this->escWidget->show();
//        return true;
//    }
    return QWidget::event(event);
}

bool MplayerWindow::eventFilter( QObject * object, QEvent * event ) {
    /*switch (event->type()) {//kobe:0621
    case QEvent::Enter: {
        if (this->isFullScreen())
            this->escWidget->show();
        break;
    }
    case QEvent::Leave: {
        this->escWidget->hide();
        break;
    }
    case QEvent::MouseButtonPress:
        this->escWidget->hide();
        break;
    default:
        break;
    }*/


    if (!mouse_drag_tracking)
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
        drag_state = NOT_DRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_DRAGGING;
            return false;
        }

        drag_state = START_DRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != DRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_DRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_DRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_DRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_DRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_DRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < DRAG_THRESHOLD && abs(diff.y()) < DRAG_THRESHOLD)
            return false;

        drag_state = DRAGGING;
    }

    emit mouseMovedDiff(diff);
    start_drag = pos;
    event->accept();
    return true;


























    //0621
//    switch (event->type()) {
//    /*case QEvent::Enter: {
//        if (this->hintWidget) {
//            this->hintWidget->hide();
//        }



////        QPoint p = parent->mapFromGlobal(mouse_event->globalPos());
//////                qDebug() << "33333AutohideWidget::eventFilter: y:" << p.y();
////        if (p.y() > (parent->height() - height() - spacing)) {
////            showWidget();
////        }
////        QPoint pos = mapFromGlobal(QCursor::pos());


////        QMouseEvent * mouse_event = dynamic_cast<QMouseEvent*>(event);
////        QWidget * parent = parentWidget();
////        QPoint p = parent->mapFromGlobal(mouse_event->globalPos());


//        QHelpEvent * help_event = static_cast<QHelpEvent *>(event);
//        qDebug() << "TimeSlider::event: total_time:" << total_time << "x:" << help_event->x();
//        int pos_in_slider = help_event->x() * maximum() / width();
//        int time = pos_in_slider * total_time / maximum();
//        qDebug() << "TimeSlider::event: time:" << time;
//        if (time >= 0 && time <= total_time) {
//            qDebug () << "test time=" << Helper::formatTime(time);
//            hintWidget->setText(Helper::formatTime(time));
//        }


//        QPoint centerPos = this->mapToGlobal(this->rect().center());
//        QSize sz = this->hintWidget->size();
//        centerPos.setX(centerPos.x()  - sz.width() / 2);
//        centerPos.setY(centerPos.y() - 32 - sz.height());
//        centerPos = this->hintWidget->mapFromGlobal(centerPos);
//        centerPos = this->hintWidget->mapToParent(centerPos);
//        this->hintWidget->move(centerPos);
//        this->hintWidget->show();
//        this->setCursor(QCursor(Qt::PointingHandCursor));
//        break;
//    }*/
//    case QEvent::Leave: {
//        if (this->hintWidget) {
//            this->hintWidget->hide();
//        }
//        this->unsetCursor();
//        break;
//    }
//    case QEvent::MouseButtonPress:
//        if (this->hintWidget) {
//            this->hintWidget->hide();
//        }
//        break;
//    default:
//        break;
//    }
//    return QObject::eventFilter(obj, event);
}

QSize MplayerWindow::sizeHint() const {
	//qDebug("MplayerWindow::sizeHint");
	return QSize( video_width, video_height );
}

QSize MplayerWindow::minimumSizeHint () const {
	return QSize(0,0);
}

void MplayerWindow::setOffsetX( int d) {
	offset_x = d;
	mplayerlayer->move( orig_x + offset_x, mplayerlayer->y() );
}

int MplayerWindow::offsetX() { return offset_x; }

void MplayerWindow::setOffsetY( int d) {
	offset_y = d;
	mplayerlayer->move( mplayerlayer->x(), orig_y + offset_y );
}

int MplayerWindow::offsetY() { return offset_y; }

void MplayerWindow::setZoom( double d) {//0526
    zoom_factor = d;
    offset_x = 0;
    offset_y = 0;

    int x = orig_x;
    int y = orig_y;
    int w = orig_width;
    int h = orig_height;

    if (zoom_factor != 1.0) {
        w = w * zoom_factor;
        h = h * zoom_factor;

        // Center
        x = (width() - w) / 2;
        y = (height() -h) / 2;
    }

    mplayerlayer->move(x,y);
    mplayerlayer->resize(w,h);//kobe:主界面全部显示视频刷新时会导致标题栏和控制栏重影  0526
}

double MplayerWindow::zoom() { return zoom_factor; }

void MplayerWindow::moveLayer( int offset_x, int offset_y ) {
	int x = mplayerlayer->x();
	int y = mplayerlayer->y();

	mplayerlayer->move( x + offset_x, y + offset_y );
}

void MplayerWindow::moveLeft() {
	if ((allow_video_movement) || (mplayerlayer->x()+mplayerlayer->width() > width() ))
		moveLayer( -16, 0 );
}

void MplayerWindow::moveRight() {
	if ((allow_video_movement) || ( mplayerlayer->x() < 0 ))
		moveLayer( +16, 0 );
}

void MplayerWindow::moveUp() {
	if ((allow_video_movement) || (mplayerlayer->y()+mplayerlayer->height() > height() ))
		moveLayer( 0, -16 );
}

void MplayerWindow::moveDown() {
	if ((allow_video_movement) || ( mplayerlayer->y() < 0 ))
		moveLayer( 0, +16 );
}

void MplayerWindow::incZoom() {
	setZoom( zoom_factor + ZOOM_STEP );
}

void MplayerWindow::decZoom() {
	double zoom = zoom_factor - ZOOM_STEP;
	if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;
	setZoom( zoom );
}

// Language change stuff
//void MplayerWindow::changeEvent(QEvent *e) {
//	if (e->type() == QEvent::LanguageChange) {
//		retranslateStrings();
//	} else {
//		QWidget::changeEvent(e);
//	}
//}

//#include "moc_mplayerwindow.cpp"
