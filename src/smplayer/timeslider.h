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

#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include "myslider.h"

class TimeTip;

class TimeSlider : public MySlider 
{
	Q_OBJECT
//    Q_PROPERTY(bool status READ isStatus WRITE setStatus)//kobe

public:
	TimeSlider( QWidget * parent );
	~TimeSlider();

//    void setStatus(bool e) { status = e; }//kobe
//    bool isStatus() const { return status; }//kobe
    void show_time_value(int time);
    void set_preview_flag(bool b);
    void show_save_preview_image(int time, QString filepath);

public slots:
	virtual void setPos(int); // Don't use setValue!
	virtual int pos();
	virtual void setDuration(double t) { total_time = t; };
	virtual double duration() { return total_time; };
    void setDragDelay(int);
    int dragDelay();

    void hideTip();

signals:
	void posChanged(int);
    void draggingPos(int);
//	//! Emitted with a few ms of delay
    void delayedDraggingPos(int);
	void wheelUp();
	void wheelDown();

    void active_status(bool);
    void requestSavePreviewImage(int time, QPoint pos);

    void requestHideTip();

protected slots:
	void stopUpdate();
	void resumeUpdate();
	void mouseReleased();
    void valueChanged_slot(int);
    void checkDragging(int);
    void sendDelayedPos();



protected:
	virtual void wheelEvent(QWheelEvent * e);
	virtual bool event(QEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);

private:
	bool dont_update;
	int position;
	double total_time;

    TimeTip *hintWidget;
	int last_pos_to_send;
	QTimer * timer;
//    bool status;//kobe
    QPoint cur_pos;
    bool preview;
};

#endif

