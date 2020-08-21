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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "timeslider.h"
#include "../utils.h"

#include <QWheelEvent>
#include <QTimer>
#include <QToolTip>
#include <QDebug>
#include <QPixmapCache>
#include <QPixmap>
#include "../timetip.h"

TimeSlider::TimeSlider( QWidget * parent ) : MySlider(parent)
	, dont_update(false)
	, position(0)
	, total_time(0)
{
	setMinimum(0);
	setMaximum(SEEKBAR_RESOLUTION);

	setFocusPolicy( Qt::NoFocus );
	setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );

    preview = false;

	connect( this, SIGNAL( sliderPressed() ), this, SLOT( stopUpdate() ) );
	connect( this, SIGNAL( sliderReleased() ), this, SLOT( resumeUpdate() ) );
	connect( this, SIGNAL( sliderReleased() ), this, SLOT( mouseReleased() ) );
    connect( this, SIGNAL( valueChanged(int) ), this, SLOT( valueChanged_slot(int) ) );
    connect( this, SIGNAL(draggingPos(int) ), this, SLOT(checkDragging(int)) );
    cur_pos = QPoint(0,0);
    last_pos_to_send = -1;
    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(sendDelayedPos()) );
    timer->start(200);

    hintWidget = new TimeTip("00:00:00", this);
//    hintWidget->setFixedHeight(60);
    hintWidget->setFixedSize(67,60);//kobe: 如果不设置固定宽度，则宽度初始值默认为100,后续自动变为67,导致第一次使用默认值时显示的时间信息的坐标有偏移
    installEventFilter(this);
}

TimeSlider::~TimeSlider() {
}

void TimeSlider::hideTip()
{
    this->hintWidget->hide();
}

void TimeSlider::stopUpdate() {
	dont_update = true;
}

void TimeSlider::resumeUpdate() {
	dont_update = false;
}

void TimeSlider::mouseReleased() {
	emit posChanged( value() );
}

void TimeSlider::valueChanged_slot(int v) {
    // Only to make things clear:
    bool dragging = dont_update;
    if (!dragging) {
        if (v!=position) {
            emit posChanged(v);
        }
    } else {
        emit draggingPos(v);
    }
}

void TimeSlider::setDragDelay(int d) {
//	qDebug("TimeSlider::setDragDelay: %d", d);
    timer->setInterval(d);
}

int TimeSlider::dragDelay() {
    return timer->interval();
}

void TimeSlider::checkDragging(int v) {
//	qDebug("TimeSlider::checkDragging: %d", v);
    last_pos_to_send = v;
}

void TimeSlider::sendDelayedPos() {
//    qDebug("TimeSlider::sendDelayedPos: %d", last_pos_to_send);
    if (last_pos_to_send != -1) {
        emit delayedDraggingPos(last_pos_to_send);
        last_pos_to_send = -1;
    }
}

void TimeSlider::setPos(int v) {
	if (v!=pos()) {
		if (!dont_update) {
			position = v;
			setValue(v);
		}
	}
}

int TimeSlider::pos() {
	return position;
}

void TimeSlider::wheelEvent(QWheelEvent * e) {
	//e->ignore();

//	qDebug("TimeSlider::wheelEvent: delta: %d", e->delta());
	e->accept();

	if (e->orientation() == Qt::Vertical) {
		if (e->delta() >= 0)
			emit wheelUp();
		else
			emit wheelDown();
	} else {
		qDebug("Timeslider::wheelEvent: horizontal event received, doing nothing");
	}
}

void TimeSlider::enterEvent(QEvent * event)
{
    emit this->active_status(true);

//    event->ignore();
//    QSlider::enterEvent(event);
}

void TimeSlider::leaveEvent(QEvent * event)
{
    emit this->active_status(false);

    emit requestHideTip();
    event->ignore();
    QSlider::leaveEvent(event);
}

