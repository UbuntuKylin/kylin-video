/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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

#ifndef _AUDIODELAYDIALOG_H_
#define _AUDIODELAYDIALOG_H_

#include "ui_audiodelaydialog.h"

#include <QDialog>
#include <QPushButton>

enum AADragState {NOT_AADRAGGING, START_AADRAGGING, AADRAGGING};

class AudioDelayDialog : public QDialog, public Ui::AudioDelayDialog
{
	Q_OBJECT

public:
    AudioDelayDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~AudioDelayDialog();

    void setDefaultValue(int audio_delay);
    int getCurrentValue();

    void initConnect();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

public slots:


private:
    AADragState drag_state;
    QPoint start_drag;
};

#endif
