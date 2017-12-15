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

#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include "myslider.h"
#include "config.h"
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

public slots:
	virtual void setPos(int); // Don't use setValue!
	virtual int pos();
	virtual void setDuration(double t) { total_time = t; };
	virtual double duration() { return total_time; };
    void setDragDelay(int);
    int dragDelay();

    void hideTip();//kobe

signals:
	void posChanged(int);
    void draggingPos(int);
//	//! Emitted with a few ms of delay
    void delayedDraggingPos(int);
	void wheelUp();
	void wheelDown();

    void active_status(bool);//kobe
    void need_to_save_pre_image(int time);//kobe

protected slots:
	void stopUpdate();
	void resumeUpdate();
	void mouseReleased();
    void valueChanged_slot(int);
    void checkDragging(int);
    void sendDelayedPos();

    void show_save_preview_image(int time, QString filepath);

protected:
	virtual void wheelEvent(QWheelEvent * e);
	virtual bool event(QEvent *event);

    void enterEvent(QEvent *event);//kobe
    void leaveEvent(QEvent *event);//kobe

    bool eventFilter(QObject *obj, QEvent *event);//kobe

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