void TimeSlider::show_time_value(int time)
{
//    hintWidget->setFixedHeight(60);
    hintWidget->setFixedSize(67,60);//kobe: 如果不设置固定宽度，则宽度初始值默认为100,后续自动变为67,导致第一次使用默认值时显示的时间信息的坐标有偏移
    hintWidget->setText(Utils::formatTime(time));
    QPoint curPos = this->mapToGlobal(cur_pos);
    QSize sz = this->hintWidget->size();
    curPos.setX(curPos.x()  - sz.width() / 2);
    curPos.setY(curPos.y() - sz.height());//- 32
    curPos = this->hintWidget->mapFromGlobal(curPos);
    curPos = this->hintWidget->mapToParent(curPos);
    this->hintWidget->move(curPos);
    this->hintWidget->show();
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void TimeSlider::set_preview_flag(bool b) {
    preview = b;
    if (!preview) {
        hintWidget->setFixedSize(67,60);
    }
}

void TimeSlider::show_save_preview_image(int time, QString filepath)
{
    if (filepath.isEmpty()) {
        this->show_time_value(time);
    }
    else {
        QPixmapCache::clear();
//        QPixmapCache::setCacheLimit(1);
        QPixmap picture;
        if (!picture.load(filepath)) {
            this->show_time_value(time);
            return;
        }
        QPixmap scaled_picture = picture.scaledToWidth(200, Qt::SmoothTransformation);
//        hintWidget->setFixedHeight(200);
        hintWidget->setFixedSize(200, scaled_picture.size().height() + 30);//kobe: 如果不设置固定宽度，则宽度初始值默认为100,后续自动变为67,导致第一次使用默认值时显示的时间信息的坐标有偏移
        hintWidget->setPixMapAndTime(scaled_picture, Utils::formatTime(time));
        QPoint curPos = this->mapToGlobal(cur_pos);
        QSize sz = this->hintWidget->size();
        curPos.setX(curPos.x()  - sz.width() / 2);
        curPos.setY(curPos.y() - sz.height());//- 32
        curPos = this->hintWidget->mapFromGlobal(curPos);
        curPos = this->hintWidget->mapToParent(curPos);
        this->hintWidget->move(curPos);
        this->hintWidget->show();
        this->setCursor(QCursor(Qt::PointingHandCursor));
    }
}

//kobe:鼠标放在进度条上时显示时间进度，移开鼠标则消失
bool TimeSlider::event(QEvent *event) {
	if (event->type() == QEvent::ToolTip) {
		QHelpEvent * help_event = static_cast<QHelpEvent *>(event);
#if 0
		int pos_in_slider = help_event->x() * maximum() / width();
        cur_pos = help_event->pos();
		int time = pos_in_slider * total_time / maximum();
#else
        cur_pos = help_event->pos();
        int time = help_event->x() * total_time / width();
//        qDebug() << "bybobbi in TimeSlider's event: width() = " << width() << ", cur_pos->x is     " << cur_pos.x() << ". time is " << time;
#endif
		if (time >= 0 && time <= total_time) {
            //QToolTip::showText(help_event->globalPos(), Utils::formatTime(time), this);
            if (preview) {
                emit this->requestSavePreviewImage(time, cur_pos);
            }
            else {
                hintWidget->setText(Utils::formatTime(time));
    //            QPoint centerPos = this->mapToGlobal(this->rect().center());
    //            QSize sz = this->hintWidget->size();
    //            centerPos.setX(centerPos.x()  - sz.width() / 2);
    //            centerPos.setY(centerPos.y() - 32 - sz.height());
    //            centerPos = this->hintWidget->mapFromGlobal(centerPos);
    //            centerPos = this->hintWidget->mapToParent(centerPos);
    //            this->hintWidget->move(centerPos);
                QPoint curPos = this->mapToGlobal(help_event->pos());
                QSize sz = this->hintWidget->size();
                curPos.setX(curPos.x()  - sz.width() / 2);
                curPos.setY(curPos.y() - sz.height());//- 32
                curPos = this->hintWidget->mapFromGlobal(curPos);
                curPos = this->hintWidget->mapToParent(curPos);
                this->hintWidget->move(curPos);
                this->hintWidget->show();
                this->setCursor(QCursor(Qt::PointingHandCursor));
            }
//
		} else {
            cur_pos = QPoint(0,0);
//            QToolTip::hideText();
			event->ignore();
		}
		return true;
	}
	return QWidget::event(event);
}

bool TimeSlider::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Leave: {
        if (this->hintWidget) {
            this->hintWidget->hide();
        }
        this->unsetCursor();
        break;
    }
    case QEvent::MouseButtonPress:
        if (this->hintWidget) {
            this->hintWidget->hide();
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}
